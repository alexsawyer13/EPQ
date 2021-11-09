#include <Core/Game.h>
#include <Core/Core.h>
#include <Core/Input.h>
#include <Core/Profiler.h>
#include <Core/Assets.h>
#include <Game/Chunk.h>
#include <Game/World.h>
#include <Game/Player.h>
#include <Graphics/Shader.h>
#include <Graphics/Cubemap.h>
#include <Graphics/Buffers.h>
#include <Graphics/BatchRenderer.h>
#include <Physics/VoxelRayCast.h>

#include <spdlog/spdlog.h>
#include <stb/stb_image.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <spdlog/spdlog.h>

#include <cstdio>
#include <queue>
#include <chrono>
#include <filesystem>

constexpr int DEFAULT_WIDTH = 1366;
constexpr int DEFAULT_HEIGHT = 768;
GLenum SeverityLevel = GL_DEBUG_SEVERITY_LOW;

int frameCount = 0;
float fps = 0.0f;
ProfilerFrame frame;

unsigned int currentAnimFrame = 0;
std::chrono::system_clock::time_point lastAnimFrame;
long long animDelay = 100.0f;
constexpr int crosshair_size = 20.0f;

glm::vec3 default_player_position = glm::vec3(0.0f, 75.0f, 0.0f);

bool ImGui_enabled = true;

void Start()
{
	// Setup
	{
		SCOPE_TIMER_MS("Setup");

		core.profiler.session_string = GetTimeFormatted(); // TODO: MOVE THIS
		printf("Session string: \"%s\"\n", core.profiler.session_string.c_str());
		std::filesystem::create_directories(s_ProjectDir + "profiler/" + core.profiler.session_string);
		
		// Setup spdlog
		spdlog::info("Running spdlog version {}.{}.{}", SPDLOG_VER_MAJOR, SPDLOG_VER_MINOR, SPDLOG_VER_PATCH);
		spdlog::set_level(spdlog::level::trace);

		// Initialise GLFW
		if (!glfwInit())
		{
			spdlog::critical("Failed to initialise GLFW");
			throw;
		}

		// Create GLFW window
		core.window = glfwCreateWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, "Hello, world", NULL, NULL);
		if (!core.window)
		{
			spdlog::critical("Failed to create GLFW window");
			throw;
		}

		glfwMakeContextCurrent(core.window);
		glfwSwapInterval(1);

		// Load OpenGL
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			spdlog::critical("Failed to load OpenGL");
			throw;
		}

		// Set GL debug callback
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(MessageCallback, 0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Setup ImGUI
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO &io = ImGui::GetIO(); (void)io;
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(core.window, true);
		ImGui_ImplOpenGL3_Init("#version 130");

		// Load stuff
		LoadCoreData();
		InputSetup();

		// Center window
		const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		if (!mode)
			return;

		int monitorX, monitorY;
		glfwGetMonitorPos(glfwGetPrimaryMonitor(), &monitorX, &monitorY);

		int windowWidth, windowHeight;
		glfwGetWindowSize(core.window, &windowWidth, &windowHeight);

		glfwSetWindowPos(core.window,
			monitorX + (mode->width - windowWidth) / 2,
			monitorY + (mode->height - windowHeight) / 2);

		InputToggleCursorGrab();

		// Setup objects
		core.player.Position = default_player_position;
		core.player.Pitch = -89.0f;
		core.player.EnableFlight = true;

		lastAnimFrame = std::chrono::system_clock::now();
	}

	while (!glfwWindowShouldClose(core.window))
	{
		Loop();
	}

	// Cleanup ImGUI
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// Cleanup objects
	WorldDestroy(&core.world);
	FreeCoreData();
	BatchDestroy(&core.uirenderer);

	// Cleanup GLFW
	glfwDestroyWindow(core.window);
	glfwTerminate();
}

void Loop()
{
	START_PROFILER_FRAME();

	frameCount++;

	// Get window info
	int width, height;
	glfwGetFramebufferSize(core.window, &width, &height);

	// Prepare OpenGL context
	glViewport(0, 0, width, height);
	glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// Update function
	Update();

	// OpenGL render
	OpenGLRender(width, height);

	// Render ImGUI frame
	if (ImGui_enabled)
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuiRender();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	// Swap buffers
	glfwSwapBuffers(core.window);
	
	END_PROFILER_FRAME();
}

void Update()
{
	PROFILE_SCOPE_US("Update");

	// Handle input
	{
		PROFILE_SCOPE_US("InputUpdate");
		InputUpdate();
	}

	if (core.input.Keys[GLFW_KEY_ESCAPE] == Pressed)
		InputToggleCursorGrab();

	if (core.input.Keys[GLFW_KEY_F3] == Pressed)
	{
		ImGui_enabled = !ImGui_enabled;
	}

	{
		PROFILE_SCOPE_US("PlayerUpdate");
		PlayerUpdate(&core.player, &core.world);
	}
	{
		PROFILE_SCOPE_US("WorldUpdate");
		WorldUpdate(&core.world);
	}

	// Test raycasting

	{
		PROFILE_SCOPE_US("Raycasting");

		if (core.input.Keys[GLFW_MOUSE_BUTTON_LEFT] == Pressed)
		{
			Raycast raycast = VoxelRayCast(&core.world, core.player.Position, core.player.Direction);
			if (raycast.Hit)
				WorldBreakBlock(&core.world, raycast.Block.x, raycast.Block.y, raycast.Block.z);
		}

		if (core.input.Keys[GLFW_MOUSE_BUTTON_RIGHT] == Pressed)
		{
			Raycast raycast = VoxelRayCast(&core.world, core.player.Position, core.player.Direction);
			if (raycast.Hit)
				WorldSetBlock(&core.world, raycast.Block.x + raycast.Normal.x, raycast.Block.y + raycast.Normal.y, raycast.Block.z + raycast.Normal.z, core.BlockIds["oak_planks"]);
		}

		if (core.input.Keys[GLFW_KEY_K] == Pressed)
			core.player.EnableFlight = !core.player.EnableFlight;
	}

	// Csv stuff

#ifdef PROFILER_ENABLE_CSV
	{
		PROFILE_SCOPE_US("CsvStuff");

		CsvAddData(core.profiler.current_csv, "Frame", frameCount);
		CsvAddData(core.profiler.current_csv, "Frametime", (int)(core.input.DeltaTime * 1000000));
		for (auto micro : core.profiler.last_frame.Microseconds)
		{
			CsvAddData(core.profiler.current_csv, micro.first, micro.second);
		}
		for (auto milli : core.profiler.last_frame.Milliseconds)
		{
			CsvAddData(core.profiler.current_csv, milli.first, milli.second * 1000);
		}
		CsvNextRow(core.profiler.current_csv);
	}
#endif
}

void OpenGLRender(int width, int height)
{
	PROFILE_SCOPE_US("OpenGLRender");

	glm::mat4 view = core.player.View;
	glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 1000.0f);
	glm::mat4 ortho = glm::ortho(0.0f, (float)width, 0.0f, (float)height);

	// Cubemap
	// TODO: Render after everything else with fancy depth testing
	core.cubemap.Render(view, proj);

	{
		PROFILE_SCOPE_US("DrawChunks");

		// Draw chunks
		core.shaders["optimisedtexarray"].Bind();
		core.shaders["optimisedtexarray"].SetMat4("u_View", view);
		core.shaders["optimisedtexarray"].SetMat4("u_Proj", proj);
		core.shaders["optimisedtexarray"].SetInt("u_Texture", 0);

		TexArrayBind(&core.block_texarray, 0);

		auto currentTime = std::chrono::system_clock::now();
		auto timeSinceLastAnimFrame = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastAnimFrame).count();

		if (timeSinceLastAnimFrame > animDelay)
		{
			currentAnimFrame++;
			core.shaders["optimisedtexarray"].SetUnsignedInt("u_CurrentAnimationFrame", currentAnimFrame);
			lastAnimFrame = currentTime;
		}

		{
			PROFILE_SCOPE_US("WorldDrawChunks");
			WorldDrawChunks(&core.world);
		}
	}

	{
		PROFILE_SCOPE_US("DrawUI");

		glEnable(GL_BLEND);

		core.shaders["batch"].Bind();
		core.shaders["batch"].SetMat4("u_Proj", ortho);

		// Crosshair
		core.uirenderer.Quads.push_back({
				{float(width - crosshair_size) / 2, float(height - crosshair_size) / 2, 0},
				{(float)crosshair_size, (float)crosshair_size},
				&core.textures["crosshair"]
			});

		// Hotbar
		for (int i = 0; i < 9; i++)
		{
			core.uirenderer.Quads.push_back({
				{(width-9*64)/2 + i*64, 0, 0},
				{64, 64},
				&core.textures["hotbar_icon"]
				});
		}

		glClear(GL_DEPTH_BUFFER_BIT);
		BatchFlush(&core.uirenderer);

		glDisable(GL_BLEND);
	}
}

int frame_delay = 30;

void ImGuiRender()
{
	PROFILE_SCOPE_US("ImGuiRender");


	if (frameCount % frame_delay == 0)
	{
		fps = 1.0f / core.input.DeltaTime;
		frame = core.profiler.last_frame;
	}

	int animDelayInt = (int)animDelay;

	ImGui::Begin("Player Info");
	ImGui::Text("Position: (%f, %f, %f)", core.player.Position.x, core.player.Position.y, core.player.Position.z);
	ImGui::Text("Camera: (%f, %f)", core.player.Pitch, core.player.Yaw);
	if (ImGui::Button("Reset player position"))
	{
		core.player.Position = default_player_position;
	}
	ImGui::End();

	ImGui::Begin("Debug");

	ImGui::SliderFloat("Speed", &core.player.BaseSpeed, 0.0f, 25.0f, nullptr, 0);
	ImGui::SliderInt("Animation delay (ms)", &animDelayInt, 1, 1000, nullptr);
	ImGui::End();

	ImGui::Begin("Performance");
	ImGui::Text("FPS: %f", fps);
	ImGui::Text("Chunk updates queued: %d", core.world.PendingMesh.size());
	ImGui::SliderInt("frame_delay", &frame_delay, 1, 100);

#ifdef PROFILER_ENABLE_PROFILER
	if (ImGui::BeginTable("Function Times", 2))
	{
		ImGui::TableSetupColumn("Function");
		ImGui::TableSetupColumn("Time");
		ImGui::TableHeadersRow();

		for (auto micro : frame.Microseconds)
		{
			ImGui::TableNextColumn();
			ImGui::Text("%s", micro.first.c_str());
			ImGui::TableNextColumn();
			ImGui::Text("%5lld us", micro.second);
			ImGui::TableNextRow();
		}
		for (auto milli : frame.Milliseconds)
		{
			ImGui::TableNextColumn();
			ImGui::Text("%s", milli.first.c_str());
			ImGui::TableNextColumn();
			ImGui::Text("%5lld ms", milli.second);
			ImGui::TableNextRow();
		}
		ImGui::EndTable();
	}

	if (ImGui::Button("Create profiler csv"))
	{
		CsvFlush(core.profiler.current_csv);
	}

	ImGui::TextWrapped("Most recent csv: %s", core.profiler.most_recent_csv.c_str());

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	if (ImGui::Button("Open profiler csv"))
	{
		if (!core.profiler.most_recent_csv.empty())
		{
			std::string copy = core.profiler.most_recent_csv;
			for (auto &c : copy)
			{
				if (c == '/') c = '\\';
			}
			std::string command = "start \"csv\" \"" + core.profiler.most_recent_csv + "\"";
			system(command.c_str());
		}
	}

	if (ImGui::Button("Open session directory"))
	{
		std::string command = "explorer \"" + s_ProjectDir + "profiler/" + core.profiler.session_string + "\"";
		for (char &c : command)
		{
			if (c == '/') c = '\\';
		}
		system(command.c_str());
	}
#endif

	if (ImGui::Button("Clear CSV data"))
	{
		core.profiler.current_csv.columns.clear();
		core.profiler.current_csv.hashed_columns.clear();
		core.profiler.current_csv.rows.clear();
		core.profiler.current_csv.current_row.clear();
	}

	ImGui::Spacing();

	if (ImGui::Button("Clear session data"))
	{
		try
		{
			std::filesystem::remove_all(s_ProjectDir + "profiler/" + core.profiler.session_string);
			std::filesystem::create_directories(s_ProjectDir + "profiler/" + core.profiler.session_string);
			core.profiler.most_recent_csv.clear();
		}
		catch (...)
		{
			spdlog::error("Failed to clear session data");
		}
	}

	if (ImGui::Button("Clear profiler data"))
	{
		try
		{
			std::filesystem::remove_all(s_ProjectDir + "profiler");
			std::filesystem::create_directories(s_ProjectDir + "profiler/" + core.profiler.session_string);
			core.profiler.most_recent_csv.clear();
		}
		catch (...)
		{
			spdlog::error("Failed to clear profiler data");
		}
	}
#endif

	ImGui::End();

	animDelay = (int)animDelayInt;
}

void MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
	switch (severity)
	{
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		if (SeverityLevel == GL_DEBUG_SEVERITY_NOTIFICATION)
			spdlog::debug("Notification severity GL message: {}", message);
		break;

	case GL_DEBUG_SEVERITY_LOW:
		if (SeverityLevel >= GL_DEBUG_SEVERITY_LOW || severity == GL_DEBUG_SEVERITY_NOTIFICATION)
			spdlog::warn("Low severity GL message: {}", message);
		break;

	case GL_DEBUG_SEVERITY_MEDIUM:
		if (SeverityLevel >= GL_DEBUG_SEVERITY_MEDIUM || severity == GL_DEBUG_SEVERITY_NOTIFICATION)
			spdlog::error("Medium severity GL message: {}", message);
		break;

	case GL_DEBUG_SEVERITY_HIGH:
		if (SeverityLevel >= GL_DEBUG_SEVERITY_HIGH || severity == GL_DEBUG_SEVERITY_NOTIFICATION)
			spdlog::critical("High severity GL message: {}", message);
		break;
	}
}
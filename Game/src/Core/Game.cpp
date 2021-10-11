#include <Core/Game.h>
#include <Core/Core.h>
#include <Core/Input.h>
#include <Game/Chunk.h>
#include <Game/World.h>
#include <Game/Player.h>
#include <Graphics/Shader.h>
#include <Graphics/TextureArray.h>
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

constexpr int DEFAULT_WIDTH = 1600;
constexpr int DEFAULT_HEIGHT = 900;
GLenum SeverityLevel = GL_DEBUG_SEVERITY_HIGH;

int frameCount = 0;
float fps = 0.0f;

unsigned int currentAnimFrame = 0;
std::chrono::system_clock::time_point lastAnimFrame;
long long animDelay = 100.0f;
constexpr int crosshair_size = 20.0f;

void Run()
{
	while (!glfwWindowShouldClose(core.window))
	{
		Loop();
	}
}

void Loop()
{
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

	// ImGui render
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGuiRender();

	// OpenGL render
	OpenGLRender(width, height);

	// Render ImGUI frame
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// Swap buffers
	glfwSwapBuffers(core.window);
}

void Setup()
{
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
	core.player.Position = glm::vec3(0.0f, 75.0f, 0.0f);
	core.player.Pitch = -89.0f;
	core.player.EnableFlight = true;

	lastAnimFrame = std::chrono::system_clock::now();
}

void Cleanup()
{
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

void Update()
{
	// Handle input
	InputUpdate();

	if (core.input.Keys[GLFW_KEY_ESCAPE] == Pressed)
		InputToggleCursorGrab();

	PlayerUpdate(&core.player, &core.world);
	WorldUpdate(&core.world);

	// Test raycasting

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
			WorldSetBlock(&core.world, raycast.Block.x + raycast.Normal.x, raycast.Block.y + raycast.Normal.y, raycast.Block.z + raycast.Normal.z, core.BlockIds["furnace_on"]);
	}

	if (core.input.Keys[GLFW_KEY_K] == Pressed)
		core.player.EnableFlight = !core.player.EnableFlight;
}

void OpenGLRender(int width, int height)
{
	glm::mat4 view = core.player.View;
	glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 1000.0f);
	glm::mat4 ortho = glm::ortho(0.0f, (float)width, 0.0f, (float)height);

	// Cubemap
	// TODO: Render after everything else with fancy depth testing
	core.cubemap.Render(view, proj);

	// Draw chunks
	core.shaders["optimisedtexarray"].Bind();
	core.shaders["optimisedtexarray"].SetMat4("u_View", view);
	core.shaders["optimisedtexarray"].SetMat4("u_Proj", proj);
	core.shaders["optimisedtexarray"].SetInt("u_Texture", 0);

	core.block_texarray.Bind(0);

	auto currentTime = std::chrono::system_clock::now();
	auto timeSinceLastAnimFrame = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastAnimFrame).count();

	if (timeSinceLastAnimFrame > animDelay)
	{
		currentAnimFrame++;
		core.shaders["optimisedtexarray"].SetUnsignedInt("u_CurrentAnimationFrame", currentAnimFrame);
		lastAnimFrame = currentTime;
	}

	WorldDrawChunks(&core.world);


	// Draw UI

	glEnable(GL_BLEND);

	core.shaders["batch"].Bind();
	core.shaders["batch"].SetMat4("u_Proj", ortho);

	// Crosshair
	core.uirenderer.Quads.push_back({
			{float(width - crosshair_size) / 2, float(height - crosshair_size) / 2, 0},
			{(float)crosshair_size, (float)crosshair_size},
			&core.textures["crosshair"]
		});

	glClear(GL_DEPTH_BUFFER_BIT);
	BatchFlush(&core.uirenderer);


	glDisable(GL_BLEND);
}

void ImGuiRender()
{
	ImGui::Begin("Camera");
	ImGui::SliderFloat("Pitch", &core.player.Pitch, -90.0f, 90.0f, nullptr, 0);
	ImGui::SliderFloat("Yaw", &core.player.Yaw, -180.0f, 180.0f, nullptr, 0);
	ImGui::SliderFloat3("Position", &core.player.Position[0], -10.0f, 10.0f, nullptr, 0);
	ImGui::SliderFloat("Speed", &core.player.Speed, 0.0f, 10.0f, nullptr, 0);
	ImGui::SliderFloat("Sensitivity", &core.player.Sensitivity, 0.0f, 10.0f, nullptr, 0);
	ImGui::End();

	if (frameCount % 10 == 0)
		fps = 1.0f / core.input.DeltaTime;

	int animDelayInt = animDelay;

	ImGui::Begin("Info");
	ImGui::Text("FPS: %f", fps);
	ImGui::SliderInt("Animation delay (ms)", &animDelayInt, 1, 1000, nullptr);
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
#include <Core/Input.h>
#include <Core/Core.h>
#include <Core/Profiler.h>

#include <spdlog/spdlog.h>
#include <imgui/imgui.h>

void InputSetup()
{
	core.input.CurrentTime = glfwGetTime();

	core.input.IsMouseCaptured = false;
	core.input.IgnoreMouse = true;

	glfwSetKeyCallback(core.window, glfw_key_callback);
	glfwSetMouseButtonCallback(core.window, glfw_mouse_button_callback);
}

void InputUpdate()
{
	// Update delta time

	core.input.CurrentTime = glfwGetTime();
	core.input.DeltaTime = core.input.CurrentTime - core.input.LastTime;
	core.input.LastTime = core.input.CurrentTime;

	// Update key statuses

	for (auto iter = core.input.Keys.begin(); iter != core.input.Keys.end(); iter++)
	{
		if (iter->second == Pressed)
			iter->second = Down;
		if (iter->second == Released)
			iter->second = Up;
	}

	// Poll GLFW events for callback functions

	{
		PROFILE_SCOPE_US("glfwPollEvents");
		glfwPollEvents();
	}

	// Update mouse location

	glfwGetCursorPos(core.window, &core.input.MouseX, &core.input.MouseY);

	if (core.input.IgnoreMouse)
	{
		core.input.IgnoreMouse = false;
	}
	else
	{
		core.input.DeltaX = core.input.MouseX - core.input.LastX;
		core.input.DeltaY = core.input.LastY - core.input.MouseY;
	}

	core.input.LastX = core.input.MouseX;
	core.input.LastY = core.input.MouseY;
}

void InputToggleCursorGrab()
{
	if (core.input.IsMouseCaptured)
	{
		glfwSetInputMode(core.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
		core.input.IsMouseCaptured = false;
	}
	else
	{
		glfwSetInputMode(core.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
		core.input.IsMouseCaptured = true;
	}
	core.input.IgnoreMouse = true;
}

void glfw_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	switch (action)
	{
	case GLFW_PRESS:
		core.input.Keys[key] = Pressed;
		break;
	case GLFW_RELEASE:
		core.input.Keys[key] = Released;
		break;
	default:
		return;
	}
}

void glfw_mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
	switch (action)
	{
	case GLFW_PRESS:
		core.input.Keys[button] = Pressed;
		break;
	case GLFW_RELEASE:
		core.input.Keys[button] = Released;
		break;
	default:
		return;
	}
}
#pragma once

#include <glfw/glfw3.h>

#include <unordered_map>

enum KeyState
{
	Down = 1,     // 0001
	Pressed = 3,  // 0011
	Up = 4,       // 0100
	Released = 12, // 1100
};

struct Input
{
	std::unordered_map<int, KeyState> Keys;

	bool IsMouseCaptured, IgnoreMouse;
	double MouseX, MouseY, LastX, LastY, DeltaX, DeltaY;

	double CurrentTime, LastTime, DeltaTime;
};

void InputSetup();
void InputUpdate();
void InputToggleCursorGrab();

void glfw_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void glfw_mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
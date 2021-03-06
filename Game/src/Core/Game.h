#pragma once

#include <GLFW/glfw3.h>
#include <glad/glad.h>

void Start();
void Loop();

void Update();
void OpenGLRender(int width, int height);
void ImGuiRender();

void MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);
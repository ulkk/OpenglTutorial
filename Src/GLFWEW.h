/**
* @file GLFWEW.h
*/
#ifndef GLFWEW_INCLUDED
#define GLFWEW_INCLUDED
#include<GL/glew.h>
#include<GLFW\glfw3.h>
#include "glm/gtc/matrix_transform.hpp"

namespace GLFWEW {

/**
* GLEWのラッパークラス
*/
	class Window
	{
	public:
		static Window& Instance();
		bool Init(int w, int h, const char* title);
		void SwapBuffers() const;
		bool ShouldClose() const;

	private:
		Window();
		~Window();
		Window(const Window&) = delete;

		bool isGLFWInitialized;
		bool isInitialized;

		GLFWwindow* window;
};
}//namespace GLFWEW

#endif //GLFWEW_INCLUDED
#pragma once

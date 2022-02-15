#include "MyPhoton.h"

#include <iostream>
#include <string>
#include <thread>
#include <mutex>

#include <windows.h>
#include <GL/GLU.h>
#include <GLFW/glfw3.h>

using namespace std;

const int RESOLUTION_X = 800;
const int RESOLUTION_Y = 600;

std::mutex MutexLock;

void onWindowResized(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//! Use ortho2D view
	gluOrtho2D(0, width, 0, height);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void MessengerFunc()
{
	string inputString;
	while (true)
	{
		//std::wcout << "Send: ";
		getline(cin, inputString);
		std::cout << "\b\b";

		MutexLock.lock();
		MyPhoton::getInstance().sendMyStringData(inputString);
		MutexLock.unlock();

		Sleep(100);
	}
}

void MessengerByteFunc()
{
	while (1)
	{
		string inputString;
		std::getline(std::cin, inputString);

		std::cout << "Me : " << inputString << std::endl;
		// Encrypt message
		for (int i = 0; i < inputString.length(); i++)
		{
			inputString[i] = inputString[i] ^ charCipherKey;
		}
		//
		std::cout << "Me [Encrypted]: " << inputString << "\n\n";

		MutexLock.lock();
		//MyPhoton::getInstance().sendString(mystring.c_str());
		MyPhoton::getInstance().sendMyByteData(inputString.c_str(), inputString.length());
		MutexLock.unlock();

		Sleep(100);
	}
}

int main (void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow (RESOLUTION_X, RESOLUTION_Y, "", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwSetWindowSizeCallback (window, onWindowResized);
	
	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	onWindowResized (window, RESOLUTION_X, RESOLUTION_Y);

	MyPhoton::getInstance().connect();

	std::thread Thread1(&MessengerByteFunc);

	while (!glfwWindowShouldClose(window))
	{
		MutexLock.lock();
		MyPhoton::getInstance().run();
		MutexLock.unlock();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	Thread1.join();

	glfwTerminate();
	return 0;
}
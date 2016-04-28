#include "Debug.h"
#include "DebugEngine.h"
#include "Window/Window.h"

void DebugSystem::Log(const std::string& log){
	Window::AddLog(log);
}

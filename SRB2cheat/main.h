#pragma once
#include <Windows.h>
#include <iostream>

//imGui window
void ImGuiWindow();
//setup hooks and enter render loop
int hack(HMODULE hModule);
//used to update memory using imgui triggers
void hackLoop();
//opens hack thread
int WINAPI main(HMODULE hModule);

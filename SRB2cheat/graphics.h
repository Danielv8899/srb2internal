#pragma once
#include "main.h"
#include "HookObj.h"



	using wglSwapBuffers_t = void(__stdcall*)( _In_ HDC hDc);
	extern "C" wglSwapBuffers_t wglSwapBuffersTrampoline;
	extern "C" void __stdcall RenderHook(_In_ HDC hDc);
	LRESULT CALLBACK windowProc_hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


	HWND hGameWindow;
	WNDPROC hGameWindowProc;

	class opengl : public HookObj{
		public:
			opengl();
			~opengl();
			int init();

		//LPVOID wglSwapBuffersHookAddr;
		HMODULE hOpengl32;
		PHOOK_OBJECT wglSwapBuffersObj;
	};

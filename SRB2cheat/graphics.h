#pragma once
#include "HookObj.h"
#include <Windows.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/glew.h"
#include "imgui/imgui_impl_win32.h"
#include "includes/gl.h"
#pragma comment(lib,"Ws2_32.lib")


	using wglSwapBuffers_t = void(__stdcall*)( _In_ HDC hDc);
	using glColor4f_t = void(__stdcall*)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
	using glEnable_t = void(__stdcall*)(GLenum cap);
	using glDisable_t = void(__stdcall*)(GLenum cap);
	using glEnableClientState_t = void(__stdcall*)(GLenum cap);
	using glDisableClientState_t = void(__stdcall*)(GLenum cap);
	using glDrawElements_t = void(__stdcall*)(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);
	using glBegin_t = void(__stdcall*)(GLenum mode);
	using glEnd_t = void(__stdcall*)();
	using glVertex_t = void(__stdcall*)(GLfloat x, GLfloat y, GLfloat z);
	using glDepthFunc_t = void(__stdcall*)(GLenum func);

	LRESULT CALLBACK windowProc_hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	extern HWND hGameWindow;
	extern WNDPROC hGameWindowProc;
	extern "C" wglSwapBuffers_t wglSwapBuffersTrampoline;

	class opengl : public HookObj{
		public:
			opengl();
			~opengl();
			int init();

		//LPVOID wglSwapBuffersHookAddr;
		HMODULE hOpengl32;
		PHOOK_OBJECT wglSwapBuffersObj;
		PHOOK_OBJECT glColor4fObj;
		PHOOK_OBJECT glEnableObj;
		PHOOK_OBJECT glDisableObj;
		PHOOK_OBJECT glEnableClientStateObj;
		PHOOK_OBJECT glDisableClientStateObj;
		PHOOK_OBJECT glDrawElementsObj;
		PHOOK_OBJECT glBeginObj;
		PHOOK_OBJECT glEndObj;
		PHOOK_OBJECT glVertexObj;
		PHOOK_OBJECT glDepthFuncObj;

	};

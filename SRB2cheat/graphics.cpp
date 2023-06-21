#include "graphics.h"

opengl::opengl()
{
	this->hOpengl32 = nullptr;
	this->wglSwapBuffersObj = nullptr;
	wglSwapBuffersTrampoline = nullptr;
}

opengl::~opengl()
{
}

int opengl::init() {

	this->hOpengl32 = GetModuleHandle(L"opengl32.dll");
	if (!this->hOpengl32) {
		printf("failed getting handle to opengl32.dll\n");
		exit(-1);
	}

	LPVOID wglSwapBuffersHookAddr = GetProcAddress(this->hOpengl32, "wglSwapBuffers");
	if (!wglSwapBuffersHookAddr) {
		printf("failed getting address of wglSwapBuffers\n");
		exit(-1);
	}

	this->wglSwapBuffersObj = Trampo::CreateHook(RenderHook, wglSwapBuffersHookAddr, 5);
	if (!this->wglSwapBuffersObj) {
		exit(-1);
	}
	wglSwapBuffersTrampoline = (wglSwapBuffers_t)wglSwapBuffersObj->trampoline;
	return 0;
}

extern "C" void __stdcall RenderHook(_In_ HDC hDc) {

    static bool imGuiInitialized = false;
    if (!imGuiInitialized) {
        imGuiInitialized = true;

        hGameWindow = WindowFromDC(hDc);

        if (!hGameWindowProc)
            hGameWindowProc = (WNDPROC)SetWindowLongPtr(hGameWindow,
                GWLP_WNDPROC, (LONG_PTR)windowProc_hook);

        glewInit();
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(hGameWindow);
        ImGui_ImplOpenGL3_Init();
        ImGui::StyleColorsDark();
        ImGui::GetStyle().AntiAliasedFill = false;
        ImGui::GetStyle().AntiAliasedLines = false;
        ImGui::CaptureMouseFromApp();
        ImGui::CaptureKeyboardFromApp();
        ImGui::GetStyle().WindowTitleAlign = ImVec2(0.5f, 0.5f);
    }

    if (triggers:: menuShown) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        ImGuiWindow();
        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
    hackLoop();
    wglSwapBuffersTrampoline(hDc);
}

LRESULT CALLBACK windowProc_hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

    if (uMsg == WM_KEYDOWN && wParam == VK_DELETE) {
        triggers::menuShown = !triggers::menuShown;
        return false;
    }

    if (triggers::menuShown) {
        CallWindowProc((WNDPROC)ImGui_ImplWin32_WndProcHandler, hWnd, uMsg, wParam, lParam);
    }
    return CallWindowProc(hGameWindowProc, hWnd, uMsg, wParam, lParam);
}

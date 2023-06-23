#include "graphics.h"
#include "triggers.h"
#include "offsets.h"
#include "main.h"

HWND hGameWindow = 0;
WNDPROC hGameWindowProc = 0;
extern "C" wglSwapBuffers_t wglSwapBuffersTrampoline = nullptr;
extern "C" glColor4f_t glColor4fTrampoline  = nullptr;
extern "C" glEnable_t glEnableTrampoline = nullptr;
extern "C" glDisable_t glDisableTrampoline = nullptr;
extern "C" glEnableClientState_t glEnableClientStateTrampoline = nullptr;
extern "C" glDisableClientState_t glDisableClientStateTrampoline = nullptr;
extern "C" glDrawElements_t glDrawElementsTrampoline = nullptr;
extern "C" glBegin_t glBeginTrampoline = nullptr;
extern "C" glEnd_t glEndTrampoline = nullptr;
extern "C" glVertex_t glVertexTrampoline = nullptr;
extern "C" glDepthFunc_t glDepthFuncTrampoline = nullptr;

#define OBJ_A 6
#define OBJ_B 24
#define OBJ_C 18
#define OBJ_D 60
#define OBJ_E 30
#define OBJ_F 36
#define OBJ_G 66
#define OBJ_H 1140
#define OBJ_I 228
#define OBJ_J 27
#define OBJ_K 72
#define OBJ_L 21
#define OBJ_M 39
#define OBJ_N 360
#define OBJ_O 579
#define OBJ_P 378

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

    if (triggers::menuShown) {
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

void __stdcall glDepthFuncHook(GLenum func) {
    glDepthFuncTrampoline(func);
}

void __stdcall glColor4fHook(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) {
    //printf("glColor4f: red: %f, green: %f, blue: %f, alpha: %f\n", red, green, blue, alpha);
	glColor4fTrampoline(red, green, blue, alpha);
}

void __stdcall glEnableHook(GLenum cap) {
    //printf("glEnable: cap: %d\n", cap);
	glEnableTrampoline(cap);
}

void __stdcall glDisableHook(GLenum cap) {
    //printf("glDisable: cap: %d\n", cap);
	glDisableTrampoline(cap);
}

void __stdcall glEnableClientStateHook(GLenum cap) {
    //printf("glEnableClientState: cap: %d\n", cap);
	glEnableClientStateTrampoline(cap);
}

void __stdcall glDisableClientStateHook(GLenum cap) {
    //printf("glDisableClientState: cap: %d\n", cap);
	glDisableClientStateTrampoline(cap);
}

void __stdcall glDrawElementsHook(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices) {
    //printf("glDrawElements: mode: %d, count: %d, type: %d, indices: %p\n", mode, count, type, indices);
    if (count >= triggers::renderCount)
    glDepthFuncHook(GL_ALWAYS);
	glDrawElementsTrampoline(mode, count, type, indices);
}

void __stdcall glBeginHook(GLenum mode) {
    //printf("glBegin: mode: %d\n", mode);
	glBeginTrampoline(mode);
}

void __stdcall glEndHook() {
    //printf("glEnd\n");
	glEndTrampoline();
}

void __stdcall glVertexHook(GLfloat x, GLfloat y, GLfloat z) {
    //printf("glVertex: x: %f, y: %f, z: %f\n", x, y, z);
	glVertexTrampoline(x, y, z);
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


opengl::opengl()
{
	this->hOpengl32 = nullptr;
	this->wglSwapBuffersObj = nullptr;
    this->glColor4fObj = nullptr;
    this->glEnableObj = nullptr;
    this->glDisableObj = nullptr;
    this->glEnableClientStateObj = nullptr;
    this->glDisableClientStateObj = nullptr;
    this->glBeginObj = nullptr;
    this->glEndObj = nullptr;
    this->glVertexObj = nullptr;
    this->glDrawElementsObj = nullptr;
    this->glDepthFuncObj = nullptr;

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

    LPVOID glColor4fHookAddr = GetProcAddress(this->hOpengl32, "glColor4f");
    if (!glColor4fHookAddr) {
		printf("failed getting address of glColor4f\n");
		exit(-1);
	}

    LPVOID glEnableHookAddr = GetProcAddress(this->hOpengl32, "glEnable");
    if (!glEnableHookAddr) {
        printf("failed getting address of glEnable\n");
        exit(-1);
    }

    LPVOID glDisableHookAddr = GetProcAddress(this->hOpengl32, "glDisable");
    if (!glDisableHookAddr) {
		printf("failed getting address of glDisable\n");
		exit(-1);
	}
    
    LPVOID glEnableClientStateHookAddr = GetProcAddress(this->hOpengl32, "glEnableClientState");
    if (!glEnableClientStateHookAddr) {
		printf("failed getting address of glEnableClientState\n");
		exit(-1);
	}

    LPVOID glDisableClientStateHookAddr = GetProcAddress(this->hOpengl32, "glDisableClientState");
    if (!glDisableClientStateHookAddr) {
        printf("failed getting address of glDisableClientState\n");
        exit(-1);
    }

    LPVOID glBeginHookAddr = GetProcAddress(this->hOpengl32, "glBegin");
    if (!glBeginHookAddr) {
		printf("failed getting address of glBegin\n");
		exit(-1);
	}

    LPVOID glEndHookAddr = GetProcAddress(this->hOpengl32, "glEnd");
    if (!glEndHookAddr) {
        printf("failed getting address of glEnd\n");
        exit(-1);
    }

    LPVOID glVertexHookAddr = GetProcAddress(this->hOpengl32, "glVertex3f");
    if (!glVertexHookAddr) {
        printf("failed getting address of glVertex\n");
        exit(-1);
    }

    LPVOID glDrawElementsHookAddr = GetProcAddress(this->hOpengl32, "glDrawElements");
    if (!glDrawElementsHookAddr) {
		printf("failed getting address of glDrawElements\n");
		exit(-1);
	}

    LPVOID glDepthFuncHookAddr = GetProcAddress(this->hOpengl32, "glDepthFunc");
    if (!glDepthFuncHookAddr) {
        printf("failed getting address of glDepthFunc\n");
        exit(-1);
    }

	this->wglSwapBuffersObj = Trampo::CreateHook(RenderHook, wglSwapBuffersHookAddr, 5);
	if (!this->wglSwapBuffersObj) {
		exit(-1);
	}
    this->glColor4fObj = Trampo::CreateHook(glColor4fHook, glColor4fHookAddr, 5);
    if (!this->glColor4fObj) {
	    exit(-1);
	}

	this->glEnableObj = Trampo::CreateHook(glEnableHook, glEnableHookAddr, 5);
    if (!this->glEnableObj) {
		exit(-1);
	}

	this->glDisableObj = Trampo::CreateHook(glDisableHook, glDisableHookAddr, 5);
    if (!this->glDisableObj) {
		exit(-1);
	}

	this->glEnableClientStateObj = Trampo::CreateHook(glEnableClientStateHook, glEnableClientStateHookAddr, 5);
    if (!this->glEnableClientStateObj) {
		exit(-1);
	}

	this->glDisableClientStateObj = Trampo::CreateHook(glDisableClientStateHook, glDisableClientStateHookAddr, 5);
    if (!this->glDisableClientStateObj) {
		exit(-1);
	}

    this->glBeginObj = Trampo::CreateHook(glBeginHook, glBeginHookAddr, 5);
    if (!this->glBeginObj) {
		exit(-1);
	}

    this->glEndObj = Trampo::CreateHook(glEndHook, glEndHookAddr, 5);
    if (!this->glEndObj) {
        exit(-1);
    }

    this->glVertexObj = Trampo::CreateHook(glVertexHook, glVertexHookAddr, 5);
    if (!this->glVertexObj) {
		exit(-1);
	}

    this->glDrawElementsObj = Trampo::CreateHook(glDrawElementsHook, glDrawElementsHookAddr, 5);
    if (!this->glDrawElementsObj) {
		exit(-1);
	}
    this->glDepthFuncObj = Trampo::CreateHook(glDepthFuncHook, glDepthFuncHookAddr, 5);
    if (!this->glDepthFuncObj) {
		exit(-1);
	}

	glColor4fTrampoline = (glColor4f_t)glColor4fObj->trampoline;
	glEnableTrampoline = (glEnable_t)glEnableObj->trampoline;
	glDisableTrampoline = (glDisable_t)glDisableObj->trampoline;
	glEnableClientStateTrampoline = (glEnableClientState_t)glEnableClientStateObj->trampoline;
	glDisableClientStateTrampoline = (glDisableClientState_t)glDisableClientStateObj->trampoline;
	wglSwapBuffersTrampoline = (wglSwapBuffers_t)wglSwapBuffersObj->trampoline;
    glBeginTrampoline = (glBegin_t)glBeginObj->trampoline;
	glEndTrampoline = (glEnd_t)glEndObj->trampoline;
	glVertexTrampoline = (glVertex_t)glVertexObj->trampoline;
	glDrawElementsTrampoline = (glDrawElements_t)glDrawElementsObj->trampoline;
    glDepthFuncTrampoline = (glDepthFunc_t)glDepthFuncObj->trampoline;

	return 0;
}

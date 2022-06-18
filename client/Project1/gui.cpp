// https://github.com/ocornut/imgui/blob/master/examples/example_win32_directx9/main.cpp

#include "gui.h"

// Data
static LPDIRECT3D9              g_pD3D = NULL;
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

char username[21];
char password[33];
int tab = 0;
const char* proc_status;
HMODULE hMods[1024];
DWORD cbNeeded;
bool proc_found;
std::string login_status = "";
std::string sub_status = "";
bool done = false;
int remaining;
std::string login_token = "";

// Main code
void gui::init()
{
    // Create application window
    //ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, xorstr_(L"loader"), NULL };
    ::RegisterClassEx(&wc);
    HWND hwnd = ::CreateWindow(wc.lpszClassName, xorstr_(L"loader"), (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX), 100, 100, 600, 400, NULL, NULL, wc.hInstance, NULL);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        return;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = NULL;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    while (!done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
        MSG msg;
        while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        // Start the Dear ImGui frame
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(-1.f, -1.f));
        ImGui::SetNextWindowSize(ImVec2(586.f, 363.f));
        
        ImGui::Begin(xorstr_("##gui"), NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings);

        switch (tab) {
        case 0:
            ImGui::SetCursorPosX(265.f);

            ImGui::Text(xorstr_("username"));

            ImGui::SetCursorPosX(103.f);

            ImGui::InputText(xorstr_("##username"), username, sizeof(username));

            ImGui::SetCursorPosX(265.f);

            ImGui::Text(xorstr_("password"));

            ImGui::SetCursorPosX(103.f);

            ImGui::InputText(xorstr_("##password"), password, sizeof(password), ImGuiInputTextFlags_Password);

            ImGui::SetCursorPosX((586.f / 2) - (ImGui::CalcTextSize(login_status.c_str()).x / 2));

            if (login_status.substr(0, 2) != xorstr_("OK")) {
                ImGui::Text(xorstr_("%s"), login_status.c_str());
            }

            ImGui::SetCursorPos(ImVec2(103.f, 213.f));

            if (ImGui::Button(xorstr_("login"), ImVec2(380.f, 19.f))) {
                std::string login_string = xorstr_("/api/login.php?username=");
                login_string += username;
                login_string += xorstr_("&password=");
                login_string += password;
                std::wstring ws_login_string{ login_string.begin(), login_string.end() };
                login_status = http::get_api_data(ws_login_string.c_str());

                if (login_status.substr(0, 2) == xorstr_("OK")) {
                    login_token = login_status.erase(0, 2);
                    std::string hwid_string = xorstr_("/api/hwid.php?username=");
                    hwid_string += username;
                    hwid_string += xorstr_("&hwid=");
                    hwid_string += hwid::get_hwid();
                    std::wstring ws_hwid_string{ hwid_string.begin(), hwid_string.end() };
                    std::string get_hwid = http::get_api_data(ws_hwid_string.c_str());

                    if (get_hwid == xorstr_("OK")) {
                        tab = 1;
                    }
                    else {
                        login_status = get_hwid;
                    }
                }
            }

            ImGui::SetCursorPos(ImVec2(10.f, 340.f));

            ImGui::Text(xorstr_("session expires in: %ds"), remaining);
            break;
        case 1:
            PROCESSENTRY32 entry;
            entry.dwSize = sizeof(PROCESSENTRY32);

            HANDLE snapshot = LI_FN(CreateToolhelp32Snapshot)(TH32CS_SNAPPROCESS, NULL);

            if (LI_FN(Process32First)(snapshot, &entry))
                while (Process32Next(snapshot, &entry))
                    if (!_wcsicmp(entry.szExeFile, xorstr_(L"csgo.exe"))) {
                        proc_status = xorstr_("csgo found!");
                        proc_found = true;
                        break;
                    }
                    else {
                        proc_status = xorstr_("waiting for csgo...");
                        proc_found = false;
                    }
            
            LI_FN(CloseHandle)(snapshot);

            if (proc_found) {
                HANDLE proc = LI_FN(OpenProcess)(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);

                if (proc) {
                    if (LI_FN(EnumProcessModules)(proc, hMods, sizeof(hMods), &cbNeeded)) {
                        for (int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
                            TCHAR szModName[MAX_PATH];

                            if (GetModuleFileNameEx(proc, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR))) {
                                if (_tcsstr(szModName, xorstr_(L"serverbrowser.dll")) != nullptr) {
                                    proc_status = xorstr_("serverbrowser.dll found! ready to inject.");
                                    break;
                                }
                                else {
                                    proc_status = xorstr_("waiting for serverbrowser.dll...");
                                }
                            }
                        }
                    }
                }

                LI_FN(CloseHandle)(proc);
            }

            ImGui::SetCursorPosX(((586.f / 2) - (ImGui::CalcTextSize(proc_status).x / 2)) - 26);

            ImGui::Text(xorstr_("status: %s"), proc_status);

            ImGui::SetCursorPosY(172.f);

            if (ImGui::Button(xorstr_("inject"), ImVec2(570.f, 19.f))) {
                std::string date_string = xorstr_("/api/expires.php?username=");
                date_string += username;
                std::wstring ws_date_string{ date_string.begin(), date_string.end() };
                std::string expires = http::get_api_data(ws_date_string.c_str());
                sub_status = expires;

                if (expires == xorstr_("OK")) {
                    if (strcmp(proc_status, xorstr_("serverbrowser.dll found! ready to inject.")) == 0) {
                        if (loader::inject()) {
                            done = true;
                            LI_FN(ExitProcess)(0);
                        }
                    }
                }
            }

            ImGui::SetCursorPosX(((586.f / 2) - (ImGui::CalcTextSize(sub_status.c_str()).x / 2)));

            ImGui::Text(xorstr_("%s"), sub_status.c_str());

            ImGui::SetCursorPos(ImVec2(10.f, 340.f));

            ImGui::Text(xorstr_("session expires in: %ds"), remaining);

            break;
        }

        ImGui::End();

        // Rendering
        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }
        HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

        // Handle loss of D3D9 device
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            ResetDevice();
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClass(wc.lpszClassName, wc.hInstance);
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
        return false;

    // Create the D3DDevice
    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
    //g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
        return false;

    return true;
}

void CleanupDeviceD3D()
{
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
}

void ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            g_d3dpp.BackBufferWidth = LOWORD(lParam);
            g_d3dpp.BackBufferHeight = HIWORD(lParam);
            ResetDevice();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
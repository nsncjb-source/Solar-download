#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <d3dcompiler.h>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <cstdio>
#include "offsets.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include "inc/rbx.hpp"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "psapi.lib")

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

ID3D11Device* g_pd3dDevice = nullptr;
ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
IDXGISwapChain* g_pSwapChain = nullptr;
ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;
HWND g_hwnd = nullptr;
bool g_running = true;
bool g_menuVisible = true;
bool flyToggled = false;

const char* KeyNames[] = { "None", "LMB", "RMB", "Middle", "X1", "X2", "Shift", "Ctrl", "Alt", "Caps", "Tab", "Space", "Enter", "Backspace", "Delete", "Insert", "Home", "End", "PgUp", "PgDn", "Up", "Down", "Left", "Right", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12" };

struct Settings_t {
    bool aimbotEnabled = true;
    bool espEnabled = true;
    bool flyEnabled = false;
    bool noclipEnabled = false;
    bool showFPS = true;
    bool showFOV = true;
    int aimbotKey = 2;
    int flyKey = 0x46;
    float aimbotFOV = 150.0f;
    float aimbotSmoothing = 8.0f;
    float aimbotDistance = 0.0f;
    int aimbotPart = 0; // 0=Head, 1=UpperTorso, 2=HumanoidRootPart
    float espColor[4] = { 1.0f, 0.5f, 0.0f, 1.0f };
    float aimbotColor[4] = { 1.0f, 0.0f, 0.0f, 0.5f };
} Settings;

const char* aimbotParts[] = { "Head", "UpperTorso", "HumanoidRootPart" };

namespace ImGui {
    void Hotkey(int* k, const ImVec2& size = ImVec2(0, 0)) {
        ImGui::PushID(k);
        static int waiting = -1;
        if (waiting != *k) {
            if (ImGui::Button(KeyNames[*k], size)) waiting = *k;
        }
        else {
            ImGui::Button("...", size);
            for (int i = 1; i < 255; i++) {
                if (GetAsyncKeyState(i) & 1) {
                    *k = i;
                    waiting = -1;
                    break;
                }
            }
        }
        ImGui::PopID();
    }
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) return true;
    if (msg == WM_DESTROY) { g_running = false; PostQuitMessage(0); return 0; }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

bool CreateDeviceD3D(HWND hWnd) {
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 2;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0,
        D3D11_SDK_VERSION, &sd, &g_pSwapChain,
        &g_pd3dDevice, nullptr, &g_pd3dDeviceContext);
    if (FAILED(hr)) return false;

    ID3D11Texture2D* pBackBuffer = nullptr;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    if (pBackBuffer) {
        g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
        pBackBuffer->Release();
    }
    return true;
}

void CleanupDeviceD3D() {
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEXW wc = { sizeof(WNDCLASSEXW), CS_CLASSDC, WndProc, 0, 0, hInstance, nullptr, nullptr, nullptr, nullptr, L"SolarOverlay", nullptr };
    RegisterClassExW(&wc);
    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);
    g_hwnd = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW,
        L"SolarOverlay", L"SOLAR", WS_POPUP, 0, 0, screenW, screenH,
        nullptr, nullptr, wc.hInstance, nullptr);
    SetLayeredWindowAttributes(g_hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);
    ShowWindow(g_hwnd, SW_SHOW);
    if (!CreateDeviceD3D(g_hwnd)) { MessageBoxW(nullptr, L"DirectX failed!", L"Error", MB_OK); return 1; }

    ImGui::CreateContext();
    ImGui_ImplWin32_Init(g_hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 8.0f;
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.04f, 0.06f, 0.14f, 0.95f);
    style.Colors[ImGuiCol_Button] = ImVec4(1.0f, 0.16f, 0.48f, 0.8f);

    std::cout << "SOLAR - Rivals Edition" << std::endl;
    std::cout << "Press ENTER to attach..." << std::endl;
    std::cin.get();

    if (!RBX::Memory::attach()) {
        MessageBoxW(nullptr, L"Failed to attach!", L"Error", MB_OK);
        return 1;
    }

    std::vector<RBX::Instance> playersList;
    ULONGLONG lastRefresh = GetTickCount64();
    ULONGLONG lastFrame = GetTickCount64();
    int fps = 0, fpsCounter = 0;
    MSG msg = {};

    while (g_running) {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (GetAsyncKeyState(VK_INSERT) & 1) g_menuVisible = !g_menuVisible;
        if (GetAsyncKeyState(VK_ESCAPE) & 1) g_running = false;

        fpsCounter++;
        if (GetTickCount64() - lastFrame >= 1000) {
            fps = fpsCounter;
            fpsCounter = 0;
            lastFrame = GetTickCount64();
        }

        RBX::Instance dataModel = RBX::getDataModel();
        if (!dataModel.address) { Sleep(10); continue; }

        RBX::Instance workspace = dataModel.findFirstChild("Workspace");
        if (!workspace.address) { Sleep(10); continue; }

        RBX::Instance players = dataModel.findFirstChild("Players");
        if (!players.address) { Sleep(10); continue; }

        void* localPlayerPtr = (void*)RBX::Memory::read<uintptr_t>((void*)((uintptr_t)players.address + Offsets::LocalPlayer));
        RBX::Instance localPlayer(localPlayerPtr);

        RBX::Instance localCharacter = localPlayer.getModelInstance();
        RBX::Instance hrp = localCharacter.findFirstChild("HumanoidRootPart");

        void* cameraPtr = (void*)RBX::Memory::read<uintptr_t>((void*)((uintptr_t)workspace.address + Offsets::Camera));
        RBX::Instance camera(cameraPtr);

        void* visualEnginePtr = (void*)RBX::Memory::read<uintptr_t>((void*)((uintptr_t)RBX::Memory::getRobloxBaseAddr() + Offsets::VisualEnginePointer));
        RBX::VisualEngine visualEngine(visualEnginePtr);

        if (GetTickCount64() - lastRefresh > 300) {
            playersList.clear();
            RBX::Instance currentPlayers = RBX::getDataModel().findFirstChild("Players");
            if (currentPlayers.address) {
                std::vector<RBX::Instance> children = currentPlayers.getChildren();
                for (size_t i = 0; i < children.size(); i++) {
                    RBX::Instance plr = children[i];
                    if (!plr.address) continue;
                    void* characterPtr = (void*)RBX::Memory::read<uintptr_t>((void*)((uintptr_t)plr.address + Offsets::ModelInstance));
                    RBX::Instance character(characterPtr);
                    if (character.address && character.name() != localPlayer.name()) {
                        playersList.push_back(character);
                    }
                }
            }
            lastRefresh = GetTickCount64();
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        ImDrawList* drawList = ImGui::GetBackgroundDrawList();

        drawList->AddText(ImVec2(10, 10), IM_COL32(255, 42, 122, 255), "SOLAR");
        drawList->AddText(ImVec2(10, 30), IM_COL32(176, 38, 255, 255), "RIVALS");

        if (Settings.showFPS) {
            char txt[32];
            sprintf_s(txt, "FPS: %d", fps);
            drawList->AddText(ImVec2((float)(screenW - 80), 10.0f), IM_COL32(255, 255, 255, 255), txt);
        }
        drawList->AddText(ImVec2(10, 55), IM_COL32(255, 200, 100, 255), "Press INSERT | ESC to exit");

        if (g_menuVisible) {
            ImGui::SetNextWindowSize(ImVec2(380, 500), ImGuiCond_FirstUseEver);
            ImGui::Begin("SOLAR - Rivals", &g_menuVisible, ImGuiWindowFlags_NoCollapse);

            ImGui::TextColored(ImVec4(1.0f, 0.16f, 0.48f, 1.0f), "SOLAR EXTERNAL");
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.69f, 0.15f, 1.0f, 1.0f), "| RIVALS MODE");
            ImGui::Separator();

            // Aimbot Section
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "AIMBOT");
            ImGui::Checkbox("Enable Aimbot", &Settings.aimbotEnabled);
            ImGui::SameLine();
            ImGui::Hotkey(&Settings.aimbotKey, ImVec2(80, 25));
            ImGui::Checkbox("Show FOV", &Settings.showFOV);

            ImGui::SliderFloat("FOV Radius", &Settings.aimbotFOV, 50.0f, 400.0f, "%.0f");
            ImGui::SliderFloat("Smoothing", &Settings.aimbotSmoothing, 1.0f, 20.0f, "%.1f");
            ImGui::SliderFloat("Max Distance (0=off)", &Settings.aimbotDistance, 0.0f, 500.0f, "%.0f");

            ImGui::Text("Aim Part");
            ImGui::SameLine();
            if (ImGui::BeginCombo("##AimPart", aimbotParts[Settings.aimbotPart])) {
                for (int i = 0; i < 3; i++) {
                    if (ImGui::Selectable(aimbotParts[i], Settings.aimbotPart == i)) {
                        Settings.aimbotPart = i;
                    }
                }
                ImGui::EndCombo();
            }

            ImGui::ColorEdit4("FOV Color", Settings.aimbotColor);
            ImGui::Separator();

            // ESP Section
            ImGui::TextColored(ImVec4(0.69f, 0.15f, 1.0f, 1.0f), "VISUALS");
            ImGui::Checkbox("Enable ESP", &Settings.espEnabled);
            ImGui::ColorEdit4("ESP Color", Settings.espColor);
            ImGui::Separator();

            // Movement Section
            ImGui::TextColored(ImVec4(0.0f, 0.47f, 0.93f, 1.0f), "MOVEMENT");
            ImGui::Checkbox("Fly Hack", &Settings.flyEnabled);
            ImGui::SameLine();
            ImGui::Hotkey(&Settings.flyKey, ImVec2(80, 25));
            ImGui::Checkbox("Noclip", &Settings.noclipEnabled);
            ImGui::Separator();

            // Info
            ImGui::TextColored(ImVec4(1.0f, 0.16f, 0.48f, 1.0f), "INFO");
            ImGui::Text("Players: %d", (int)playersList.size());
            ImGui::Text("Aimbot: %s", Settings.aimbotEnabled ? "ON" : "OFF");
            ImGui::Text("ESP: %s", Settings.espEnabled ? "ON" : "OFF");
            ImGui::Text("Game: RIVALS");

            ImGui::End();
        }

        // ==================== IMPROVED AIMBOT ====================
        if (Settings.aimbotEnabled && visualEngine.address && (GetAsyncKeyState(Settings.aimbotKey) & 0x8000)) {

            float closestDistance = Settings.aimbotFOV;
            RBX::Instance bestTarget(nullptr);
            RBX::Vector3 bestTargetPos = { 0, 0, 0 };
            RBX::Vector2 bestTargetScreen = { 0, 0 };

            const char* hitboxOrder[] = { "Head", "UpperTorso", "HumanoidRootPart", "Torso" };

            for (size_t i = 0; i < playersList.size(); i++) {
                RBX::Instance plr = playersList[i];
                if (!plr.address) continue;

                // Distance check
                if (Settings.aimbotDistance > 0 && hrp.address) {
                    float distance = hrp.getDistance(plr.getPosition());
                    if (distance > Settings.aimbotDistance) continue;
                }

                // Get target part based on selection
                RBX::Instance targetPart(nullptr);
                if (Settings.aimbotPart == 0) targetPart = plr.findFirstChild("Head");
                else if (Settings.aimbotPart == 1) targetPart = plr.findFirstChild("UpperTorso");
                else targetPart = plr.findFirstChild("HumanoidRootPart");

                // Fallback if selected part not found
                if (!targetPart.address) {
                    for (int h = 0; h < 4; h++) {
                        targetPart = plr.findFirstChild(hitboxOrder[h]);
                        if (targetPart.address) break;
                    }
                }
                if (!targetPart.address) continue;

                RBX::Vector3 worldPos = targetPart.getPosition();
                RBX::Vector2 screenPos = visualEngine.worldToScreen(worldPos);

                if (screenPos.x <= 0 || screenPos.x >= screenW || screenPos.y <= 0 || screenPos.y >= screenH) continue;

                POINT mouse;
                GetCursorPos(&mouse);

                float dx = screenPos.x - (float)mouse.x;
                float dy = screenPos.y - (float)mouse.y;
                float dist = sqrtf(dx * dx + dy * dy);

                if (dist > Settings.aimbotFOV) continue;

                if (dist < closestDistance) {
                    closestDistance = dist;
                    bestTarget = plr;
                    bestTargetPos = worldPos;
                    bestTargetScreen = screenPos;
                }
            }

            if (bestTarget.address && bestTargetScreen.x > 0 && bestTargetScreen.y > 0) {
                POINT mouse;
                GetCursorPos(&mouse);

                float dx = bestTargetScreen.x - (float)mouse.x;
                float dy = bestTargetScreen.y - (float)mouse.y;

                if (Settings.aimbotSmoothing > 1.0f) {
                    dx /= Settings.aimbotSmoothing;
                    dy /= Settings.aimbotSmoothing;
                }

                if (fabsf(dx) > 0.3f || fabsf(dy) > 0.3f) {
                    INPUT input = { INPUT_MOUSE };
                    input.mi.dwFlags = MOUSEEVENTF_MOVE;
                    input.mi.dx = (LONG)dx;
                    input.mi.dy = (LONG)dy;
                    SendInput(1, &input, sizeof(INPUT));
                }

                // Target indicator
                drawList->AddCircle(ImVec2(bestTargetScreen.x, bestTargetScreen.y), 6.0f, IM_COL32(255, 0, 0, 255), 16);
                drawList->AddLine(ImVec2(bestTargetScreen.x - 12, bestTargetScreen.y), ImVec2(bestTargetScreen.x + 12, bestTargetScreen.y), IM_COL32(255, 0, 0, 255));
                drawList->AddLine(ImVec2(bestTargetScreen.x, bestTargetScreen.y - 12), ImVec2(bestTargetScreen.x, bestTargetScreen.y + 12), IM_COL32(255, 0, 0, 255));
            }
        }

        // ==================== FOV CIRCLE ====================
        if (Settings.showFOV) {
            POINT mouse;
            GetCursorPos(&mouse);
            drawList->AddCircle(ImVec2((float)mouse.x, (float)mouse.y), Settings.aimbotFOV,
                ImColor(Settings.aimbotColor[0], Settings.aimbotColor[1], Settings.aimbotColor[2], Settings.aimbotColor[3]), 64);
        }

        // ==================== ESP ====================
        if (Settings.espEnabled && visualEngine.address) {
            for (size_t i = 0; i < playersList.size(); i++) {
                RBX::Instance plr = playersList[i];
                if (!plr.address) continue;

                RBX::Instance head = plr.findFirstChild("Head");
                RBX::Instance root = plr.findFirstChild("HumanoidRootPart");
                if (!head.address || !root.address) continue;

                RBX::Vector3 headPos = head.getPosition();
                RBX::Vector3 rootPos = root.getPosition();
                RBX::Vector2 headScreen = visualEngine.worldToScreen(headPos);
                RBX::Vector2 rootScreen = visualEngine.worldToScreen(rootPos);

                if (headScreen.x <= 0 || headScreen.x >= screenW || headScreen.y <= 0) continue;
                if (rootScreen.x <= 0 || rootScreen.x >= screenW || rootScreen.y <= 0) continue;

                float height = fabsf(rootScreen.y - headScreen.y);
                float width = height * 0.6f;

                ImVec2 boxMin = ImVec2(headScreen.x - width / 2, headScreen.y);
                ImVec2 boxMax = ImVec2(headScreen.x + width / 2, headScreen.y + height);

                drawList->AddRect(boxMin, boxMax, ImColor(Settings.espColor[0], Settings.espColor[1], Settings.espColor[2], Settings.espColor[3]), 3.0f);
                drawList->AddText(ImVec2(headScreen.x - 30, headScreen.y - 15), IM_COL32(255, 255, 255, 255), plr.name().c_str());
            }
        }

        // Fly hack
        if (Settings.flyEnabled && hrp.address && (GetAsyncKeyState(Settings.flyKey) & 1)) {
            flyToggled = !flyToggled;
            if (flyToggled) {
                RBX::Vector3 vel = { 0, 0, 0 };
                RBX::Memory::write<RBX::Vector3>((void*)((uintptr_t)hrp.getPrimitive() + Offsets::Velocity), vel);
            }
        }

        if (flyToggled && hrp.address) {
            RBX::Vector3 pos = RBX::Memory::read<RBX::Vector3>((void*)((uintptr_t)hrp.getPrimitive() + Offsets::Position));
            float speed = 15.0f;
            if (GetAsyncKeyState('W') & 0x8000) pos.z -= speed;
            if (GetAsyncKeyState('S') & 0x8000) pos.z += speed;
            if (GetAsyncKeyState('A') & 0x8000) pos.x -= speed;
            if (GetAsyncKeyState('D') & 0x8000) pos.x += speed;
            if (GetAsyncKeyState(VK_SPACE) & 0x8000) pos.y += speed;
            if (GetAsyncKeyState(VK_LCONTROL) & 0x8000) pos.y -= speed;
            RBX::Memory::write<RBX::Vector3>((void*)((uintptr_t)hrp.getPrimitive() + Offsets::Position), pos);
        }

        // Noclip
        if (Settings.noclipEnabled && localCharacter.address) {
            RBX::Instance head = localCharacter.findFirstChild("Head");
            RBX::Instance torso = localCharacter.findFirstChild("UpperTorso");
            if (head.address && torso.address) {
                RBX::Memory::write<int>((void*)((uintptr_t)head.getPrimitive() + Offsets::CanCollide), 0);
                RBX::Memory::write<int>((void*)((uintptr_t)torso.getPrimitive() + Offsets::CanCollide), 0);
                if (hrp.address) {
                    RBX::Memory::write<int>((void*)((uintptr_t)hrp.getPrimitive() + Offsets::CanCollide), 0);
                }
            }
        }

        ImGui::Render();
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        const float clear[] = { 0, 0, 0, 0 };
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        g_pSwapChain->Present(1, 0);
        Sleep(1);
    }

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    CleanupDeviceD3D();
    DestroyWindow(g_hwnd);
    return 0;
}
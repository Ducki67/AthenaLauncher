#include <iostream>
#include <string>
#include <d3d11.h>
#include <Windows.h>
#include <wincodec.h>
#include <vector>

#include "resource.h"

#include "includes/imgui/imgui.h"
#include "includes/imgui/imgui_impl_dx11.h"
#include "includes/imgui/imgui_impl_win32.h"
#include "Gui.h"


#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "windowscodecs.lib")


#define TARGET_PNG_ID IDR_RCDATA1

//#define TARGET_PNG_ID 101

static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;
ID3D11ShaderResourceView* g_ButtonIconTexture = nullptr;



extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) return true;
    if (msg == WM_DESTROY) { PostQuitMessage(0); return 0; }
    return DefWindowProcA(hWnd, msg, wParam, lParam);
}



void LoadLauncherIconTexture(ID3D11Device* device)
{
    if (!device) return;

    CoInitialize(NULL);
    IWICImagingFactory* pFactory = nullptr;

    if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFactory))))
    {
        CoUninitialize();
        return;
    }

    HRSRC hResource = FindResourceA(GetModuleHandle(NULL), "IDB_PNG1", "PNG");
    if (!hResource)
    {
        hResource = FindResourceA(GetModuleHandle(NULL), MAKEINTRESOURCEA(IDB_PNG1), "PNG");
        if (!hResource)
        {
            MessageBoxA(NULL, "Meg mindig nem talalhato a Resource!", "Hiba", MB_ICONERROR);
            pFactory->Release();
            CoUninitialize();
            return;
        }
    }




    DWORD imageSize = SizeofResource(GetModuleHandle(NULL), hResource);
    HGLOBAL hResData = LoadResource(GetModuleHandle(NULL), hResource);
    if (!hResData)
    {
        pFactory->Release();
        CoUninitialize();
        return;
    }

    void* pImageData = LockResource(hResData);

    IWICStream* pStream = nullptr;
    if (SUCCEEDED(pFactory->CreateStream(&pStream)))
    {
        if (SUCCEEDED(pStream->InitializeFromMemory(reinterpret_cast<BYTE*>(pImageData), imageSize)))
        {
            IWICBitmapDecoder* pDecoder = nullptr;

            if (SUCCEEDED(pFactory->CreateDecoderFromStream(pStream, NULL, WICDecodeMetadataCacheOnDemand, &pDecoder)))
            {
                IWICBitmapFrameDecode* pFrame = nullptr;
                if (SUCCEEDED(pDecoder->GetFrame(0, &pFrame)))
                {
                    IWICFormatConverter* pConverter = nullptr;
                    if (SUCCEEDED(pFactory->CreateFormatConverter(&pConverter)))
                    {
                        if (SUCCEEDED(pConverter->Initialize(pFrame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeCustom)))
                        {
                            UINT width = 0, height = 0;
                            pConverter->GetSize(&width, &height);

                            std::vector<BYTE> buffer(width * height * 4);
                            if (SUCCEEDED(pConverter->CopyPixels(NULL, width * 4, static_cast<UINT>(buffer.size()), buffer.data())))
                            {
                                D3D11_TEXTURE2D_DESC desc = {};
                                desc.Width = width;
                                desc.Height = height;
                                desc.MipLevels = 1;
                                desc.ArraySize = 1;
                                desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
                                desc.SampleDesc.Count = 1;
                                desc.Usage = D3D11_USAGE_DEFAULT;
                                desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
                                desc.CPUAccessFlags = 0;
                                desc.MiscFlags = 0;

                                D3D11_SUBRESOURCE_DATA initData = {};
                                initData.pSysMem = buffer.data();
                                initData.SysMemPitch = width * 4;
                                initData.SysMemSlicePitch = 0;

                                ID3D11Texture2D* pTexture = nullptr;
                                if (SUCCEEDED(device->CreateTexture2D(&desc, &initData, &pTexture)))
                                {
                                    device->CreateShaderResourceView(pTexture, nullptr, &g_ButtonIconTexture);
                                    pTexture->Release();
                                }
                            }
                        }
                        pConverter->Release();
                    }
                    pFrame->Release();
                }
                pDecoder->Release();
            }
        }
        pStream->Release();
    }
    pFactory->Release();
    CoUninitialize();
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    FortAthena::Configure::LoadConfig(); //loads
    FortAthena::Configure::DownloadRedirect();

    WNDCLASSEXA wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, "FortAthenaClass", nullptr };
    wc.hIcon = LoadIcon(wc.hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wc.hIconSm = LoadIcon(wc.hInstance, MAKEINTRESOURCE(IDI_ICON1));


    RegisterClassExA(&wc);
    HWND hwnd = CreateWindowExA(0, wc.lpszClassName, "FortAthenaLauncher", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, 100, 100, 516, 439, nullptr, nullptr, wc.hInstance, nullptr);


    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 2;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hwnd;
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    D3D_FEATURE_LEVEL featureLevel;
    D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);

    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
    LoadLauncherIconTexture(g_pd3dDevice);


    bool bRunning = true;
    MSG msg;
    while (bRunning) {
        while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT) bRunning = false;
        }
        if (!bRunning) break;

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        FortAthena::Gui::RenderLauncherUI(); // render

        ImGui::Render();
        const float clear_color[4] = { 0.10f, 0.10f, 0.10f, 1.00f };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        g_pSwapChain->Present(1, 0);
    }

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    if (g_mainRenderTargetView) g_mainRenderTargetView->Release();
    if (g_pSwapChain) g_pSwapChain->Release();
    if (g_pd3dDeviceContext) g_pd3dDeviceContext->Release();
    if (g_pd3dDevice) g_pd3dDevice->Release();
    UnregisterClassA(wc.lpszClassName, wc.hInstance);

    return 0;
}

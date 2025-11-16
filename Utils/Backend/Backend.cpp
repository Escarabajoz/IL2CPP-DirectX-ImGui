#include "../Backend/Backend.h"
#include "../../Cheat/Functions/Functions.h"
#include <map>

Backend::presentVariable originalPresent;
Backend::presentVariable hookedPresent;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static bool init = false;
static std::map<std::string, float> checkboxAnimations;

Backend RunBackend;

// Animated Checkbox Function
bool AnimatedCheckbox(const char* label, bool* v)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

    const float square_sz = ImGui::GetFrameHeight();
    const ImVec2 pos = window->DC.CursorPos;
    const ImRect total_bb(pos, ImVec2(pos.x + square_sz + (label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f), pos.y + label_size.y + style.FramePadding.y * 2.0f));
    ImGui::ItemSize(total_bb, style.FramePadding.y);
    if (!ImGui::ItemAdd(total_bb, id))
        return false;

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(total_bb, id, &hovered, &held);
    if (pressed)
    {
        *v = !(*v);
        ImGui::MarkItemEdited(id);
    }

    // Animation
    std::string key = std::string(label);
    if (checkboxAnimations.find(key) == checkboxAnimations.end())
        checkboxAnimations[key] = *v ? 1.0f : 0.0f;

    float& anim = checkboxAnimations[key];
    float target = *v ? 1.0f : 0.0f;
    anim += (target - anim) * 0.15f;

    const ImRect check_bb(pos, ImVec2(pos.x + square_sz, pos.y + square_sz));
    
    // Background with animation
    ImU32 bg_col = ImGui::GetColorU32(hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
    ImU32 check_col = ImGui::GetColorU32(ImGuiCol_CheckMark);
    
    // Interpolate background color
    ImVec4 bg_color = ImGui::ColorConvertU32ToFloat4(bg_col);
    ImVec4 active_color = ImGui::ColorConvertU32ToFloat4(ImGui::GetColorU32(ImGuiCol_ButtonActive));
    ImVec4 final_bg = ImVec4(
        bg_color.x + (active_color.x - bg_color.x) * anim,
        bg_color.y + (active_color.y - bg_color.y) * anim,
        bg_color.z + (active_color.z - bg_color.z) * anim,
        bg_color.w
    );

    window->DrawList->AddRectFilled(check_bb.Min, check_bb.Max, ImGui::ColorConvertFloat4ToU32(final_bg), style.FrameRounding);
    window->DrawList->AddRect(check_bb.Min, check_bb.Max, ImGui::GetColorU32(ImGuiCol_Border), style.FrameRounding, 0, 1.5f);

    // Animated checkmark
    if (anim > 0.01f)
    {
        const float pad = ImMax(1.0f, IM_FLOOR(square_sz / 6.0f));
        const float check_sz = square_sz - pad * 2.0f;
        const float check_anim = anim;
        
        ImVec2 center = ImVec2(check_bb.Min.x + pad + check_sz * 0.5f, check_bb.Min.y + pad + check_sz * 0.5f);
        float r = check_sz * 0.5f * check_anim;
        
        // Draw animated checkmark
        window->DrawList->AddCircleFilled(center, r * 0.7f, check_col, 12);
        
        if (check_anim > 0.5f)
        {
            float check_progress = (check_anim - 0.5f) * 2.0f;
            ImVec2 p1 = ImVec2(center.x - r * 0.3f, center.y);
            ImVec2 p2 = ImVec2(center.x - r * 0.1f, center.y + r * 0.3f);
            ImVec2 p3 = ImVec2(center.x + r * 0.4f, center.y - r * 0.4f);
            
            window->DrawList->AddLine(p1, ImVec2(p1.x + (p2.x - p1.x) * check_progress, p1.y + (p2.y - p1.y) * check_progress), 
                ImGui::GetColorU32(ImGuiCol_Text), 2.0f);
            if (check_progress > 0.5f)
            {
                float line2_progress = (check_progress - 0.5f) * 2.0f;
                window->DrawList->AddLine(p2, ImVec2(p2.x + (p3.x - p2.x) * line2_progress, p2.y + (p3.y - p2.y) * line2_progress), 
                    ImGui::GetColorU32(ImGuiCol_Text), 2.0f);
            }
        }
    }

    if (label_size.x > 0.0f)
        ImGui::RenderText(ImVec2(check_bb.Max.x + style.ItemInnerSpacing.x, check_bb.Min.y + style.FramePadding.y), label);

    return pressed;
}

bool Backend::DirectXPresentHook()
{
    ZeroMemory(&m_gSwapChainDescription, sizeof(m_gSwapChainDescription));

    m_gSwapChainDescription.BufferCount = 2;
    m_gSwapChainDescription.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    m_gSwapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    m_gSwapChainDescription.OutputWindow = GetForegroundWindow();
    m_gSwapChainDescription.SampleDesc.Count = 1;
    m_gSwapChainDescription.Windowed = TRUE;
    m_gSwapChainDescription.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    HRESULT createDevice = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, m_gFeatureLevels, 2, D3D11_SDK_VERSION, &m_gSwapChainDescription, &m_gSwapChain, &m_gDevice, nullptr, nullptr);
        
    if (FAILED(createDevice)) 
        return false; // dont return false make an endless cycle (only if u wanna go cpu boom) 

    void** DX11Vtable = *reinterpret_cast<void***>(m_gSwapChain);

    UnloadDevices(false); // don't need to reset mainrendertargetview
    hookedPresent = (Backend::presentVariable)DX11Vtable[8]; // 8. virtual table is present

    return true;
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (RunBackend.m_bOpenMenu && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) // if menu open then handle imgui events
        return true;

    return CallWindowProc(RunBackend.m_goriginalWndProc, hWnd, uMsg, wParam, lParam);
}

void Backend::LoadImGui(HWND window, ID3D11Device* device, ID3D11DeviceContext* context)
{
    ImGui::CreateContext(); // creating the context cus we need imgui
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange; // dont change cursors
    ImGui_ImplWin32_Init(window); // which window u wanna draw your imgui huh???
    ImGui_ImplDX11_Init(device, context); // u need the device's context since u can't draw with only device, thanx dx11
} // loading the imgui

void Backend::DrawImGui(ID3D11DeviceContext* context, ID3D11RenderTargetView* targetview)
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (m_bOpenMenu)
    {
        // Modern styling
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 8.0f;
        style.FrameRounding = 6.0f;
        style.GrabRounding = 6.0f;
        style.WindowPadding = ImVec2(15, 15);
        style.FramePadding = ImVec2(8, 4);
        style.ItemSpacing = ImVec2(8, 8);
        
        ImGui::SetNextWindowSize(ImVec2(350, 0), ImGuiCond_FirstUseEver);
        ImGui::Begin("DirectX11 Hook", &m_bOpenMenu, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.9f, 0.9f, 1.0f));
            
            AnimatedCheckbox("Camera FOV", &Variables::EnableCamera);
            if(Variables::EnableCamera)
            {
                ImGui::Indent(20.0f);
                ImGui::SliderFloat("##CameraFOV", &Variables::CameraFov, 20, 180, "FOV: %.0f");
                ImGui::Unindent(20.0f);
            }
            
            ImGui::Spacing();
            AnimatedCheckbox("Circle FOV", &Variables::EnableCircleFov);
            if (Variables::EnableCircleFov)
            {
                ImGui::Indent(20.0f);
                ImGui::SliderFloat("##CircleFOV", &Variables::CircleFov, 20, 180, "Circle: %.0f");
                AnimatedCheckbox("Rainbow", &Variables::EnableRainbow);
                ImGui::Unindent(20.0f);
            }
            
            ImGui::Spacing();
            AnimatedCheckbox("Watermark", &Variables::EnableWatermark);
            if (Variables::EnableWatermark)
            {
                ImGui::Indent(20.0f);
                AnimatedCheckbox("Show FPS", &Variables::EnableFPS);
                ImGui::Unindent(20.0f);
            }
            
            ImGui::PopStyleColor();
        }
        ImGui::End();
    }

    RunBackend.RenderCheat();
    ImGui::EndFrame();
    ImGui::Render();
    context->OMSetRenderTargets(1, &targetview, NULL);  // 1 render target, render it to our monitor, no dsv
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); // drawing the imgui menu
}

void Backend::UnloadImGui()
{
	MH_DisableHook(hookedPresent); 
	MH_RemoveHook(hookedPresent);
	MH_Uninitialize();

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Backend::UnloadDevices(bool renderTargetViewReset)
{
	if(renderTargetViewReset) if (m_gMainRenderTargetView) { m_gMainRenderTargetView->Release(); m_gMainRenderTargetView = nullptr; }
	if (m_gPointerContext) { m_gPointerContext->Release(); m_gPointerContext = nullptr; }
	if (m_gDevice) { m_gDevice->Release(); m_gDevice = nullptr; }
	SetWindowLongPtr(m_gWindow, GWLP_WNDPROC, (LONG_PTR)(m_goriginalWndProc));
}

void Backend::RenderCheat()
{
	try
	{
		if (Variables::EnableCamera)
		{
			Unity::CCamera* CameraMain = Unity::Camera::GetMain();

			if (CameraMain != nullptr)
			{
				try
				{
					CameraMain->SetFieldOfView(Variables::CameraFov);
				}
				catch (...)
				{
					// Silently fail if camera FOV change crashes
				}
			}
		}

		if (Variables::EnableCircleFov && Variables::EnableRainbow)
			Utils::UseFov(true);
		else if (Variables::EnableCircleFov && !Variables::EnableRainbow)
			Utils::UseFov(false);

		if (Variables::EnableWatermark && Variables::EnableFPS)
			Utils::Watermark("@zzzmate", true);
		else if(Variables::EnableWatermark && !Variables::EnableFPS)
			Utils::Watermark("@zzzmate", false);
	}
	catch (...)
	{
		// Prevent entire render from crashing
	}
}	

static long __stdcall PresentHook(IDXGISwapChain* pointerSwapChain, UINT sync, UINT flags)
{
	if (!init) {
		if (SUCCEEDED(pointerSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&RunBackend.m_gDevice))) // check if device working 
		{
			RunBackend.m_gDevice->GetImmediateContext(&RunBackend.m_gPointerContext); // need context immediately!!
			pointerSwapChain->GetDesc(&RunBackend.m_gPresentHookSwapChain); // welp we need the presenthook's outputwindow so it's actually ours o_o
			RunBackend.m_gWindow = RunBackend.m_gPresentHookSwapChain.OutputWindow;

			pointerSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&RunBackend.m_gPointerBackBuffer); // getting back buffer
			if (RunBackend.m_gPointerBackBuffer != nullptr) RunBackend.m_gDevice->CreateRenderTargetView(RunBackend.m_gPointerBackBuffer, NULL, &RunBackend.m_gMainRenderTargetView); // from backbuffer to our monitor
			RunBackend.m_gPointerBackBuffer->Release(); // don't need this shit anymore, but please comeback the next injection

			RunBackend.LoadImGui(RunBackend.m_gWindow, RunBackend.m_gDevice, RunBackend.m_gPointerContext); // load imgui!!!
			RunBackend.m_goriginalWndProc = (WNDPROC)SetWindowLongPtr(RunBackend.m_gWindow, GWLP_WNDPROC, (LONG_PTR)WndProc); // i think u need this

			RunBackend.m_gPointerContext->RSGetViewports(&RunBackend.m_gVps, &RunBackend.m_gViewport);
			Variables::ScreenSize = { RunBackend.m_gViewport.Width, RunBackend.m_gViewport.Height };
			Variables::ScreenCenter = { RunBackend.m_gViewport.Width / 2.0f, RunBackend.m_gViewport.Height / 2.0f };

			ImGui::GetIO().Fonts->AddFontDefault();
			RunBackend.g_mFontConfig.GlyphExtraSpacing.x = 1.2;
			BaseFonts::GameFont = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(BaseFonts::TTSquaresCondensedBold, 14, 14, &RunBackend.g_mFontConfig);
			ImGui::GetIO().Fonts->AddFontDefault();

			init = true;
		}
		else
			return originalPresent(pointerSwapChain, sync, flags); // returning original too
	}

	if (Utils::KeyPressed(VK_INSERT))
		RunBackend.m_bOpenMenu = !RunBackend.m_bOpenMenu;

	static float isRed = 0.0f, isGreen = 0.01f, isBlue = 0.0f;
	int FrameCount = ImGui::GetFrameCount();

	if (isGreen == 0.01f && isBlue == 0.0f) isRed += 0.01f;
	if (isRed > 0.99f && isBlue == 0.0f) {isRed = 1.0f; isGreen += 0.01f; }
	if (isGreen > 0.99f && isBlue == 0.0f) { isGreen = 1.0f; isRed -= 0.01f; }
	if (isRed < 0.01f && isGreen == 1.0f){ isRed = 0.0f; isBlue += 0.01f; }
	if (isBlue > 0.99f && isRed == 0.0f) { isBlue = 1.0f; isGreen -= 0.01f; } // ugliest function ive ever seen
	if (isGreen < 0.01f && isBlue == 1.0f) { isGreen = 0.0f; isRed += 0.01f; }
	if (isRed > 0.99f && isGreen == 0.0f) { isRed = 1.0f; isBlue -= 0.01f; }
	if (isBlue < 0.01f && isGreen == 0.0f) { isBlue = 0.0f; isRed -= 0.01f;
		if (isRed < 0.01f) isGreen = 0.01f; }

	Variables::RainbowColor = ImVec4(isRed, isGreen, isBlue, 1.0f);

	RunBackend.DrawImGui(RunBackend.m_gPointerContext, RunBackend.m_gMainRenderTargetView); // draw imgui every time
	return originalPresent(pointerSwapChain, sync, flags); // return the original so no stack corruption
}

bool Backend::Load()
{
	RunBackend.DirectXPresentHook(); // this always okay if game directx11
	MH_Initialize(); // aint no error checking cuz if minhook bad then its your problem 

	MH_CreateHook(reinterpret_cast<void**>(hookedPresent), &PresentHook, reinterpret_cast<void**>(&originalPresent)); 
	MH_EnableHook(hookedPresent); // hooking present

	return true;
}

void Backend::Unload()
{
	UnloadImGui(); // imgui unload
	UnloadDevices(true); // unloading all devices
}
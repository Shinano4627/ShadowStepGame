#include "DebugUI.h"

std::vector<std::function<void(void)>> DebugUI::m_debugfunction;
std::vector<int> DebugUI::m_idList;

void DebugUI::Init(ID3D11Device* device, ID3D11DeviceContext* context) 
{

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(Application::GetWindow()); // ウィンドウハンドルを取得
    ImGui_ImplDX11_Init(device, context);
}

void DebugUI::DisposeUI() {
    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

// デバッグ表示関数の登録
void DebugUI::RedistDebugFunction(int id, std::function<void()> func) {
    // 同じ ID がすでに存在するか確認
    auto it = std::find_if(m_idList.begin(), m_idList.end(),
        [id](const int& i) { return i == id; });

    if (it == m_idList.end()) {
        m_debugfunction.push_back({ std::move(func) });
        m_idList.push_back(id);
    }
}

void DebugUI::Render() {
    // ImGuiの新しいフレームを開始
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // ウィンドウとデバッグ情報の描画
    ImGui::Begin("Debug Information");
    ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

    ImGui::End();

    // デバッグ関数の実行
    for (auto& f : m_debugfunction)
    {
        f();
    }

    // フレームのレンダリングを完了
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

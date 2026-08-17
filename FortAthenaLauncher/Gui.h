#pragma once
#include "includes/imgui/imgui.h"
#include "Configure.h"
#include "AddBuild.h"
#include "Launch.h"

#include <d3d11.h>



extern ID3D11ShaderResourceView* g_ButtonIconTexture;

namespace UiConfig {
    int Hight = 500;
    int Width = 400;
}

namespace FortAthena::Gui {
    inline void RenderLauncherUI() {
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(UiConfig::Hight, UiConfig::Width));
        ImGui::Begin("Launcher", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground);

        ImGui::Text("Athena Launcher - Made by: Ducki67 (@ducki67 on discord)");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("Fortnite Installation Path:");
        ImGui::PushItemWidth(-105.0f);
        ImGui::InputText("##Path", Configure::GamePath, IM_ARRAYSIZE(Configure::GamePath));
        ImGui::PopItemWidth();
        ImGui::SameLine();
        if (ImGui::Button("Browse...", ImVec2(90, 0))) {
            Build::BrowseFortniteBuild(Configure::GamePath, IM_ARRAYSIZE(Configure::GamePath));
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();


        ImGui::Text("Email / Username:");
        ImGui::PushItemWidth(-1.0f);
        ImGui::InputText("##Email", Configure::Email, IM_ARRAYSIZE(Configure::Email));
        ImGui::PopItemWidth();

        ImGui::Spacing();

        ImGui::Text("Password:");
        ImGui::PushItemWidth(-1.0f);
        ImGui::InputText("##Password", Configure::Password, IM_ARRAYSIZE(Configure::Password), ImGuiInputTextFlags_Password);
        ImGui::PopItemWidth();

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();


        ImGui::Checkbox("Use Additional Launch Arguments", &Configure::bUseArgs);

        if (Configure::bUseArgs) {
            ImGui::Text("Custom Args:");
            ImGui::PushItemWidth(-1.0f);
            ImGui::InputText("##Args", Configure::AdditionalArgs, IM_ARRAYSIZE(Configure::AdditionalArgs));
            ImGui::PopItemWidth();
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        /*
        if (ImGui::Button("LAUNCH FORTNITE", ImVec2(-1.0f, 45))) {
            Configure::SaveConfig();
            Launch::LaunchFortniteWithRedirect(Configure::GamePath, Configure::Email, Configure::Password);
        }*/

        ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_Button]);

        ImVec2 buttonPos = ImGui::GetCursorScreenPos();

        if (ImGui::Button("##LaunchButton", ImVec2(-1.0f, 45))) {
            //Configure::LoadConfig();
            Configure::SaveConfig();
            Launch::LaunchFortniteWithRedirect(Configure::GamePath, Configure::Email, Configure::Password);
        }
        ImGui::PopStyleColor();

        ImGui::SetNextItemAllowOverlap();

        float textWidth = ImGui::CalcTextSize("Launch Fortnite").x;
        float totalWidth = 24.0f + 8.0f + textWidth;
        float startX = (ImGui::GetItemRectSize().x - totalWidth) * 0.5f;
        float startY = (45.0f - 24.0f) * 0.5f;

        ImGui::GetWindowDrawList()->PushClipRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), true);

        ImGui::SetCursorScreenPos(ImVec2(buttonPos.x + startX, buttonPos.y + startY));

        if (g_ButtonIconTexture)
        {
            ImGui::Image((void*)g_ButtonIconTexture, ImVec2(24, 24));
            ImGui::SameLine(0, 8);
        }

        ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x, buttonPos.y + (45.0f - ImGui::GetTextLineHeight()) * 0.5f));
        ImGui::Text("LAUNCH FORTNITE");

        ImGui::GetWindowDrawList()->PopClipRect();
        

        ImGui::End();
    }
}

#include "Window.hpp"


std::vector<WINDOWMANAGER::Window*> WINDOWMANAGER::Windows;

void WINDOWMANAGER::WindowGarbageCollector()
{
	if (!Windows.empty())
	{
		for (size_t i = 0; i < Windows.size(); i++)
		{
			if (!Windows[i]->IsOpen())
			{
				delete Windows[i];
				Windows.erase(Windows.begin() + i);
				LOG_INF("Collected a window!");
			}
		}
	}
}

void WINDOWMANAGER::DisposeWindows()
{
	for (Window* Window : Windows)
	{
		delete Window;
	}

	LOG_INF("Disposed the windows!");
}

void WINDOWMANAGER::UpdateWindows()
{
	for (size_t i = 0; i < Windows.size(); i++)
	{
		Windows[i]->Update();
	}
}

void WINDOWMANAGER::Window::Update()
{}

void WINDOWMANAGER::Window::UpdateLocalWindowSize()
{
	this->WindowSize.SetValues(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
}

void WINDOWMANAGER::Window::SetWindowName(std::string&& WindowName)
{
	this->WindowName = std::forward<std::string>(WindowName);
}

WINDOWMANAGER::OutlinerWindow::OutlinerWindow(scene& scene, DATA::ObjectSelectionState& SelectionState, UI::UIcolorShemePack &GlobalColorSheme ,Shader& ImportShader)
{
	this->GlobalColorSheme = &GlobalColorSheme;
	this->GlobalScene = &scene;
	this->GlobalSelectionState = &SelectionState;
	this->ImportShader = ImportShader.GetID();
}

void WINDOWMANAGER::OutlinerWindow::Update()
{
	if (WindowOpen) {

		ImGui::Begin("Outliner", &WindowOpen);
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImGui::ColorConvertFloat4ToU32(GlobalColorSheme->ChildMenuColor));
		ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 40.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 3.0f);

		for (size_t i = 1; i < GlobalScene->models.size(); i++)
		{

			if (ImGui::TreeNode(GlobalScene->models[i]->ModelName.c_str()))
			{

				if (ImGui::Selectable(("Select##Object" + std::to_string(i)).c_str()))
				{
					GlobalSelectionState->SelectedObject = i + 2;
				}
				ImGui::Indent();

				for (size_t t = 0; t < GlobalScene->models[i]->meshes.size(); t++)
				{
					if (ImGui::TreeNode(GlobalScene->models[i]->meshes[t].meshname.c_str()))
					{

						if (ImGui::Selectable(("Select##Object" + std::to_string(t)).c_str()))
						{

						}

						ImGui::TreePop();
					}
				}

				ImGui::TreePop();

				ImGui::Unindent();

			}

		}

		for (size_t i = 0; i < GlobalScene->lights.size(); i++)
		{

			if (ImGui::TreeNode(("Light" + std::to_string(i)).c_str()))
			{
				if (ImGui::Selectable(("Select##Object" + std::to_string(i)).c_str()))
				{
					GlobalSelectionState->SelectedLight = i + 1 + GlobalScene->GetModelCount() + 1;
				}

				if (ImGui::Selectable(("Delete##Object" + std::to_string(i)).c_str()))
				{
					GlobalScene->DeleteLight(i, this->ImportShader);
					std::string logtemp = "A light is deleted!";
					logs.push_back(logtemp);
					GlobalSelectionState->SelectedLight = 0;
				}


				ImGui::TreePop();

			}

		}

		for (size_t i = 0; i < Cameras.size(); i++)
		{
			if (ImGui::TreeNode(("Camera" + std::to_string(i)).c_str()))
			{
				if (ImGui::Selectable(("Set Active##Object" + std::to_string(i)).c_str()))
				{
					ActiveCameraID = i;
				}
				ImGui::TreePop();
			}
		}


		ImGui::PopStyleColor();
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();

		UpdateLocalWindowSize();
		ImGui::End();
	}
}

WINDOWMANAGER::LogsWindow::LogsWindow(UI::UIcolorShemePack& GlobalColorSheme)
{
	this->GlobalColorSheme = &GlobalColorSheme;
}

void WINDOWMANAGER::LogsWindow::Update()
{
	if (WindowOpen) {

		ImGui::Begin("Logs", &WindowOpen);
		UpdateLocalWindowSize();
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImGui::ColorConvertFloat4ToU32(GlobalColorSheme->MidMenuColor));
		ImGui::Spacing();

		if (logs.size() >= 1)
		{
			for (size_t i = 0; i < logs.size(); i++)
			{
				ImGui::Text(logs.at(i).c_str());
				ImGui::Separator();

				if (i == 4)
				{
					ImGui::Spacing();
					ImVec2 p = ImGui::GetCursorScreenPos();
					ImGui::GetWindowDrawList()->AddLine(ImVec2(p.x + (WindowSize.x * 0.01f), p.y), ImVec2(p.x + (WindowSize.x * 0.9f), p.y), ImGui::GetColorU32(GlobalColorSheme->ChildMenuColor), 2.0f);
					ImGui::Spacing();
					ImGui::Spacing();
				}
			}
		}

		ImGui::PopStyleColor();

		

		ImGui::End();

		
	

	}
}

WINDOWMANAGER::RenderSettings::RenderSettings()
{
}

void WINDOWMANAGER::RenderSettings::Update()
{


}

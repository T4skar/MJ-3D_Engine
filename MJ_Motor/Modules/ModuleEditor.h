#pragma once
#include "Module.h"
#include "Globals.h"
#include"GameObject.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_sdl2.h"
#include "ImGui/imgui_impl_opengl3.h"

#include <vector>
#include <map>

using namespace std;

struct Console
{
	//Parameters
	ImGuiTextBuffer Buf;
	ImGuiTextFilter Filter;
	ImVector<int> LineOffsets;

	bool ScrollToBottom;

	//Functions
	void Clear()
	{
		Buf.clear();
	}

	void AddLog(const char file[], int line, const char* logText, ...)
	{
		static char tmp_string[4096];
		static char tmp_string2[4096];
		static va_list  ap;

		va_start(ap, logText);
		vsprintf_s(tmp_string, 4096, logText, ap);

		va_end(ap);
		sprintf_s(tmp_string2, 4096, "\n%s(%d) : %s", file, line, tmp_string);

		OutputDebugString(tmp_string2);

		int old_size = Buf.size();
		va_list args;
		va_start(args, logText);
		Buf.appendfv(logText, args);
		Buf.appendfv("\n", args);
		va_end(args);

		for (int new_size = Buf.size(); old_size < new_size; old_size++)
		{
			if (Buf[old_size] == '\n')
			{
				LineOffsets.push_back(old_size);
			}
		}
		ScrollToBottom = true;
	}

	void Draw(const char* title, bool* p_opened = NULL)
	{
		ImGui::SetNextWindowSize(ImVec2(500, 400));
		ImGui::Begin(title, p_opened);

		if (ImGui::Button("Clear"))
		{
			Clear();
		}

		ImGui::SameLine();
		bool copy = ImGui::Button("Copy");
		ImGui::Separator();
		ImGui::BeginChild("scrolling");
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 1));

		if (copy)
		{
			ImGui::LogToClipboard();
		}

		ImGui::TextUnformatted(Buf.begin());

		if (ScrollToBottom)
		{
			ImGui::SetScrollHereY(1.0f);
		}

		ScrollToBottom = false;

		ImGui::PopStyleVar();
		ImGui::EndChild();
		ImGui::End();
	}
};

class ModuleEditor : public Module
{
public:
	ModuleEditor(Application* app, bool start_enabled = true);
	~ModuleEditor();

	SDL_GLContext context;


	bool Start();
	bool Init();
	void DrawEditor();
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void DisplayGameObjects(GameObject* game_object);

	//ImGui Windows
	void ImGuiRenderWindow();
	void ImGuiMainWindow();
	void ImGuiInspectorWindow();
	void ImGuiCreditsWindow();
	void ImGuiLicenseWindow();
	void GameViewport();
	void AssetsWindow();

	//GameObjects
	uint CreateGameObject(GameObject* parent, std::string name);

	GameObject* gameobject_selected = nullptr;
	static std::map<uint, GameObject*> gameObjects;


	//ImGui Tool Bar Window Booleans
	bool showScene = true;
	bool showGame = true;
	bool showMain = true;
	bool showInspector = true;
	bool showConsole = true;
	bool showAbout = false;
	bool showHierarchy = true;
	bool showCameraWindow = true;
	bool showAssetsWindow = true;
	bool show_AABB = false;

	//Inspector booleans
	bool showCubeDirectMode = false;
	bool showCubeCheckers = false;

	//Renderer booleans
	bool activateWireframe = false;
	bool activateVSYNC = true;
	bool activateDepthTest = true;
	bool activateCullFace = true;
	bool activateLighting = true;
	bool activateColorMaterial = true;

	//Input booleans
	bool activateDragAndDrop = true;

	//Play and pause booleans
	bool playPressed;
	bool pausePressed;

	//Basic Shapes file paths
	const char* cube_file_path = "Assets/BasicShapes/cube.fbx";
	const char* sphere_file_path = "Assets/BasicShapes/sphere.fbx";
	const char* cone_file_path = "Assets/BasicShapes/cone.fbx";
	const char* cylinder_file_path = "Assets/BasicShapes/cylinder.fbx";

	//About booleans
	bool showCredits = false;
	bool showLicense = false;

	//Console struct
	Console console_log;

	//FPS Graph vectors
	vector<float> fps_log;
	vector<float> ms_log;

	//FPS Graph functions
	void PushLog(std::vector<float>* Log, float toPush);
};
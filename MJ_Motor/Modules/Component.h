#pragma once
#include "Globals.h"
#include "Application.h"
#include "GameObject.h"


#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_sdl2.h"
#include "ImGui/imgui_impl_opengl3.h"


#include <iostream>
#include <string>
#include <vector>

class GameObject;

class Component
{
public:
	enum class TYPE
	{
		NONE,
		TRANSFORM,
		MESH,
		TEXTURE,
		//MATERIAL,
		CAMERA,
		//NUM_OF_COMPS,
	};


	Component(GameObject* game_object, TYPE type);
	virtual ~Component();

	virtual void OnGui();
	virtual void Update();

	TYPE GetType() { return type; }

protected:
	TYPE type = TYPE::NONE;
	GameObject* go = nullptr;
};

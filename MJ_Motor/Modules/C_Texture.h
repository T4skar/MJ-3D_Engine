#pragma once
#include "Globals.h"
#include "Application.h"
#include "GameObject.h"
#include "FBXLoader.h"

#include <iostream>
#include <string>
#include <vector>



class C_Texture : public Component
{
public:
	C_Texture(GameObject* gameObject);
	~C_Texture();

	void SetTexture(const char* texture_name);

	void OnGui();


	const char* name;
};

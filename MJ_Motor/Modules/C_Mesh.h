#pragma once
#include "Globals.h"
#include "Application.h"
#include "GameObject.h"
#include "FBXLoader.h"

#include <iostream>
#include <string>
#include <vector>



class C_Mesh : public Component
{
public:
	C_Mesh(GameObject* gameObject);
	~C_Mesh();

	void SetMesh(MeshStorer* mesh, std::string mesh_name);

	void OnGui();

	void Update();

	bool CheckInFrustrum();

	MeshStorer* GetMesh()
	{
		return mesh;
	}

	std::string name;
	MeshStorer* mesh;
};
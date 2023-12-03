#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"


class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void Look(const float3& Position, const float3& Reference, bool RotateAroundReference = false);
	void LookAt(const float3& Spot);
	void Move(const float3& Movement);
	float* GetViewMatrix();

	//ImVec2 Normalize(float x, float y, float w, float h, ImVec2 point);

	bool firstPersonView = false;

	void CalculateViewMatrix();

public:

	// You won't need this after using Frustum
	vec3 X, Y, Z, Position, Reference;
	Frustum frustum;
	float4x4 ViewMatrix;
	float3 reference;
};
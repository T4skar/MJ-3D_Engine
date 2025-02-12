#pragma once
#include "Module.h"
#include "Globals.h"
#include "C_Camera.h"
#include "MathGeoLib/include/Math/float3.h"
#include "MathGeoLib/include/Math/float4x4.h"


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

	C_Camera* cam;


	

	bool firstPersonView = false;

private:

	void CalculateViewMatrix();

public:

	//You won't need this after using Frustum
	float3 X, Y, Z, Position, Reference;

private:

	mat4x4 ViewMatrix;
	//Frustum mMainCamera; Some help here :)
};
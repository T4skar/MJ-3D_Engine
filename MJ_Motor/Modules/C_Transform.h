#pragma once
#include "Globals.h"
#include "Application.h"
#include "GameObject.h"

#include "MathGeoLib/include/MathGeoLib.h"


#include <iostream>
#include <string>
#include <vector>

struct Transform
{
	float3 eulRotation;
	Quat quatRotation;

	float4x4 globalPos;
	float4x4 localPos;

	float4x4 transGlobalPos;

	float3 position;

	Quat rotation;

	float3 scale;
};

class C_Transform : public Component
{
public:
	C_Transform(GameObject* gameObject);
	~C_Transform();

	void SetTransform(float3 position, Quat rotation, float3 scale);
	void Update() override;

	float* GetGlobalTransposed();
	float4x4 GetGlobalMatrix();

	void OnGui();

	Transform transform;
};
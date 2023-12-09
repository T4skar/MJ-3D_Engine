#pragma once
#include "Globals.h"
#include "Application.h"
#include "GameObject.h"
#include "FBXLoader.h"

#include <iostream>
#include <string>
#include <vector>



class C_Camera : public Component
{
public:

	C_Camera(GameObject* game_object);
	
	~C_Camera();

	void SetCam();
	void InitFrameBuffer();

	float* GetViewMatrix();
	float* GetProjectionMatrix();
	void LookAt(const float3& target);

	void OnGui();


	Frustum frustum;
	float3 reference;

	int typeCameraSelected = 0;

	int cameraFOV = 100;
	float farDistance = 500.0f;
	float nearDistance = 0.1f;
	int printCount;

	bool isMainCamera = false;

	unsigned int cameraBuffer;
	unsigned int frameBuffer;

	float4x4 viewMatrix;
	float4x4 projectionMatrix;

private:

	unsigned int renderObjBuffer;

};
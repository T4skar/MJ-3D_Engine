#pragma once
#include "Globals.h"
#include "Application.h"
#include "GameObject.h"
#include "Component.h"
#include "C_Camera.h"
#include "C_Transform.h"

#include <iostream>
#include <string>
#include <vector>

C_Camera::C_Camera(GameObject* gameObject) : Component(gameObject, TYPE::CAMERA)
{
    go = nullptr;
    printCount = 0;
    SetCam();
    InitFrameBuffer();
}

C_Camera::~C_Camera()
{
	glDeleteFramebuffers(1, &cameraBuffer);
	glDeleteFramebuffers(1, &frameBuffer);
	glDeleteFramebuffers(1, &renderObjBuffer);
}
void C_Camera::SetCam()
{
	go = nullptr;
	frustum.type = FrustumType::PerspectiveFrustum;
	frustum.nearPlaneDistance = 0.1f;
	frustum.farPlaneDistance = 500.f;
	frustum.front = float3::unitZ;
	frustum.up = float3::unitY;

	frustum.verticalFov = cameraFOV * DEGTORAD;
	frustum.horizontalFov = 2.0f * atanf(tanf(frustum.verticalFov / 2.0f) * 1.7f);

	frustum.pos = float3(0, 0, 0);
}


void C_Camera::InitFrameBuffer()
{
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	glGenTextures(1, &cameraBuffer);
	glBindTexture(GL_TEXTURE_2D, cameraBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	float color[4] = { 0.1,0.1,0.1,0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, cameraBuffer, 0);

	glGenRenderbuffers(1, &renderObjBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderObjBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREEN_WIDTH, SCREEN_HEIGHT);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderObjBuffer);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		LOG("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


float* C_Camera::GetViewMatrix()
{

	viewMatrix = frustum.ViewMatrix();

	viewMatrix.Transpose();

	return viewMatrix.ptr();
}

float* C_Camera::GetProjectionMatrix()
{

	projectionMatrix = frustum.ProjectionMatrix();

	projectionMatrix.Transpose();

	return projectionMatrix.ptr();
}



void C_Camera::LookAt(const float3& target)
{
	frustum.front = (target - frustum.pos).Normalized();
	float3 X = float3(0, 1, 0).Cross(frustum.front).Normalized();
	frustum.up = frustum.front.Cross(X);
}



void C_Camera::OnGui()
{
	if (ImGui::CollapsingHeader("Camera"))
	{
		ImGui::Text("");
		ImGui::Text("Rendered objects: %d", printCount);
		ImGui::Text("");

		if (ImGui::SliderInt("FOV", &cameraFOV, 5, 180)) {
			frustum.verticalFov = cameraFOV * DEGTORAD;
			frustum.horizontalFov = 2.0f * atanf(tanf(frustum.verticalFov / 2.0f) * 1.7f);
		}
		if (ImGui::Button("Reset FOV")) {
			cameraFOV = 60.0f;

			frustum.verticalFov = cameraFOV * DEGTORAD;
			frustum.horizontalFov = 2.0f * atanf(tanf(frustum.verticalFov / 2.0f) * 1.7f);
		}


		ImGui::SliderFloat("Near Distance", &frustum.nearPlaneDistance, 0.1f, frustum.farPlaneDistance);
		if (ImGui::Button("Reset Near Distance")) {
			frustum.nearPlaneDistance = 0.1f;
		}

		ImGui::InputFloat("Far Distance", &frustum.farPlaneDistance);
		if (ImGui::Button("Reset Far Distance")) {
			frustum.farPlaneDistance = 500.f;
		}
		ImGui::Text("");
		if (ImGui::Button("Set Main Camera", ImVec2(120, 50))) {
			App->renderer3D->Camera;
		}
	}
}

bool C_Camera::ContainsAaBox(MeshStorer* refBox)
{
	float3 vCorner[8];
	int iTotalIn = 0;
	refBox->globalAABB.GetCornerPoints(vCorner); 

	Plane m_plane[6];
	frustum.GetPlanes(m_plane);

	for (int p = 0; p < 6; ++p) {
		int iInCount = 8;
		int iPtIn = 1;
		for (int i = 0; i < 8; ++i) {
			// test this point against the planes
			if (m_plane[p].IsOnPositiveSide(vCorner[i])) 
			{
				iPtIn = 0;
				--iInCount;
			}
		}
		
		if (iInCount == 0)
			return false;
		
		iTotalIn += iPtIn;
	}
	
	if (iTotalIn == 6)
		return true;
}
#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "glmath.h"

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	/*X = float3(1.0f, 0.0f, 0.0f);
	Y = float3(0.0f, 1.0f, 0.0f);
	Z = float3(0.0f, 0.0f, 1.0f);

	Position = float3(7.0f, 5.0f, 7.0f);
	Reference = float3(0.0f, 0.0f, 0.0f);*/
	//ViewMatrix = IdentityMatrix;

	CalculateViewMatrix();

	frustum.type = PerspectiveFrustum;
	frustum.nearPlaneDistance = 0.1f;
	frustum.farPlaneDistance = 400.f;
	frustum.front = float3::unitZ;
	frustum.up = float3::unitY;
	frustum.verticalFov = 60.0f * DEGTORAD;
	frustum.horizontalFov = 2.0f * atanf(tanf(frustum.verticalFov / 2.0f) * (16.f / 9.f));

	frustum.pos = float3(0, 5, -20);
}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	LOG("Setting up the camera");
	App->editor->console_log.AddLog(__FILE__, __LINE__, "Setting up the camera");

	bool ret = true;

	return ret;
}



// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{
	// Implement a debug camera with keys and mouse
	// Now we can make this movememnt frame rate independant!

	float3 newPos(0,0,0);
	float speed = 15.0f * dt;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) {
		speed = 20.0f * dt;
	}
		

	//Center camera to origin
	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
	{
		LookAt(float3(0, 0, 0));
	}

	//Set camera in First Person
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		firstPersonView = !firstPersonView;

		if (firstPersonView)
		{
			LOG("First Person ON");
			App->editor->console_log.AddLog(__FILE__, __LINE__, "First Person ON");
			Reference = Position - (Position * 0.01);
		}
		else
		{
			LOG("First Person OFF");
			App->editor->console_log.AddLog(__FILE__, __LINE__, "First Person OFF");
			//Reference = float3(0.0f, 0.0f, 0.0f);
		}
	}

	//if(App->input->GetMouseButton(SDL_MOUSEWHEEL) == KEY_REPEAT) newPos.z += speed;
	//if(App->input->GetMouseButton(SDL_MOUSEWHEEL) == KEY_REPEAT) newPos.z -= speed;

	//if (App->input->GetMouseZ() != 0 && App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT) newPos -= Z * speed * App->input->GetMouseZ();

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT && App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT) frustum.pos += frustum.front * speed;
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT && App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT) frustum.pos -= frustum.front * speed;

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT) frustum.pos -= frustum.WorldRight() * speed;
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT) frustum.pos += frustum.WorldRight() * speed;


	if (App->input->GetMouseZ() != 0) frustum.pos += frustum.front * speed * App->input->GetMouseZ();

	/*Position += newPos;
	Reference += newPos;*/

	// Mouse motion ----------------

	if(App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
	{
		int dx = -App->input->GetMouseXMotion();
		int dy = -App->input->GetMouseYMotion();

		float Sensitivity = 7.0f * dt;

		Quat dir;
		frustum.WorldMatrix().Decompose(float3(), dir, float3());

		Position -= Reference;

		if (dx != 0)
		{
			float DeltaX = (float)dx * Sensitivity;
			Quat X = Quat::identity;
			X.SetFromAxisAngle(float3(0.0f, 1.0f, 0.0f), DeltaX * DEGTORAD);
			dir = X * dir;
		}

		if (dy != 0)
		{
			float DeltaY = (float)dy * Sensitivity;
			Quat Y = Quat::identity;
			Y.SetFromAxisAngle(float3(1.0f, 0.0f, 0.0f), DeltaY * DEGTORAD);
			dir = dir * Y;
		}

		float4x4 rm = frustum.WorldMatrix();
		rm.SetRotatePart(dir.Normalized());
		frustum.SetWorldMatrix(rm.Float3x4Part());
	}


	// Recalculate matrix -------------
	CalculateViewMatrix();

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOG("Cleaning camera");

	return true;
}

// -----------------------------------------------------------------
void ModuleCamera3D::Look(const float3& Position, const float3& Reference, bool RotateAroundReference)
{
	frustum.pos = Position;
	reference = Reference;

	LookAt(reference);

	if (!RotateAroundReference)
	{
		reference = Position;
	}
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt(const float3& Spot)
{
	reference = Spot;
	frustum.front = (Spot - frustum.pos).Normalized();
	float3 X = float3(0, 1, 0).Cross(frustum.front).Normalized();
	frustum.up = frustum.front.Cross(X);

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void ModuleCamera3D::Move(const float3& Movement)
{
    frustum.pos += Movement;

    CalculateViewMatrix();
}

// -----------------------------------------------------------------
float* ModuleCamera3D::GetViewMatrix()
{
	return ViewMatrix.ptr();
}

// -----------------------------------------------------------------
void ModuleCamera3D::CalculateViewMatrix()
{
	ViewMatrix = frustum.ViewMatrix();
	ViewMatrix.Transpose();
}

//ImVec2 ModuleCamera3D::Normalize(float x, float y, float w, float h, ImVec2 point)
//{
//	ImVec2 normalizedPoint;
//
//	normalizedPoint.x = (point.x - x) / ((x + w) - x);
//	normalizedPoint.y = (point.y - y) / ((y + h) - y);
//
//	return normalizedPoint;
//}
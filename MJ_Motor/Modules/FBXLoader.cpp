#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#pragma comment (lib, "Assimp/libx86/assimp.lib")

#include "FBXLoader.h"
#include "Application.h"
#include "TexLoader.h"



vector <MeshStorer*>FBXLoader::meshesVector; 

void FBXLoader::Debug()
{
	// Stream log messages to Debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);
}

void FBXLoader::FileLoader(const char* file_path)
{
	const aiScene* scene = aiImportFile(file_path, aiProcessPreset_TargetRealtime_MaxQuality);



	if (scene != nullptr && scene -> HasMeshes())
	{

		GameObject* FbxGameObject = new GameObject(App->editor->gameObjects[0], file_path);


		for (int i = 0; i < scene->mRootNode->mNumChildren; i++)
		{
			MeshStorer* ourMesh = new MeshStorer();

			aiMesh* ai_mesh = scene->mMeshes[scene->mRootNode->mChildren[i]->mMeshes[0]];

			// Copy vertices
			ourMesh->num_vertex = ai_mesh->mNumVertices;
			ourMesh->vertex = new float[ourMesh->num_vertex * VERTEX_FEATURES];
			LOG("New mesh with %d vertex", ourMesh->num_vertex);
					
			for (int v = 0; v < ourMesh->num_vertex; v++) {

				ourMesh->vertex[v * VERTEX_FEATURES] = ai_mesh->mVertices[v].x;
				ourMesh->vertex[v * VERTEX_FEATURES + 1] = ai_mesh->mVertices[v].y;
				ourMesh->vertex[v * VERTEX_FEATURES + 2] = ai_mesh->mVertices[v].z;
			
				if (ai_mesh->HasTextureCoords(0))
				{
					ourMesh->vertex[v * VERTEX_FEATURES + 3] = ai_mesh->mTextureCoords[0][v].x;
					ourMesh->vertex[v * VERTEX_FEATURES + 4] = ai_mesh->mTextureCoords[0][v].y;
				}
			}
		
			// Copy faces
			if (ai_mesh->HasFaces())
			{
				ourMesh->num_index = ai_mesh->mNumFaces * 3;
				ourMesh->index = new uint[ourMesh->num_index]; // assume each face is a triangle
				for (uint j = 0; j < ai_mesh->mNumFaces; ++j)
				{
					if (ai_mesh->mFaces[j].mNumIndices != 3)
					{
						LOG("WARNING, geometry face with != 3 indices!");
					}
					else
					{
						memcpy(&ourMesh->index[j * 3], ai_mesh->mFaces[j].mIndices, 3 * sizeof(uint));
						LOG("New mesh with %d index", ourMesh->num_index);

					}
				}
			}

			ourMesh->ID = App->editor->CreateGameObject(FbxGameObject, scene->mRootNode->mChildren[i]->mName.C_Str());

			//Load Texture
			ourMesh->id_texture = TexLoader::LoadTexture(ourMesh->bakerHouseTexPath);

			//Generate Buffer
			FBXLoader::GenerateMeshBuffer(ourMesh);

			//Store Mesh
			meshesVector.push_back(ourMesh);

		}

		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		aiReleaseImport(scene);

		//ourMesh->RenderOneMesh();
	}
	else
		LOG("Error loading scene % s", file_path);
}

void FBXLoader::RenderAll() 
{
	for (int i = 0; i < meshesVector.size(); i++) 
	{
		meshesVector[i]->RenderOneMesh();
	}
}

void MeshStorer::RenderOneMesh()
{
	//Enable vertex arrays
	glBindBuffer(GL_ARRAY_BUFFER, id_vertex);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_index);

	glBindTexture(GL_TEXTURE_2D, id_texture);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//Before draw, specify vertex and index arrays with their offsets
	glVertexPointer(3, GL_FLOAT, sizeof(float) * VERTEX_FEATURES, NULL);
	glTexCoordPointer(2, GL_FLOAT, sizeof(float) * VERTEX_FEATURES, (void*)(3 * sizeof(float)));
	glNormalPointer(GL_FLOAT, sizeof(float) * VERTEX_FEATURES, NULL);

	glDrawElements(GL_TRIANGLES,            // primitive type
		num_index,                      // # of indices
		GL_UNSIGNED_INT,         // data type
		(void*)0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
	glDisable(GL_TEXTURE_COORD_ARRAY);
}


void FBXLoader::GenerateMeshBuffer(MeshStorer* ourMesh)
{
	//Mesh Buffer
	//Mesh Buffer Generator
	glGenBuffers(1, (GLuint*)&(ourMesh->id_vertex));
	glGenBuffers(1, (GLuint*)&(ourMesh->id_index));
	glBindBuffer(GL_ARRAY_BUFFER, ourMesh->id_vertex);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ourMesh->id_index);

	//Mesh Buffer Data
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * ourMesh->num_vertex * VERTEX_FEATURES, ourMesh->vertex, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * ourMesh->num_index, ourMesh->index, GL_STATIC_DRAW);

	//Close Mesh Buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


void MeshStorer::RenderMesh(const GLfloat* globalTransform, uint texID)
{

	//Wireframe mode
	/*if (App->editorGui->wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);*/


	//Enable/Disable lights
	if (App->editor->activateLighting == false)
		glDisable(GL_LIGHTING);
	else
		glEnable(GL_LIGHTING);

	//Enable/Disable depth test
	if (App->editor->activateDepthTest == false)
		glDisable(GL_DEPTH_TEST);
	else
		glEnable(GL_DEPTH_TEST);

	//Enable/Disable cull face
	if (App->editor->activateCullFace == false)
		glDisable(GL_CULL_FACE);
	else
		glEnable(GL_CULL_FACE);

	RenderAABB();

	// Bind Buffers
	glBindBuffer(GL_ARRAY_BUFFER, id_vertex);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_index);

	glBindTexture(GL_TEXTURE_2D, texID);


	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	//glEnableClientState(GL_NORMAL_ARRAY);

	// Vertex Array [ x, y, z, u, v ]
	glVertexPointer(3, GL_FLOAT, sizeof(float) * VERTEX_FEATURES, NULL);
	glTexCoordPointer(2, GL_FLOAT, sizeof(float) * VERTEX_FEATURES, (void*)(3 * sizeof(float)));
	glNormalPointer(GL_FLOAT, sizeof(float) * VERTEX_FEATURES, NULL);


	glPushMatrix();
	glMultMatrixf(globalTransform);

	// Draw
	glDrawElements(GL_TRIANGLES, num_index, GL_UNSIGNED_INT, NULL);

	glPopMatrix();

	glClientActiveTexture(GL_TEXTURE0);

	// Unbind buffers
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisable(GL_TEXTURE_COORD_ARRAY);

}

void MeshStorer::GenerateLocalBoundingBox()
{

	//TODO: Should this be here?
	float* vertex_positions = new float[num_vertex * 3];
	for (size_t i = 0; i < num_vertex; i++)
	{
		vertex_positions[i * 3] = vertex[i * VERTEX_FEATURES];
		vertex_positions[i * 3 + 1] = vertex[i * VERTEX_FEATURES + 1];
		vertex_positions[i * 3 + 2] = vertex[i * VERTEX_FEATURES + 2];
	}

	localAABB.SetNegativeInfinity();
	localAABB.Enclose((float3*)vertex_positions, num_vertex);
	localAABB_init = true;

}

void MeshStorer::GenerateGlobalBoundingBox()
{
	// Generate global OBB
	globalOBB = localAABB;
	//globalOBB.Transform(App->scene_intro->gameObjects[ID]->transform->GetGlobalMatrix());

	// Generate global AABB
	globalAABB.SetNegativeInfinity();
	globalAABB.Enclose(globalOBB);
}

void MeshStorer::RenderAABB()
{
	float3 vertexAABB[8];

	globalAABB.GetCornerPoints(vertexAABB);


	if (App->editor->show_AABB)
	{
		glBegin(GL_LINES);
		glVertex3f(vertexAABB[0].x, vertexAABB[0].y, vertexAABB[0].z);
		glVertex3f(vertexAABB[1].x, vertexAABB[1].y, vertexAABB[1].z);
		glVertex3f(vertexAABB[0].x, vertexAABB[0].y, vertexAABB[0].z);
		glVertex3f(vertexAABB[4].x, vertexAABB[4].y, vertexAABB[4].z);
		glVertex3f(vertexAABB[1].x, vertexAABB[1].y, vertexAABB[1].z);
		glVertex3f(vertexAABB[5].x, vertexAABB[5].y, vertexAABB[5].z);
		glVertex3f(vertexAABB[4].x, vertexAABB[4].y, vertexAABB[4].z);
		glVertex3f(vertexAABB[5].x, vertexAABB[5].y, vertexAABB[5].z);

		glVertex3f(vertexAABB[2].x, vertexAABB[2].y, vertexAABB[2].z);
		glVertex3f(vertexAABB[3].x, vertexAABB[3].y, vertexAABB[3].z);
		glVertex3f(vertexAABB[2].x, vertexAABB[2].y, vertexAABB[2].z);
		glVertex3f(vertexAABB[6].x, vertexAABB[6].y, vertexAABB[6].z);
		glVertex3f(vertexAABB[6].x, vertexAABB[6].y, vertexAABB[6].z);
		glVertex3f(vertexAABB[7].x, vertexAABB[7].y, vertexAABB[7].z);
		glVertex3f(vertexAABB[3].x, vertexAABB[3].y, vertexAABB[3].z);
		glVertex3f(vertexAABB[7].x, vertexAABB[7].y, vertexAABB[7].z);

		glVertex3f(vertexAABB[0].x, vertexAABB[0].y, vertexAABB[0].z);
		glVertex3f(vertexAABB[2].x, vertexAABB[2].y, vertexAABB[2].z);
		glVertex3f(vertexAABB[1].x, vertexAABB[1].y, vertexAABB[1].z);
		glVertex3f(vertexAABB[3].x, vertexAABB[3].y, vertexAABB[3].z);
		glVertex3f(vertexAABB[4].x, vertexAABB[4].y, vertexAABB[4].z);
		glVertex3f(vertexAABB[6].x, vertexAABB[6].y, vertexAABB[6].z);
		glVertex3f(vertexAABB[5].x, vertexAABB[5].y, vertexAABB[5].z);
		glVertex3f(vertexAABB[7].x, vertexAABB[7].y, vertexAABB[7].z);
		glEnd();
	}
}

void FBXLoader::CleanUp()
{
	// detach log stream
	aiDetachAllLogStreams();

	meshesVector.clear();
}

#pragma once
#include "Globals.h"
#include <string>
#include <vector>

#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#pragma comment (lib, "Assimp/libx86/assimp.lib")



#include "MathGeoLib/include/MathGeoLib.h"

//It has 5 vertex features for x,y,z,u,v
#define VERTEX_FEATURES 5

using namespace std;

class GameObject;

struct MeshStorer
{
	MeshStorer() {}

	~MeshStorer() {
		delete[num_vertex]vertex;
		delete[num_index]index;
	}

	//Parameters
	uint id_index = 0; // index in VRAM
	uint num_index = 0;
	uint* index = nullptr;
	uint id_vertex = 0; // unique vertex in VRAM
	uint num_vertex = 0;
	float* vertex = nullptr;
	GLuint id_texture;

	uint ID;

	const char* bakerHouseTexPath = "Assets/BakerHouse/Baker_house.png";

	bool localAABB_init = false;
	AABB localAABB;
	AABB globalAABB;
	OBB globalOBB;


	//Methods
	void GenerateGlobalBoundingBox();
	void GenerateLocalBoundingBox();

	void RenderOneMesh();
	void RenderMesh(const GLfloat* globalTransform, uint texID);
	void RenderAABB();

};

class FBXLoader
{
public:

	//Methods
	void Debug();
	static void FileLoader(const char* file_path);
	static void RenderAll();
	static void CleanUp();

	static void GenerateMeshBuffer(MeshStorer* ourMesh);
	static void GetNodeInfo(const aiScene* rootScene, aiNode* rootNode, GameObject* GameObgectFather);

	static vector <MeshStorer*> meshesVector;
};

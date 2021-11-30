#include "Renderer.h"
#include "../nclgl/Light.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/WaterMesh.h"
#include "../nclgl/Shader.h"
#include "../nclgl/Camera.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/MeshAnimation.h"
#include "../nclgl/MeshMaterial.h"
#include <time.h>
#include <algorithm>

Renderer::Renderer(Window& parent) :OGLRenderer(parent) {

	currentFrame = 0;
	frameTime = 0.0f;

	// Init Environment
	skyBox = Mesh::GenerateQuad();
	postScene = Mesh::GenerateQuad();
	heightMap = new HeightMap(TEXTUREDIR"deckardIsland.png");
	heightmapSize = heightMap->GetHeightMapSize();
	water = new WaterMesh(heightmapSize);
	waveSpeed = 0.009;
	waveTime = 0.0;
	grassWaveTime = 0.0;
	grassWaveSpeed = 0.03;
	waterLineSwitch = false;
	camForwardSwitch = false;
	camRotateSwitch = false;
	camSpeed = 5.0;
	camOffset = 0.0;
	camMode = 1;
	drawMode = NORMAL_DRAW;
	
	LoadTextures();

	LoadShaders();

	LoadMeshes();

	AddNodes();

	light = new Light(heightmapSize * Vector3(0.5f, 30.5f, 0.5f),
		Vector4(1, 1, 1, 1), heightmapSize.x);
	fireLight = new Light(heightmapSize * Vector3(0.51f, 1.4f, 0.635f),
		Vector4(0.737, 0.29, 0.173, 1.0), heightmapSize.x/15);
	camera = new Camera(0.0f, 0.0f, heightmapSize * Vector3(0.5f, 4.0f, 2.0f));
	subCam = new Camera(0.0f, 13.0f, heightmapSize * Vector3(0.48f, 2.0f, 0.73f));
	curCam = camera;
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);

	// init Post processing 
	InitPostProcess();

	// init Deffered rendering
	InitDefferedRender();

	// init Shadow mappings
	InitShadowRender();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glDepthFunc(GL_LESS);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	init = true;
}

Renderer::~Renderer(void) {
	delete camera;
	delete light;
	delete fireLight;
	delete heightMap;
	delete water;
	delete skyBox;
	delete postScene;
	delete blurShader;
	delete skyboxShader;
	delete terrainShader;
	delete toLineShader;
	delete waterLineShader;
	delete turtleShader;
	delete bonfireShader;
	delete waterShader;
	delete grassShader;
	delete treeShader;
	delete postShader;
	delete deferShader;
	delete combineShader;
	delete pointLightShader;
	delete shadowShader;
	delete lightSphere;
	delete defferedScene;
	delete[] pointLights;
	delete turtleAnim;
	delete turtleMat;
	delete bonfireMat;
	
	// delete post processing items
	glDeleteTextures(2, bufferColourTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &processFBO);

	// delete deffered rendering items
	glDeleteTextures(1, &dfBufferColourTex);
	glDeleteTextures(1, &dfBufferNormalTex);
	glDeleteTextures(1, &dfBufferDepthTex);
	glDeleteTextures(1, &lightDiffuseTex);
	glDeleteTextures(1, &lightSpecularTex);
	glDeleteFramebuffers(1, &gBufferFBO);
	glDeleteFramebuffers(1, &pointLightFBO);

	// delete shadow mapping items
	glDeleteTextures(1, &shadowTex);
	glDeleteFramebuffers(1, &shadowFBO);

}


void Renderer::UpdateScene(float dt) {
	camera->UpdateCamera(dt);
	subCam->UpdateCamera(dt);
	camOffset += 0.07;
	if (camForwardSwitch)camera->SetPosition(camera->GetPosition() + Vector3(0.0, 0.0, -camSpeed));
	if (camRotateSwitch)camera->SetYaw(camera->GetyYaw() + 3*sin(camOffset));

	viewMatrix = curCam->BuildViewMatrix();

	waveTime += waveSpeed;
	grassWaveTime += grassWaveSpeed;

	root->Update(dt);
	frameTime -= dt;
	while (frameTime < 0.0f) {
		currentFrame = (currentFrame + 1) % turtleAnim->GetFrameCount();
		frameTime += 1.0f / turtleAnim->GetFrameRate();
	}
}

void Renderer::RenderScene() {
	switch (drawMode)
	{
	case NORMAL_DRAW:
		NormalDraw();
		break;

	case BLUR_DRAW:
		PreDrawScene();
		DrawPostProcess();
		DrawScene();
		break;
	case DEFFERED_DRAW:
		DefferedDraw();
		break;
	default:
		NormalDraw();
		break;
	}
	
}

void Renderer::NormalDraw() {
	BuildNodeLists(root);
	SortNodeLists();
	
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	UpdateShaderMatrices();
	DrawSkybox();
	DrawLowPolyTerrain();
	DrawGrass();
	DrawNodes();
	DrawLowPolyWater();

	ClearNodeLists();
}


void Renderer::LoadShaders() {

	toLineShader = new Shader("LineVertex.glsl", "LineFragment.glsl", "LineGeometry.glsl");
	terrainShader = new Shader("TerrainVertex.glsl", "TerrainFragment.glsl");
	grassShader = new Shader("GrassVertex.glsl", "GrassFragment.glsl", "GrassGeometry.glsl");
	skyboxShader = new Shader("skyboxVertex.glsl", "skyboxFragment.glsl");
	waterShader = new Shader("waterVertex.glsl", "waterFragment.glsl");
	waterLineShader = new Shader("waterVertex.glsl", "waterFragment.glsl","LineGeometry.glsl");
	turtleShader = new Shader("turtleVertex.glsl", "turtleFragment.glsl");
	treeShader = new Shader("TexturedVertex.glsl", "TexturedFragment.glsl");
	blurShader = new Shader("blurVertex.glsl", "blurFragment.glsl");
	postShader = new Shader("TexturedVertex.glsl", "TexturedFragment.glsl");
	deferShader = new Shader("deferVertex.glsl", "deferFragment.glsl");
	pointLightShader = new Shader("pointLightVertex.glsl", "pointLightFragment.glsl");
	combineShader = new Shader("combineVertex.glsl", "combineFragment.glsl");
	bonfireShader = new Shader("TexturedVertex.glsl", "TexturedFragment.glsl");
	shadowShader = new Shader("shadowVertex.glsl", "shadowFragment.glsl");
	

	// todo add all shaders
	if (!skyboxShader->LoadSuccess() ||
		!terrainShader->LoadSuccess() || !turtleShader->LoadSuccess()) {
		return;
	}
}

void Renderer::LoadMeshes() {
	// load gun man mesh
	turtleMsh = Mesh::LoadFromMeshFile("SCharacter_Turtle.msh");
	turtleAnim = new MeshAnimation("SCharacter_Turtle.anm");
	turtleMat = new MeshMaterial("SCharacter_Turtle.mat");

	for (int i = 0; i < turtleMsh->GetSubMeshCount(); ++i) {
		const MeshMaterialEntry* matEntry = turtleMat->GetMaterialForLayer(i);

		const string* filename = nullptr;
		matEntry->GetEntry("Diffuse", &filename);
		string path = TEXTUREDIR + *filename;
		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		turtleTextures.emplace_back(texID);
	}

	// load tree 1 mesh
	tree1Msh = Mesh::LoadFromMeshFile("tree1.msh");
	GLuint texID = SOIL_load_OGL_texture(TEXTUREDIR"treeMat.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
	treeTextures.emplace_back(texID);

	// load bonfire mesh
	bonfireMsh = Mesh::LoadFromMeshFile("Firewood_B.msh");
	bonfireMat = new MeshMaterial("Firewood_B.mat");

	for (int i = 0; i < bonfireMsh->GetSubMeshCount(); ++i) {
		const MeshMaterialEntry* matEntry = bonfireMat->GetMaterialForLayer(i);

		const string* filename = nullptr;
		matEntry->GetEntry("Diffuse", &filename);
		string path = TEXTUREDIR + *filename;
		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		bonfireTextures.emplace_back(texID);
	}
}

void Renderer::LoadTextures() {
	cubeMap = SOIL_load_OGL_cubemap(
		TEXTUREDIR"day.right.jpg", TEXTUREDIR"day.left.jpg",
		TEXTUREDIR"day.top.jpg", TEXTUREDIR"day.bottom.jpg",
		TEXTUREDIR"day.front.jpg", TEXTUREDIR"day.back.jpg",
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
	if (!cubeMap) {
		return;
	}

	waterNoiseTex = SOIL_load_OGL_texture(TEXTUREDIR"waterNoise.png",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	if (!waterNoiseTex) {
		return;
	}

	grassNoiseTex = SOIL_load_OGL_texture(TEXTUREDIR"noise.png",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	if (!grassNoiseTex) {
		return;
	}

	grassTex = SOIL_load_OGL_texture(TEXTUREDIR"grassTex.png",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	if (!grassTex) {
		return;
	}

	SetTextureRepeating(waterNoiseTex, true);
	SetTextureRepeating(grassNoiseTex, true);
}

void Renderer::AddNodes() {
	// add animals
	animals = new SceneNode();
	turtle = new SceneNode(turtleMsh, Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	turtle->SetTransform(Matrix4::Translation(heightmapSize * Vector3(0.45f, 0.16f, 0.67f))*
		Matrix4::Rotation(70, Vector3(0, 1, 0)));
	turtle->SetModelScale(Vector3(30.0f, 30.0f, 30.0f));
	animals->AddChild(turtle);

	// add nature scenes
	natureScene = new SceneNode();
	tree1 = new SceneNode(tree1Msh, Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	tree1->SetTransform(Matrix4::Translation(heightmapSize * Vector3(0.3f,1.99f, 0.27f)));
	tree1->SetModelScale(Vector3(30.0f, 30.0f, 30.0f));
	natureScene->AddChild(tree1);

	bonfire = new SceneNode(bonfireMsh, Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	bonfire->SetTransform(Matrix4::Translation(heightmapSize * Vector3(0.51f, 1.4f, 0.635f)));
	bonfire->SetModelScale(Vector3(30.0f, 30.0f, 30.0f));
	natureScene->AddChild(bonfire);


	root = new SceneNode();
	root->AddChild(animals);
	root->AddChild(natureScene);
}

void Renderer::BuildNodeLists(SceneNode* from) {

	Vector3 dir = from->GetWorldTransform().GetPositionVector() - camera->GetPosition();
	from->SetCameraDistance(Vector3::Dot(dir, dir));

	if (from->GetColour().w < 1.0f) {
		transparentNodeList.push_back(from);
	}
	else {
		nodeList.push_back(from);
	}


	for (vector<SceneNode*>::const_iterator i = from->GetChildIteratorStart(); i != from->GetChildIteratorEnd(); ++i) {
		BuildNodeLists((*i));
	}
}

void Renderer::SortNodeLists() {
	std::sort(transparentNodeList.rbegin(), transparentNodeList.rend(), SceneNode::CompareByCameraDistance);

	std::sort(nodeList.begin(), nodeList.end(), SceneNode::CompareByCameraDistance);
}

void Renderer::DrawNodes() {
	for (const auto& i : nodeList) {
		DrawNode(i);
	}
	for (const auto& i : transparentNodeList) {
		DrawNode(i);
	}
}

void Renderer::ClearNodeLists() {
	transparentNodeList.clear();
	nodeList.clear();
}

void Renderer::DrawNode(SceneNode* n) {
	if (n->GetMesh()) {
		if (n == turtle) {
			//DrawShadowScene();
			BindShader(turtleShader);
			SetShaderLight(*light);
			glUniform1i(glGetUniformLocation(turtleShader->GetProgram(), "diffuseTex"), 0);
			glUniform1i(glGetUniformLocation(turtleShader->GetProgram(), "shadowTex"), 1);
			UpdateShaderMatrices();
			Matrix4 model = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());

			glUniformMatrix4fv(glGetUniformLocation(turtleShader->GetProgram(), "modelMatrix"), 1, false, model.values);

			vector<Matrix4> frameMatrices;

			const Matrix4* invBindPose = turtleMsh->GetInverseBindPose();
			const Matrix4* frameData = turtleAnim->GetJointData(currentFrame);

			for (unsigned int i = 0; i < turtleMsh->GetJointCount(); ++i) {
				frameMatrices.emplace_back(frameData[i] * invBindPose[i]);
			}

			int j = glGetUniformLocation(turtleShader->GetProgram(), "joints");
			glUniformMatrix4fv(j, frameMatrices.size(), false, (float*)frameMatrices.data());

			for (int i = 0; i < turtleMsh->GetSubMeshCount(); ++i) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, turtleTextures[i]);
				turtleMsh->DrawSubMesh(i);
			}
		}
		else if (n==tree1){
			BindShader(treeShader);
			glUniform1i(glGetUniformLocation(treeShader->GetProgram(), "diffuseTex"), 0);
			UpdateShaderMatrices();
			Matrix4 model = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());

			glUniformMatrix4fv(glGetUniformLocation(treeShader->GetProgram(), "modelMatrix"), 1,
				false, model.values);

			//const Matrix4* invBindPose = tree1Msh->GetInverseBindPose();

			for (int i = 0; i < tree1Msh->GetSubMeshCount(); ++i) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, treeTextures[i]);
				tree1Msh->DrawSubMesh(i);
			}
		}

		else if (n == bonfire) {
			BindShader(bonfireShader);
			glUniform1i(glGetUniformLocation(bonfireShader->GetProgram(), "diffuseTex"), 0);
			UpdateShaderMatrices();
			Matrix4 model = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());

			glUniformMatrix4fv(glGetUniformLocation(bonfireShader->GetProgram(), "modelMatrix"), 1,
				false, model.values);

			for (int i = 0; i < bonfireMsh->GetSubMeshCount(); ++i) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, bonfireTextures[i]);
				bonfireMsh->DrawSubMesh(i);
			}
		}


	}
}

void Renderer::DrawSkybox() {
	glDepthMask(GL_FALSE);

	BindShader(skyboxShader);
	UpdateShaderMatrices();

	skyBox->Draw();

	glDepthMask(GL_TRUE);
}

void Renderer::DrawLowPolyTerrain() {
	
	// HeightMap class has been modified a lot!

	glEnable(GL_DEPTH_TEST);
	BindShader(terrainShader);
	SetShaderLight(*light);
	glUniform3fv(glGetUniformLocation(terrainShader->GetProgram(),
		"cameraPos"), 1, (float*)&camera->GetPosition());

	modelMatrix.ToIdentity();

	UpdateShaderMatrices();

	heightMap->Draw();
}

void Renderer::DrawGrass() {
	glEnable(GL_DEPTH_TEST);
	BindShader(grassShader);
	SetShaderLight(*light);
	glUniform3fv(glGetUniformLocation(grassShader->GetProgram(),
		"cameraPos"), 1, (float*)&camera->GetPosition());
	glUniform1f(glGetUniformLocation(grassShader->GetProgram(), "time"),
		grassWaveTime);

	glUniform1i(glGetUniformLocation(grassShader->GetProgram(), "grassNoiseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, grassNoiseTex);

	glUniform1i(glGetUniformLocation(grassShader->GetProgram(), "grassTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, grassTex);


	modelMatrix.ToIdentity();

	UpdateShaderMatrices();

	heightMap->Draw();
}

void Renderer::DrawLowPolyWater() {
	Shader* curShader;
	if (waterLineSwitch) {
		curShader = waterLineShader;
	}
	else {
		curShader = waterShader;
	}
	BindShader(curShader);
	SetShaderLight(*light);
	glUniform3fv(glGetUniformLocation(curShader->GetProgram(), "cameraPos"),
		1, (float*)&camera->GetPosition());
	glUniform1f(glGetUniformLocation(curShader->GetProgram(), "time"),
		waveTime);
	glUniform3fv(glGetUniformLocation(curShader->GetProgram(), "mapSize"),
		1, (float*)&heightmapSize);


	glUniform1i(glGetUniformLocation(curShader->GetProgram(), "waterNoiseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, waterNoiseTex);
	


	Vector3 hSize = heightMap->GetHeightMapSize();
	modelMatrix = Matrix4::Translation(Vector3(hSize.x * (-2.0),hSize.y*0.6f,hSize.z*(-2.0)));
	UpdateShaderMatrices();
	water->Draw();
}

void Renderer::SetCamPosition(int camMode) {
	switch (camMode)
	{
	case 1:
		camera->SetPosition(heightmapSize * Vector3(0.5f, 4.0f, 2.0f));
		break;
	case 2:
		camera->SetPosition(heightmapSize * Vector3(0.52f, 2.4f, 0.175f));
		break;
	case 3:
		camera->SetPosition(heightmapSize * Vector3(0.48f, 2.0f, 0.73f));//turtle view
		break;
	case 4:
		camera->SetPosition(heightmapSize * Vector3(0.54f, 2.0f, 0.73f));
		break;
	default:
		break;
	}
}

//void Renderer::SeperateDraw() {
//	glViewport(width / 2, 0.0, width / 2, height);
//	
//	curCam = camera;
//	viewMatrix = curCam->BuildViewMatrix();
//	NormalDraw();
//	// delete post processing items
//	glDeleteTextures(2, bufferColourTex);
//	glDeleteTextures(1, &bufferDepthTex);
//	glDeleteFramebuffers(1, &bufferFBO);
//	glDeleteFramebuffers(1, &processFBO);
//
//	// delete deffered rendering items
//	glDeleteTextures(1, &dfBufferColourTex);
//	glDeleteTextures(1, &dfBufferNormalTex);
//	glDeleteTextures(1, &dfBufferDepthTex);
//	glDeleteTextures(1, &lightDiffuseTex);
//	glDeleteTextures(1, &lightSpecularTex);
//	glDeleteFramebuffers(1, &gBufferFBO);
//	glDeleteFramebuffers(1, &pointLightFBO);
//
//	// delete shadow mapping items
//	glDeleteTextures(1, &shadowTex);
//	glDeleteFramebuffers(1, &shadowFBO);
//
//	
//	glViewport(0.0, 0.0, width / 2, height);
//	InitDefferedRender();
//	InitPostProcess();
//	//glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
//	curCam = subCam;
//	viewMatrix = curCam->BuildViewMatrix();
//	NormalDraw();
//
//	glViewport(0.0, 0.0, width, height);
//}
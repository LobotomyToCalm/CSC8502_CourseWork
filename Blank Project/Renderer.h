#pragma once
#include "../nclgl/OGLRenderer.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/Frustum.h"
#include <Vector>;

// shadow mapping factor
#define SHADOWSIZE 2048

class Camera;
class Shader;
class HeightMap;
class SceneNode;
class Light;
class MeshAnimation;
class MeshMaterial;
class WaterMesh;

enum DrawModes {
	NORMAL_DRAW,		// key 7
	BLUR_DRAW,			// key 8
	DEFFERED_DRAW	,	// key 9
	SEPERATE_DRAW,	// key 0
};
// post processing factor
const int POST_PASSES = 10;
// deffered rendering factor
const int LIGHT_NUM = 32;


class Renderer :public OGLRenderer {
public:
	Renderer(Window& parent);
	~Renderer(void);
	void RenderScene() override;
	void UpdateScene(float dt) override;
	void SetCamPosition(int mode);

	bool	waterLineSwitch;
	bool	camForwardSwitch;
	bool	camRotateSwitch;
	float	camSpeed;
	float	camOffset;
	int 	camMode;
	int		drawMode;

protected:
	Light* light;
	Light* fireLight;
	Camera* camera;
	Camera* subCam;
	Camera* curCam;

	/*void SeperateDraw();
	void GenerateRain();
	void DrawRain();*/

	// Post processing 
	void PreDrawScene();
	void DrawPostProcess();
	void DrawScene();
	void InitPostProcess();
	
	// Deffered rendering
	void InitDefferedRender();
	void FillBuffers();
	void DrawPointLights();
	void CombineBuffers();
	void GenerateScreenTexture(GLuint& into, bool depth = false);
	void DefferedDraw();

	// Shadow mapping 
	void InitShadowRender();
	void DrawShadowScene();
	void DrawMainScene();

	// Draw environment functions
	void NormalDraw();
	void DrawLowPolyTerrain();
	void DrawGrass();
	void DrawLowPolyWater();
	void DrawSkybox();
	
	// Load Data functions
	void LoadShaders();
	void LoadMeshes();
	void LoadTextures();
	void AddNodes();

	// Scene management functions
	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();
	void ClearNodeLists();
	void DrawNodes();
	void DrawNode(SceneNode* n);

	// Shaders
	Shader* lightShader;
	Shader* skyboxShader;
	Shader* turtleShader;
	Shader* bonfireShader;
	Shader* terrainShader;
	Shader* waterShader;
	Shader* waterLineShader;
	Shader* grassShader;
	Shader* toLineShader;
	Shader* treeShader;
	Shader* blurShader;
	Shader* postShader;
	Shader* deferShader;
	Shader* pointLightShader;
	Shader* combineShader;
	Shader* shadowShader;
	Shader* terrainShadowShader;
	
	// environment meshes
	HeightMap*	heightMap;
	Vector3		heightmapSize;
	Mesh*			skyBox;
	WaterMesh*	water;
	//Mesh*			rain;
	
		
	// object meshes
	Mesh*				turtleMsh;
	MeshAnimation*	turtleAnim;
	MeshMaterial*	turtleMat;
	Mesh*				tree1Msh;
	Mesh*				bonfireMsh;
	MeshMaterial*	bonfireMat;

	// environment IDs
	GLuint cubeMap;

	// For post processing blur
	GLuint bufferFBO;				
	GLuint processFBO;
	GLuint bufferColourTex[2];
	GLuint bufferDepthTex;
	Mesh* postScene;

	// For deffered rendering
	GLuint gBufferFBO;				
	GLuint dfBufferColourTex;
	GLuint dfBufferNormalTex;
	GLuint dfBufferDepthTex;
	GLuint pointLightFBO;
	GLuint lightDiffuseTex;
	GLuint lightSpecularTex;
	Light* pointLights;
	Mesh* lightSphere;
	Mesh* defferedScene;

	// For shadow mapping
	GLuint					shadowTex;
	GLuint					shadowFBO;
	vector<Mesh*>		shadowMeshes;

	// textures
	GLuint				waterNoiseTex;
	GLuint				grassNoiseTex;
	GLuint				grassTex;
	vector<GLuint>	turtleTextures;
	vector<GLuint>	treeTextures;
	vector<GLuint>	bonfireTextures;

	// scene nodes
	SceneNode*				root;
	SceneNode*				animals;
	SceneNode*				natureScene;
	SceneNode*				turtle;
	SceneNode*				tree1;
	SceneNode*				bonfire;
	vector<SceneNode*>	transparentNodeList;
	vector<SceneNode*>	nodeList;

	// environment factors
	int		currentFrame;
	float	frameTime;
	float	grassWaveSpeed;
	float	grassWaveTime;
	float	waveSpeed;
	float	waveTime;

};

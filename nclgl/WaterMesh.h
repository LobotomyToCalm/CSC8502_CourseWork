#pragma once

#include <string>
#include "Mesh.h"

class WaterMesh :public Mesh {
public:
	WaterMesh(Vector3 heightmapSize);
	~WaterMesh(void);
protected:
	//Vector4* triCorners;
	
};

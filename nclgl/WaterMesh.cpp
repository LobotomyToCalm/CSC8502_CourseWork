#include "WaterMesh.h"
WaterMesh::WaterMesh(Vector3 heightmapSize) {
	Vector3 vertexScale = Vector3(10.0f, 1.0f, 10.0f); // same as heightmap
	int sizeScale = 5;
	numVertices = sizeScale * sizeScale * 6 * (heightmapSize.x / vertexScale.x) *
		(heightmapSize.z / vertexScale.z);						// how large the water surface is
	vertices = new Vector3[numVertices];
	colours = new Vector4[numVertices];
	tangents = new Vector4[numVertices];
	textureCoords = new Vector2[numVertices];

	float height = sizeScale * (heightmapSize.x / vertexScale.x);
	float width = sizeScale * (heightmapSize.z / vertexScale.z);
	float surfaceHeight = 0.0f;

	int i = 0;
	for (int z = 0; z < height; ++z) {
		for (int x = 0; x < width; ++x) {
			Vector3 a = Vector3(z * vertexScale.z, surfaceHeight, x * vertexScale.x);
			Vector3 b = Vector3(z * vertexScale.z, surfaceHeight, (x+1) * vertexScale.x);
			Vector3 c = Vector3((z+1) * vertexScale.z, surfaceHeight, (x+1) * vertexScale.x);
			Vector3 d = Vector3((z+1) * vertexScale.z, surfaceHeight, x * vertexScale.x);

			vertices[i++] = a;
			vertices[i++] = c;
			vertices[i++] = b;

			vertices[i++] = c;
			vertices[i++] = a;
			vertices[i++] = d;
		}
	}

	// Set Colour;
	for (int i = 0; i < numVertices; i++) {
		colours[i] = Vector4 (0.64f, 0.85f, 0.87f, 0.70f);
		
	}

	// Culculate Tangents
	GenerateLowPolyCorners();
	// Culculate Normal
	GenerateNormals();
	// Culculate TexCoord (modified to put it in vertex shader)
	//GenerateTexCoord(heightmapSize);
	// send data to buffer
	BufferData();
}
WaterMesh::~WaterMesh(void) {
	//delete[] triCorners;
}

//void WaterMesh::GenerateTexCoord(Vector3 heightmapSize) {
//	int sizeScale = 5;
//	Vector3 vertexScale = Vector3(10.0f, 1.0f, 10.0f); // same as heightmap
//	float height = sizeScale * (heightmapSize.x );
//	float width = sizeScale * (heightmapSize.z );
//
//	for (int i = 0; i < numVertices; i++) {
//		textureCoords[i] = Vector2(vertices[i].x / height, vertices[i].z / width);
//	}
//}
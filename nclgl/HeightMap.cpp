#include "HeightMap.h"
#include <iostream>

HeightMap::HeightMap(const std::string& name) {
	int iWidth, iHeight, iChans;
	unsigned char* data = SOIL_load_image(name.c_str(), &iWidth, &iHeight, &iChans, 1);
	if (!data) {
		std::cout << "Heightmap can't load file!\n";
		return;
	}
	numVertices = iWidth * iHeight;
	numIndices = (iWidth - 1) * (iHeight - 1) * 6;
	vertices = new Vector3[numVertices];
	textureCoords = new Vector2[numIndices];
	colours = new Vector4[numIndices];

	Vector3 vertexScale = Vector3(10.0f, 1.0f, 10.0f);
	Vector2 textureScale = Vector2(1 / 10.0f, 1 / 10.0f);

	for (int z = 0; z < iHeight; ++z) {
		for (int x = 0; x < iWidth; ++x) {
			int offset = (z * iWidth) + x;
			vertices[offset] = Vector3(x, data[offset], z) * vertexScale;
			
			// todo mod to indices
		}
	}
	SOIL_free_image_data(data);

	// Low Poly style terrain needs separate vertices
	int i = 0;
	Vector3* finalVertices;
	finalVertices = new Vector3[numIndices];

	for (int z = 0; z < iHeight - 1; ++z) {
		for (int x = 0; x < iWidth - 1; ++x) {
			int a = (z * (iWidth)) + x;
			int b = (z * (iWidth)) + (x + 1);
			int c = ((z + 1) * (iWidth)) + (x + 1);
			int d = ((z + 1) * (iWidth)) + x;

			finalVertices[i++] = vertices[a];
			finalVertices[i++] = vertices[c];
			finalVertices[i++] = vertices[b];

			finalVertices[i++] = vertices[c];
			finalVertices[i++] = vertices[a];
			finalVertices[i++] = vertices[d];
		}
	}
	
	delete[]vertices;
	vertices = finalVertices;
	numVertices = numIndices;
	numIndices = 0;

	float TranspHeight = 13.0f;
	float SandHeight = 50.0f;
	float SandGrassHeight = 68.0f;
	float GrassRockHeight = 126.0f;
	float GrassHeight = 150.0f;
	float RockBlendHeight = 175.0f;
	float RockHeight = 210.0f;

	// Set texCoords
	for (int i = 0; i < numVertices; ++i) {
		textureCoords[i] = Vector2(vertices[i].x,vertices[i].y) * textureScale;
	}

	// Set color
	for (int i = 0; i < numVertices; ++i) {

		// Transparent?
		if (vertices[i].y < TranspHeight) {
			colours[i] = Vector4(1.0f, 1.0f, 1.0f, 0.0f);
		}
		// Sand
		else if (vertices[i].y < SandHeight)
		{
			colours[i] = Vector4(0.88f, 0.80f, 0.47f, 1.0f);

		}
		// Grass
		else if (vertices[i].y < GrassHeight)
		{
			if (vertices[i].y < SandGrassHeight)
			{
				float diffA = (vertices[i].y - SandHeight) / (SandGrassHeight - SandHeight);
				float diffB = (SandGrassHeight - vertices[i].y) / (SandGrassHeight - SandHeight);
				colours[i] = Vector4(0.88f * diffB + 0.44f * diffA,
					0.80f * diffB + 0.71f * diffA,
					0.47f * diffB + 0.34f * diffA, 1.0f);
			}
			else if (vertices[i].y >GrassRockHeight) {
				float diffA = (vertices[i].y - GrassRockHeight) / (GrassHeight - GrassRockHeight);
				float diffB = (GrassHeight - vertices[i].y) / (GrassHeight - GrassRockHeight);
				colours[i] = Vector4(0.49f * diffA + 0.44f * diffB,
					0.49f * diffA + 0.71f * diffB,
					0.49f * diffA + 0.34f * diffB, 1.0f);
			}
			else {
				colours[i] = Vector4(0.44f, 0.71f, 0.34f, 1.0f);
			}
		}
		// Rock
		else if (vertices[i].y < RockHeight)
		{
			if (vertices[i].y > RockBlendHeight) {
				float diffA = (vertices[i].y - RockBlendHeight) / (RockHeight - RockBlendHeight);
				float diffB = (RockHeight - vertices[i].y) / (RockHeight - RockBlendHeight);
				colours[i] = Vector4(0.49f * diffB + 0.74f * diffA,
					0.49f * diffB + 0.74f * diffA,
					0.49f * diffB + 0.74f * diffA, 1.0f);
			}
			else{
				colours[i] = Vector4(0.49f, 0.49f, 0.49f, 1.0f);
			}
		}
		// Top
		else {
			colours[i] = Vector4(0.74f, 0.74f, 0.74f, 1.0f);
		}
	}
	// Culculate Normal
	GenerateNormals();
	// Culculate Tangents
	GenerateTangents();
	// send data to buffer
	BufferData();

	heightmapSize.x = vertexScale.x * (iWidth - 1);
	heightmapSize.y = vertexScale.y * 50.0f;
	heightmapSize.z = vertexScale.z * (iHeight - 1);

	finalVertices = nullptr;

}
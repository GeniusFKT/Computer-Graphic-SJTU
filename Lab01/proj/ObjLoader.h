#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "Camera.h"
#include "Shader.h"

#include <string>
#include <vector>
using namespace std;

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
};

struct Index
{
	int i1;
	int i2;
	int i3;
};

class ObjLoader
{
public:
	ObjLoader(string filename);
	void Draw();
private:
	vector<Vertex>vSets;
	vector<Index>fSets;
	void setupMesh();
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;
};


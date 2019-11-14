#include "ObjLoader.h"

ObjLoader::ObjLoader(string filename)
{
	std::ifstream file(filename);
	std::string line;
	while (getline(file, line))
	{
		if (line[0] == 'v')
		{
			Vertex Point;
			std::istringstream s(line.substr(2));
			s >> Point.Position.x >> Point.Position.y >> Point.Position.z;
			vSets.push_back(Point);
		}
		else if (line[0] == 'f')
		{
			Index vIndex;
			std::istringstream vtns(line.substr(2));
			vtns >> vIndex.i1 >> vIndex.i2 >> vIndex.i3;
			vIndex.i1 -= 1;
			vIndex.i2 -= 1;
			vIndex.i3 -= 1;
			fSets.push_back(vIndex);
		}
	}

	for (int i = 0; i < vSets.size(); ++i)
	{
		vSets[i].Normal = glm::vec3(0, 0, 0);
	}

	for (int i = 0; i < fSets.size(); ++i)
	{
		glm::vec3 v1(vSets[fSets[i].i2].Position - vSets[fSets[i].i1].Position);
		glm::vec3 v2(vSets[fSets[i].i3].Position - vSets[fSets[i].i1].Position);
		glm::vec3 normal = glm::normalize(glm::cross(v1, v2));
		vSets[fSets[i].i1].Normal += normal;
		vSets[fSets[i].i2].Normal += normal;
		vSets[fSets[i].i3].Normal += normal;
	}

	for (int i = 0; i < vSets.size(); ++i)
	{
		vSets[i].Normal = glm::normalize(vSets[i].Normal);
	}

	file.close();

	setupMesh();
}

void ObjLoader::Draw()
{
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, fSets.size() * 3, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void ObjLoader::setupMesh()
{
	// create buffers/arrays
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// A great thing about structs is that their memory layout is sequential for all its items.
	// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
	// again translates to 3/2 floats which translates to a byte array.
	glBufferData(GL_ARRAY_BUFFER, vSets.size() * sizeof(Vertex), &vSets[0], GL_STATIC_DRAW);
	// set the vertex attribute pointers
	// vertex Positions
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, fSets.size() * sizeof(Index), &fSets[0], GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <shader.h>
#include <camera.h>
#include <math.h>

using namespace std;

// view port call back function
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void read_data(string filename);


vector <float> vertices;
vector <unsigned int> faces;
vector <vector <float>> vertices_array;
vector <vector <unsigned int>> faces_array;

// float vertices_array[75000];
// unsigned int faces_array[150000];

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

// mouse
bool firstMouse = true;
float lastX = WIDTH / 2.0;
float lastY = HEIGHT / 2.0;

// lighting
glm::vec3 lightPos(1.0f, 1.0f, 1.0f);

glm::mat4 model = glm::mat4(1.0f);

int main()
{
	glfwInit();		// Initialization
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);	// GL version constraint
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);	// use core profile
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// create a window
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Dragon", NULL, NULL);
	if (window == NULL)
	{
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// glad initialization
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Failed to initialize GLAD" << endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);
	// glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// glDepthMask(GL_FALSE);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	Shader ourShader("vs.txt", "fs.txt");

	string filename = "D:\\SJTU\\Semester5\\CG\\Lab01\\dragon.obj";
	read_data(filename);

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(unsigned int), &faces[0], GL_STATIC_DRAW);


	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	ourShader.use();
	
	// render loop
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);		// DETECT ESC

		glClearColor(0.2f, 0.3f, 0.3f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// glClear(GL_COLOR_BUFFER_BIT);
		// be sure to activate shader when setting uniforms/drawing objects
		ourShader.use();

		ourShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
		ourShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		ourShader.setVec3("lightPos", lightPos);
		ourShader.setVec3("viewPos", camera.Position);

		// camera/view transformation
		glm::mat4 view = camera.GetViewMatrix();
		ourShader.setMat4("view", view);

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
		ourShader.setMat4("projection", projection);

		ourShader.setMat4("model", model);

		// render container
		
		glBindVertexArray(VAO);

		glDrawElements(GL_TRIANGLES, faces.size(), GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);

		glfwSwapBuffers(window);	// ������ɫ����
		glfwPollEvents();			// ��鴥���¼�
	}

	glfwTerminate();		//	terminate

	return 0;
}



void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	model = glm::rotate(model, glm::radians(0.2f * yoffset), glm::vec3(1, 0, 0));
	model = glm::rotate(model, glm::radians(0.2f * xoffset), glm::vec3(0, 1, 0));

	// camera.ProcessMouseMovement(xoffset, yoffset, true);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

void read_data(string filename)
{
	fstream in;
	in.open(filename, ios::in);

	if (!in.is_open())
		cout << "Damn! I can not open this file" << endl;

	else
	{
		while (!in.eof())
		{
			string a;
			getline(in, a);

			if (a[0] == '#')
				continue;

			if (a[0] == 'v')
			{
				string tmp = "";
				vector <float> vertice;

				for (int i = 2; i < a.length(); ++i)
				{
					if (a[i] != ' ')
					{
						tmp += a[i];
						if (i == (a.length() - 1))
							vertice.push_back(atof(tmp.c_str()));
					}

					else
					{
						vertice.push_back(atof(tmp.c_str()));
						tmp = "";
					}
				}

				vertices_array.push_back(vertice);
			}

			if (a[0] == 'f')
			{
				string tmp = "";
				vector <unsigned int> face;

				for (int i = 2; i < a.length(); ++i)
				{
					if (a[i] != ' ')
					{
						tmp += a[i];
						if (i == (a.length() - 1))
							face.push_back(atoi(tmp.c_str()) - 1);
					}
					else
					{
						face.push_back(atoi(tmp.c_str()) - 1);
						tmp = "";
					}
				}

				faces_array.push_back(face);
			}
		}
	}

	// calculate normal vector
	for (int i = 0; i < vertices_array.size(); ++i)
		for (int j = 0; j < 3; ++j)
			vertices_array[i].push_back(0.0);

	for (int i = 0; i < faces_array.size(); ++i)
	{
		glm::vec3 v1(vertices_array[faces_array[i][1]][0] - vertices_array[faces_array[i][0]][0],
					 vertices_array[faces_array[i][1]][1] - vertices_array[faces_array[i][0]][1],
					 vertices_array[faces_array[i][1]][2] - vertices_array[faces_array[i][0]][2]);
		glm::vec3 v2(vertices_array[faces_array[i][2]][0] - vertices_array[faces_array[i][0]][0],
					 vertices_array[faces_array[i][2]][1] - vertices_array[faces_array[i][0]][1],
					 vertices_array[faces_array[i][2]][2] - vertices_array[faces_array[i][0]][2]);

		glm::vec3 n = glm::normalize(glm::cross(v1, v2));
		vertices_array[faces_array[i][0]][3] += n.x;
		vertices_array[faces_array[i][0]][4] += n.y;
		vertices_array[faces_array[i][0]][5] += n.z;
		vertices_array[faces_array[i][1]][3] += n.x;
		vertices_array[faces_array[i][1]][4] += n.y;
		vertices_array[faces_array[i][1]][5] += n.z;
		vertices_array[faces_array[i][2]][3] += n.x;
		vertices_array[faces_array[i][2]][4] += n.y;
		vertices_array[faces_array[i][2]][5] += n.z;
	}

	for (int i = 0; i < vertices_array.size(); ++i)
	{
		float length = sqrt(pow(vertices_array[i][3], 2) + pow(vertices_array[i][4], 2) + pow(vertices_array[i][5], 2));
		vertices_array[i][3] /= length;
		vertices_array[i][4] /= length;
		vertices_array[i][5] /= length;
	}

	for (int i = 0; i < vertices_array.size(); ++i)
		for (int j = 0; j < vertices_array[0].size(); ++j)
			vertices.push_back(vertices_array[i][j]);

	for (int i = 0; i < faces_array.size(); ++i)
		for (int j = 0; j < faces_array[0].size(); ++j)
			faces.push_back(faces_array[i][j]);
}

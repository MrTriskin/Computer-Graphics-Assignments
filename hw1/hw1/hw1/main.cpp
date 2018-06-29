#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_s.h"
#include "camera.h"

#include <iostream>
  

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);


int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 16);  //anti aliasing


	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "hw1", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);  //enable depth test
	glEnable(GL_MULTISAMPLE); //enable anti aliasing

	// build and compile our shader program
	// ------------------------------------
	Shader lightingShader("lightingShader.vs", "lightingShader.fs"); // you can name your shader files however you like
	Shader lightSource("lightSource.vs", "lightSource.fs");  // shader for light source
														
															 
	float pi = 3.1415926;
	float sphere[23328];	// 18 * 26 *2 * 3 * 6 float
	float step = pi / 18;
	for (int h = 0; h < 36; h++)
	{
		for (int v = 0; v < 18; v++)
		{
			sphere[h * 648 + v*36] = sin(v*step)*sin(h*step);
			sphere[h * 648 + v * 36 + 1] = cos(v*step);
			sphere[h * 648 + v*36 + 2] = sin(v*step)*cos(h*step);
			sphere[h * 648 + v * 36 + 3] = sin(v *step)*sin(h *step);
			sphere[h * 648 + v * 36 + 4] = cos(v *step);
			sphere[h * 648 + v * 36 + 5] = sin(v *step)*cos(h *step);
			

			sphere[h * 648 + v*36 + 6] = sin(v*step)*sin((h + 1)*step);
			sphere[h * 648 + v*36 + 8] = sin(v*step)*cos((h + 1)*step);
			sphere[h * 648 + v*36 + 7] = cos(v*step);
			sphere[h * 648 + v*36 + 9] = sin(v*step)*sin((h + 1)*step);
			sphere[h * 648 + v*36 + 11] = sin(v*step)*cos((h + 1)*step);
			sphere[h * 648 + v*36 + 10] = cos(v*step);

			sphere[h * 648 + v*36 + 12] = sin((v + 1)*step)*sin(h*step);
			sphere[h * 648 + v*36 + 14] = sin((v + 1)*step)*cos(h*step);
			sphere[h * 648 + v*36 + 13] = cos((v + 1) *step);
			sphere[h * 648 + v*36 + 15] = sin((v + 1) *step)*sin(h*step);
			sphere[h * 648 + v*36 + 17] = sin((v + 1) *step)*cos(h*step);
			sphere[h * 648 + v*36 + 16] = cos((v + 1) *step);
      
			sphere[h * 648 + v*36 + 18] = sin(v*step)*sin(h*step);
			sphere[h * 648 + v*36 + 20] = sin(v*step)*cos(h*step);
			sphere[h * 648 + v*36 + 19] = cos(v*step);
			sphere[h * 648 + v*36 + 21] = sin(v*step)*sin(h*step);
			sphere[h * 648 + v*36 + 23] = sin(v*step)*cos(h*step);
			sphere[h * 648 + v*36 + 22] = cos(v*step);

			sphere[h * 648 + v*36 + 24] = sin((v + 1)*step)*sin((h - 1)*step);
			sphere[h * 648 + v*36 + 26] = sin((v + 1) *step)*cos((h - 1)*step);
			sphere[h * 648 + v*36 + 25] = cos((v + 1) *step);
			sphere[h * 648 + v*36 + 27] = sin((v + 1) *step)*sin((h - 1)*step);
			sphere[h * 648 + v*36 + 29] = sin((v + 1) *step)*cos((h - 1)*step);
			sphere[h * 648 + v*36 + 28] = cos((v + 1) *step);

			sphere[h * 648 + v * 36 + 30] = sin((v + 1) *step)*sin(h *step);
			sphere[h * 648 + v * 36 + 32] = sin((v + 1) *step)*cos(h *step);
			sphere[h * 648 + v * 36 + 31] = cos((v + 1) *step);
			sphere[h * 648 + v * 36 + 33] = sin((v + 1) *step)*sin(h *step);
			sphere[h * 648 + v * 36 + 35] = sin((v + 1) *step)*cos(h *step);
			sphere[h * 648 + v * 36 + 34] = cos((v + 1) *step);
		}
	}
	float vertices[23328 + 288];
	float points[] = {
		//cube
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,

		// tetrahedron
		-0.5f, 0.5f, -0.5f, -1.0f, -1.0f, -1.0f,
		-0.5f, -0.5f, 0.5f, -1.0f, -1.0f, -1.0f,
		0.5f, -0.5f, -0.5f, -1.0f, -1.0f, -1.0f,

		-0.5f, 0.5f, -0.5f, -1.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, -1.0f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.5f, -1.0f, 1.0f, 1.0f,

		-0.5f, 0.5f, -0.5f, 1.0f, 1.0f, -1.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f, -1.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f, -1.0f,

		-0.5f, -0.5f, 0.5f, 1.0f, -1.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 1.0f, -1.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 1.0f, -1.0f, 1.0f,


	};
	for (size_t i = 288; i <= 23328+288; i++)
	{
		vertices[i] = sphere[i - 288];
	}
	for (size_t i = 0; i < 288; i++)
	{
		vertices[i] = points[i];
	}
	for (size_t i = 288; i < 388; i++)
	{
		std::cout << vertices[i];
	}
	// first, configure the cube's VAO (and VBO)
	unsigned int VBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// note that we update the lamp's position attribute's stride to reflect the updated buffer data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	// glBindVertexArray(0);


	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // GL_DEPTH_BUFFER_BIT for depth test

		// be sure to activate shader when setting uniforms/drawing objects
		lightingShader.use();
		lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
		lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		lightingShader.setVec3("lightPos", lightPos);
		lightingShader.setVec3("viewPos", camera.Position);

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);

		// world transformation
		glm::mat4 model;
		//model = glm::translate(model, glm::vec3(1.0f, 1.0f, 1.0f));
		lightingShader.setMat4("model", model);
		
		// render the cube
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glm::mat4 tetrahedron;
		tetrahedron = glm::rotate(tetrahedron, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
		tetrahedron = glm::translate(tetrahedron, glm::vec3(2.0f, 2.0f, 2.0f));
		tetrahedron = glm::rotate(tetrahedron, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
		lightingShader.setMat4("model", tetrahedron);
		
		glDrawArrays(GL_TRIANGLES, 36, 12 );
		glm::mat4 sphere;
		sphere = glm::translate(sphere, glm::vec3(-1.5f, -1.5f, -1.2f));
		lightingShader.setMat4("model", sphere);
		glDrawArrays(GL_TRIANGLES, 48, 3888);

		// also draw the lamp object
		lightSource.use();
		lightSource.setMat4("projection", projection);
		lightSource.setMat4("view", view);
		model = glm::mat4();
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
		lightSource.setMat4("model", model);

		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);
	
	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
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

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}


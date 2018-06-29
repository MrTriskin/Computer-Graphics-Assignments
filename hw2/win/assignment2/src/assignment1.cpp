#include "../bezier/mlbezier.h"
#include "../inc/shader_m.h"
#include "../3rdparty/stb_image.h"
#include "../inc/camera.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

bool dragFlag = false;

// timing
float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间
mlBezier mlbezier;
//glm::vec3* testp = NULL;

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// lighting
glm::vec3 lightPos(1.2f, 5.0f, 1.0f);

void drawControlPoint(mlBezier &mlbezier)
{
	glBegin(GL_QUADS);

	for (int i = 0; i < (int)mlbezier.indicesofControlpoints.size() / 4; i++)
	{
		glColor3f(0, 1, 0);
		//std::cout << "rec "<<(int)mlbezier.indicesofControlpoints.size() / 4 << std::endl;
		glVertex3f
		(
			mlbezier.controlPoints[mlbezier.indicesofControlpoints[4 * i]].x,
			mlbezier.controlPoints[mlbezier.indicesofControlpoints[4 * i]].y,
			mlbezier.controlPoints[mlbezier.indicesofControlpoints[4 * i]].z
		);
		glVertex3f
		(
			mlbezier.controlPoints[mlbezier.indicesofControlpoints[4 * i + 1]].x,
			mlbezier.controlPoints[mlbezier.indicesofControlpoints[4 * i + 1]].y,
			mlbezier.controlPoints[mlbezier.indicesofControlpoints[4 * i + 1]].z
		);
		glVertex3f
		(
			mlbezier.controlPoints[mlbezier.indicesofControlpoints[4 * i + 2]].x,
			mlbezier.controlPoints[mlbezier.indicesofControlpoints[4 * i + 2]].y,
			mlbezier.controlPoints[mlbezier.indicesofControlpoints[4 * i + 2]].z
		);
		glVertex3f
		(
			mlbezier.controlPoints[mlbezier.indicesofControlpoints[4 * i + 3]].x,
			mlbezier.controlPoints[mlbezier.indicesofControlpoints[4 * i + 3]].y,
			mlbezier.controlPoints[mlbezier.indicesofControlpoints[4 * i + 3]].z
		);
	}
	glEnd();
}

// decasteljau function
glm::vec3 decasteljau(glm::vec3* ctrP, float t, int numofcp) {
	glm::vec3 *cp = new glm::vec3[numofcp];
	for (size_t i = 0; i < numofcp; i++)
	{
		cp[i] = ctrP[i];
	}
	for (size_t j = 0; j < numofcp - 1; j++)
	{
		for (size_t i = 0; i < numofcp - j - 1; i++)
		{
			cp[i] = (1 - t)*cp[i] + t * cp[i + 1];
		}
	}
	return(cp[0]);
}
// tan
glm::vec3 tangent(glm::vec3* ctrP, float t, int numofcp) {
	glm::vec3 *cp = new glm::vec3[numofcp];
	for (size_t i = 0; i < numofcp; i++)
	{
		cp[i] = ctrP[i];
		//std::cout << cp[i].x << " " << cp[i].y << " " << cp[i].z << std::endl;
	}
	for (size_t j = 0; j < numofcp - 1; j++)
	{
		/*tmp = new glm::vec3[numofcp - j - 1];*/
		if (j == numofcp - 2)
		{
			return( cp[1] - cp[0] );
		}
		for (size_t i = 0; i < numofcp - j - 1; i++)
		{
			cp[i] = (1 - t)*cp[i] + t * cp[i + 1];
			/*std::cout << cp[i].x << " " << cp[i].y << " " << cp[i].z << std::endl;
			std::cout << ctrP[i].x << " " << ctrP[i].y << " " << ctrP[i].z << std::endl;*/
		}
	}
}

// curve tan
glm::vec3* evalTanCurve(glm::vec3* p, int uv, int c) {
	// output uv points
	// num of c control points
	glm::vec3 *ps = new glm::vec3[uv];
	for (size_t i = 0; i < uv; i++)
	{
		ps[i] = tangent(p, i / (float)(uv - 1), c);
	};


	return(ps);
}

// bazier curve
glm::vec3* evalBezierCurve(glm::vec3* p, int uv, int c) {
	// output uv points
	// num of c control points
	glm::vec3 *ps = new glm::vec3[uv];
	for (size_t i = 0; i < uv; i++)
	{
		ps[i] = decasteljau(p, i / (float)(uv - 1), c);
	};


	return(ps);
}

// bezier surface
glm::vec3* evaluateBezierSurface(glm::vec3* ctrlPoints, int u, int v, int uc, int vc) {
	// u cols of points
	// v rows of points
	// uc cols of control points 
	// vc rows of control points
	glm::vec3* out = new glm::vec3[u*v];
	glm::vec3** pv = new glm::vec3*[u];
	glm::vec3** pu = new glm::vec3*[vc];
	// compute u control points along u direction
	for (int i = 0; i < vc; i++)
	{
		glm::vec3 *ctrp = new glm::vec3[uc];
		for (int j = 0; j < uc; j++)
		{
			ctrp[j] = ctrlPoints[i * uc + j];
		};
		pu[i] = evalBezierCurve(ctrp, u, uc);
	};
	for (size_t k = 0; k < u; k++)
	{
		glm::vec3 *cpv = new glm::vec3[vc];
		for (size_t j = 0; j < vc; j++)
		{
			cpv[j] = pu[j][k];
		};
		pv[k] = evalBezierCurve(cpv, v, vc);
	}
	for (size_t row = 0; row < v; row++)
	{
		for (size_t col = 0; col < u; col++)
		{
			out[u*row + col] = pv[col][row];
		}
	}
	return out;
}

// evaluateNormalU
glm::vec3* evaluateNormalU(glm::vec3* ctrlPoints, int u, int v, int uc, int vc) {
	// u cols of points
	// v rows of points
	// uc cols of control points 
	// vc rows of control points
	glm::vec3* out = new glm::vec3[u*v];
	glm::vec3** pv = new glm::vec3*[uc];
	glm::vec3** pu = new glm::vec3*[v];
	// compute u control points along u direction
	for (int i = 0; i < uc; i++)
	{
		glm::vec3 *ctrp = new glm::vec3[vc];
		for (int j = 0; j < vc; j++)
		{
			ctrp[j] = ctrlPoints[j * uc + i];
		};
		pv[i] = evalBezierCurve(ctrp, v, vc);
	};
	for (size_t k = 0; k < v; k++)
	{
		glm::vec3 *cpv = new glm::vec3[uc];
		for (size_t j = 0; j < uc; j++)
		{
			cpv[j] = pv[j][k];
		};
		pu[k] = evalTanCurve(cpv, u, uc);
	}
	for (size_t row = 0; row < v; row++)
	{
		for (size_t col = 0; col < u; col++)
		{
			out[u*row + col] = pu[row][col];
		}
	}
	return out;
}

// evaluateNormalV
glm::vec3* evaluateNormalV(glm::vec3* ctrlPoints, int u, int v, int uc, int vc) {
	// u cols of points
	// v rows of points
	// uc cols of control points 
	// vc rows of control points
	glm::vec3* out = new glm::vec3[u*v];
	glm::vec3** pv = new glm::vec3*[u];
	glm::vec3** pu = new glm::vec3*[vc];
	// compute u control points along u direction
	for (int i = 0; i < vc; i++)
	{
		glm::vec3 *ctrp = new glm::vec3[uc];
		for (int j = 0; j < uc; j++)
		{
			ctrp[j] = ctrlPoints[i * uc + j];
		};
		pu[i] = evalBezierCurve(ctrp, u, uc);
	};
	for (size_t k = 0; k < u; k++)
	{
		glm::vec3 *cpv = new glm::vec3[vc];
		for (size_t j = 0; j < vc; j++)
		{
			cpv[j] = pu[j][k];
		};
		pv[k] = evalTanCurve(cpv, v, vc);
	}
	for (size_t row = 0; row < v; row++)
	{
		for (size_t col = 0; col < u; col++)
		{
			out[u*row + col] = pv[col][row];
		}
	}
	return out;
}

glm::vec3* evalNorms(glm::vec3* tanu, glm::vec3* tanv, int u, int v) {
	glm::vec3* out = new glm::vec3[u*v];
	for (size_t i = 0; i < u*v; i++)
	{
		/*std::cout << "tanu "<<tanu[i].x << " " << tanu[i].y << " " << tanu[i].z << std::endl;
		std::cout << "tanv "<<tanv[i].x << " " << tanv[i].y << " " << tanv[i].z << std::endl;*/
		out[i] = glm::normalize(glm::cross(tanu[i], tanv[i]));
		//std::cout << out[i].x << " " << out[i].y << " " << out[i].z << std::endl;
	}
	return out;
}

// another decasteljau function
glm::vec3 decasteljau(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4, float t) {
	glm::vec3 p12 = (1 - t) * p1 + t * p2;
	glm::vec3 p23 = (1 - t) * p2 + t * p3;
	glm::vec3 p34 = (1 - t) * p3 + t * p4;

	glm::vec3 p1223 = (1 - t) * p12 + t * p23;
	glm::vec3 p2334 = (1 - t) * p23 + t * p34;

	return((1 - t)*p1223 + t * p2334);
}

void drawBezierSurface(mlBezier &mlbezier, glm::vec3* Points)
{
	glBegin(GL_TRIANGLES);
	//std::cout << mlbezier.indicesofP.size() << std::endl;
	for (int i = 0; i < (int)mlbezier.indicesofP.size() / 3; i++)
	{
		glColor3f(0, 0, 1);
		//std::cout << Points[mlbezier.indicesofP[3*i + 1]].x << std::endl;
		glVertex3f
		(
			Points[mlbezier.indicesofP[3 * i]].x,
			Points[mlbezier.indicesofP[3 * i]].y,
			Points[mlbezier.indicesofP[3 * i]].z
		);
		glVertex3f
		(
			Points[mlbezier.indicesofP[3 * i + 1]].x,
			Points[mlbezier.indicesofP[3 * i + 1]].y,
			Points[mlbezier.indicesofP[3 * i + 1]].z
		);
		glVertex3f
		(
			Points[mlbezier.indicesofP[3 * i + 2]].x,
			Points[mlbezier.indicesofP[3 * i + 2]].y,
			Points[mlbezier.indicesofP[3 * i + 2]].z
		);
	}
	glEnd();
}

void drawNorms(mlBezier &mlbezier, glm::vec3* Norms, glm::vec3* Points)
{
	glBegin(GL_LINES);
	//std::cout << mlbezier.indicesofP.size() << std::endl;
	for (int i = 0; i < (int)mlbezier.indicesofP.size() / 3; i++)
	{
		glColor3f(1, 0, 0);
		//std::cout << Points[mlbezier.indicesofP[3*i + 1]].x << std::endl;
		glVertex3f
		(
			Points[mlbezier.indicesofP[3 * i]].x,
			Points[mlbezier.indicesofP[3 * i]].y,
			Points[mlbezier.indicesofP[3 * i]].z
		);
		glVertex3f
		(
			(Points[mlbezier.indicesofP[3 * i]] + Norms[mlbezier.indicesofP[3 * i]]).x,
			(Points[mlbezier.indicesofP[3 * i]] + Norms[mlbezier.indicesofP[3 * i]]).y,
			(Points[mlbezier.indicesofP[3 * i]] + Norms[mlbezier.indicesofP[3 * i]]).z
		);
	}
	glEnd();
}
float* loadVao(glm::vec3* p, glm::vec3* n, mlBezier &mlbezier, glm::vec2* tc) {
	float* tank = new float[mlbezier.indicesofP.size() * 8];
	for (size_t i = 0; i < (int)mlbezier.indicesofP.size() / 3; i++)
	{	
		std::cout << mlbezier.indicesofP[3 * i] << std::endl;
		tank[i * 24] = p[mlbezier.indicesofP[3 * i]].x;
		tank[i * 24 + 1] = p[mlbezier.indicesofP[3 * i]].y;
		tank[i * 24 + 2] = p[mlbezier.indicesofP[3 * i]].z;
		tank[i * 24 + 3] = n[mlbezier.indicesofP[3 * i]].x;
		tank[i * 24 + 4] = n[mlbezier.indicesofP[3 * i]].y;
		tank[i * 24 + 5] = n[mlbezier.indicesofP[3 * i]].z;
		tank[i * 24 + 6] = tc[mlbezier.indicesofP[3 * i]].x;
		tank[i * 24 + 7] = tc[mlbezier.indicesofP[3 * i]].y;
		tank[i * 24 + 8] = p[mlbezier.indicesofP[3 * i + 1]].x;
		tank[i * 24 + 9] = p[mlbezier.indicesofP[3 * i + 1]].y;
		tank[i * 24 + 10] = p[mlbezier.indicesofP[3 * i + 1]].z;
		tank[i * 24 + 11] = n[mlbezier.indicesofP[3 * i + 1]].x;
		tank[i * 24 + 12] = n[mlbezier.indicesofP[3 * i + 1]].y;
		tank[i * 24 + 13] = n[mlbezier.indicesofP[3 * i + 1]].z;
		tank[i * 24 + 14] = tc[mlbezier.indicesofP[3 * i + 1]].x;
		tank[i * 24 + 15] = tc[mlbezier.indicesofP[3 * i + 1]].y;
		tank[i * 24 + 16] = p[mlbezier.indicesofP[3 * i + 2]].x;
		tank[i * 24 + 17] = p[mlbezier.indicesofP[3 * i + 2]].y;
		tank[i * 24 + 18] = p[mlbezier.indicesofP[3 * i + 2]].z;
		tank[i * 24 + 19] = n[mlbezier.indicesofP[3 * i + 2]].x;
		tank[i * 24 + 20] = n[mlbezier.indicesofP[3 * i + 2]].y;
		tank[i * 24 + 21] = n[mlbezier.indicesofP[3 * i + 2]].z;
		tank[i * 24 + 22] = tc[mlbezier.indicesofP[3 * i + 2]].x;
		tank[i * 24 + 23] = tc[mlbezier.indicesofP[3 * i + 2]].y;
	}
	return tank;
}
void initPMV()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(60, SCR_WIDTH / SCR_HEIGHT, 0.1, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt
	(
		0, 0, -7.5,
		0, 0, 0,
		0, 1, 0
	);
}

void initTexture(unsigned int &texture1)
{
	// texture
	// ---------
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	//stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char *data = stbi_load("./resource/textures/02.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);
}


void AddLight(mlBezier &mlbezier)
{


	

}
int main()
{
	
	mlbezier.u = 14;
	mlbezier.v = 14;
	mlbezier.uc = 5;
	mlbezier.vc = 5;
	glm::vec3* Points = NULL; 
	glm::vec3* TanU = NULL;
	glm::vec3* TanV = NULL;
	glm::vec3* Norms = NULL;
	glm::vec2* tc = new glm::vec2[mlbezier.u*mlbezier.v];
	for (size_t row = 0; row < mlbezier.v; row++)
	{
		for (size_t col = 0; col < mlbezier.u; col++)
		{
			tc[row*mlbezier.u + col].x = (float)col / mlbezier.u;
			tc[row*mlbezier.u + col].y = -(float)row / mlbezier.v;
		};
	};
	//float* vertices = NULL;
	//mlBezier mlbezier;
	mlbezier.divs = 25;
	mlbezier.mlCreateBeizermesh();
	mlbezier.mlTriangularization();

	Points = evaluateBezierSurface(mlbezier.controlPoints, mlbezier.u, mlbezier.v, mlbezier.uc, mlbezier.vc);
	TanU = evaluateNormalU(mlbezier.controlPoints, mlbezier.u, mlbezier.v, mlbezier.uc, mlbezier.vc);
	TanV = evaluateNormalV(mlbezier.controlPoints, mlbezier.u, mlbezier.v, mlbezier.uc, mlbezier.vc);
	Norms = evalNorms(TanU, TanV, mlbezier.u, mlbezier.v);
	// test-----------------------------------
	/*glm::vec3 testp[4]
		=
	{
		{ -2.0, -2.0,  1.0 },
	{ -0.5, -2.0,  0.0 },
	{ 0.5, -2.0, -2.0 },
	{ 2.0, -2.0,  2.0 } };
	glm::vec3 t1 = decasteljau(testp, 0.1, 4);
	glm::vec3 tan = tangent(testp, 0.1, 4);
	std::cout << t1.x << " " << t1.y << " " << t1.z << std::endl;
	std::cout << tan.x << " " << tan.y << " " << tan.z << std::endl;*/
	// -----------------------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow * window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "hw2", NULL, NULL);
	

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
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );


	glewExperimental = GL_TRUE;
	glewInit();
	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);  //enable depth test
	glEnable(GL_MULTISAMPLE); //enable anti aliasing
	// init Shader here :D
	Shader viewCamera("./shader/camera.vs", "./shader/camera.fs");
	Shader lightSource("./shader/lightSource.vs", "./shader/lightSource.fs");
	// load VAO
	float* vertices = NULL;
	vertices = loadVao(Points, Norms, mlbezier, tc);
	float in[13*13*2*3*8];
	for (size_t i = 0; i < 13 * 13 * 2 * 3 * 8; i++)
	{
		in[i] = vertices[i];
	}
	/*float vertices[] = {
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
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};*/
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(in), in, GL_STATIC_DRAW);
	glBindVertexArray(VAO);
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// load and create a texture 
	// -------------------------
	unsigned int texture1;
	initTexture(texture1);
	// tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
	// -------------------------------------------------------------------------------------------
	//viewCamera.use(); // don't forget to activate/use the shader before setting uniforms!
					 // either set it manually like so:
	//viewCamera.setInt("texture1", 0);
	// or set it via the texture class
	//ourShader.setInt("texture2", 1);
	
	
	
	

	initPMV();

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
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);

		glPointSize(5);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		
		//drawControlPoint(mlbezier);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		/*drawBezierSurface(mlbezier, Points);
		drawNorms(mlbezier, Norms, Points);*/
		 //active shader
		viewCamera.use();
		viewCamera.setVec3("objectColor", 0.2f, 0.2f, 0.2f);
		viewCamera.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		viewCamera.setVec3("lightPos", lightPos);
		viewCamera.setVec3("viewPos", camera.Position);

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		viewCamera.setMat4("projection", projection);
		viewCamera.setMat4("view", view);

		

		// -------------------------------------------------------------------------------------------------------------------------------
		// render boxes
		
		// world transformation
		glm::mat4 model;
		viewCamera.setMat4("model", model);
		/*model = glm::translate(model, cubePositions[i]);*/
		/*float angle = 20.0f * i;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		viewCamera.setMat4("model", model);*/
        glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 1014);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		drawControlPoint(mlbezier);
		//----------------------------------------------------------------------------------------------------------------------------------
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
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



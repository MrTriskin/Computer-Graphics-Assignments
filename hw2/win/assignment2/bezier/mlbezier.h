#pragma once
#ifndef _MLBEZIERH_
#define _MLBEZIERH_
#include "../inc/common.h"
#include <vector>
#include <memory>

#define MOUSE_SELECT_SENSITIVENESS 5e-2
constexpr  uint32_t controlPointsNums = 25;
class mlBezier
{
public:
	mlBezier();
	~mlBezier();


	glm::vec3 mlEvalBezierCurve(const glm::vec3 *P, const float &t);
	glm::vec3 mlEvalBezierPatch(const glm::vec3 *controlPoints, const float &u, const float &v);
	glm::vec3 derivBezier(const glm::vec3 *P, const float &t);
	glm::vec3 dUBezier(const glm::vec3 *controlPoints, const float &u, const float &v);
	glm::vec3 dVBezier(const glm::vec3 *controlPoints, const float &u, const float &v);

	void mlCreateBeizermesh();
	void mlTriangularization();

	int getSelectedControlPointIndice(int posX, int posY);
	void getRayScreenToSpace(int posX, int posY, glm::vec3 &outRay_o, glm::vec3 &outRay_d);
	double distancePointToRay(glm::vec3 ray_o, glm::vec3 ray_d, glm::vec3 p);
	void updateControlPointPosition(int posX, int posY, int selectedInd);
public:

	uint32_t divs;
	std::vector<glm::vec3> P;
	std::vector<glm::vec3> N;
	std::vector<glm::vec2> st;

	std::vector<int> indicesofControlpoints;
	std::vector<int> indicesofP;
public:
	int u; // u columns of points
	int v; // v rows of points
	int uc; // uc columns of control points
	int vc;	// vc rows of control points
	glm::vec3 controlPoints[25]
		=
	{
		{ -2.0, -2.0,  1.0 },
	{ -0.5, -2.0,  0.0 },
	{ 0.5, -2.0, -2.0 },
	{ 2.0, -2.0,  2.0 },
	{ 4.0, -2.0, 1.0 },	


	{ -2.0, -0.5,  2.0 },
	{ -0.5, -0.5,  1.5 },
	{ 0.5, -0.5,  0.0 },
	{ 2.0, -0.5, -2.0 },
	{ 4.0, -0.5, 1.0 },	

	{ -2.0,  0.5,  2.0 },
	{ -0.5,  0.5,  1.0 },
	{ 0.5,  0.5, -1.0 },
	{ 2.0,  0.5,  1.0 },
	{ 4.0, 0.5, 1.0 },

	{ -2.0,  2.0, -1.0 },
	{ -0.5,  2.0, -1.0 },
	{ 0.5,  2.0,  0.0 },
	{ 2.0,  2.0, 1.5 },
	{ 4.0, 2.0, -1.0 },

	{ -2.0,  3.5, 3.0 },
	{ -0.5,  3.5, 1.0 },
	{ 0.5,  3.5,  0.0 },
	{ 2.0,  3.5, -0.5 },
	{ 4.0, 3.5, 4.0 },

	};
};

mlBezier::mlBezier()
{
}

mlBezier::~mlBezier()
{
}

glm::vec3 mlBezier::mlEvalBezierCurve(const glm::vec3 * P, const float & t)
{
	return glm::vec3(0.0);
}

glm::vec3 mlBezier::mlEvalBezierPatch(const glm::vec3 * controlPoints, const float & u, const float & v)
{
	return glm::vec3(0.0);
}

inline glm::vec3 mlBezier::derivBezier(const glm::vec3 * P, const float & t)
{
	return glm::vec3(0.0);
}

glm::vec3 mlBezier::dUBezier(const glm::vec3 * controlPoints, const float & u, const float & v)
{
	return glm::vec3(0.0);
}

glm::vec3 mlBezier::dVBezier(const glm::vec3 * controlPoints, const float & u, const float & v)
{
	return glm::vec3(0.0);

}

void mlBezier::mlCreateBeizermesh()
{
	for (size_t i = 0; i < u - 1; i++)
	{
		for (size_t j = 0; j < v - 1; j++) {
			int ind = i * u + j;
			indicesofP.push_back(ind);
			indicesofP.push_back(ind + 1);
			indicesofP.push_back(ind + u);

			indicesofP.push_back(ind + 1);
			indicesofP.push_back(ind + u);
			indicesofP.push_back(ind + u + 1);
		};
	};
}

void mlBezier::mlTriangularization()
{
	for (int j = 0; j < vc - 1; j++)
	{
		for (int i = 0; i < uc - 1; i++)
		{
			int ind = j * uc + i;
			indicesofControlpoints.push_back(ind);
			indicesofControlpoints.push_back(ind + 1);

			indicesofControlpoints.push_back(ind + uc + 1);
			indicesofControlpoints.push_back(ind + uc);
		}
	}

}




// you will fill the four function to edit control points
int mlBezier::getSelectedControlPointIndice(int posX, int posY)
{
	return 0;
}

void mlBezier::getRayScreenToSpace(int posX, int posY, glm::vec3 &outRay_o, glm::vec3 &outRay_d)
{

}

double mlBezier::distancePointToRay(glm::vec3 ray_o, glm::vec3 ray_d, glm::vec3 p)
{
	return 0;
}

void mlBezier::updateControlPointPosition(int posX, int posY, int selectedInd)
{
}

#endif // !_MLBEZIERH_

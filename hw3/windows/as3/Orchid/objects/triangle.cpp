#include "triangle.h"

namespace Orchid
{
	Triangle::Triangle()
	{

	}
	Triangle::Triangle(const Vector3d & p0, const Vector3d & p1, const Vector3d & p2, Material material)
	{
		_p0 = p0;
		_p1 = p1;
		_p2 = p2;
		_material = material;
	}
	Triangle::Triangle(const Triangle & tri)
	{
		this->operator=(tri);
	}
	Triangle::~Triangle()
	{
	}
	Triangle & Triangle::operator=(const Triangle & tri)
	{
		// TODO: insert return statement here
		this->_p0 = tri._p0;
		this->_p1 = tri._p1;
		this->_p2 = tri._p2;
		this->_material = tri._material;
		return *this;
	}
	Vector3d Triangle::gravity() const
	{
		return Vector3d();
	}
	AABBox Triangle::get_bounding_box()
	{
		Vector3d bl = Vector3d(
			std::min(_p0.x(), std::min(_p1.x(), _p2.x())),
			std::min(_p0.y(), std::min(_p1.y(), _p2.y())),
			std::min(_p0.z(), std::min(_p1.z(), _p2.z()))
		);

		Vector3d tr = Vector3d(
			std::max(_p0.x(), std::max(_p1.x(), _p2.x())),
			std::max(_p0.y(), std::max(_p1.y(), _p2.y())),
			std::max(_p0.z(), std::max(_p1.z(), _p2.z()))
		);
		return AABBox(bl, tr);
	}
	bool Triangle::triangleInBbox()
	{
		return false;
	}
	Vector3d Triangle::get(int id) const
	{
		Assertion(0 <= id && id <= 2, "ID must be between 0 and 2");
		if (0 == id)
			return _p0;
		if (1 == id)
			return _p1;
		if (2 == id)
			return _p2;
	}
	Vector3d Triangle::normal() const
	{
		return (_p0 - _p1).cross((_p1 - _p2)).normalized();
	}
	ObjectIntersection Triangle::getIntersection(const Ray & ray)
	{
		const Vector3d e1 = _p1 - _p0;
		const Vector3d e2 = _p2 - _p0;
		const Vector3d s = ray.origin() - _p0;
		double head = 1 / (e1.dot(ray.direction().cross(e2)));
		
		double t = head * (e2.dot(s.cross(e1)));
		double b1 = head * (s.dot(ray.direction().cross(e2)));
		double b2 = head * (ray.direction().dot(s.cross(e1)));
		double b = b1 + b2;

		if (b <= 1 && b1 > 0 && b2 > 0)
		{
			Vector3d intersectPoint = ray.origin() + t * ray.direction();
			Vector3d norm = (_p0 - _p1).cross((_p1 - _p2));
			norm = norm.normalized();

			return ObjectIntersection(true, t, norm, _material);
		}
		return ObjectIntersection(false, 0.0, Vector3d(0), Material(DIFF,Vector3d(0.0)));
	}
	double Triangle::area() const
	{
		const Vector3d e1 = _p1 - _p0;
		const Vector3d e2 = _p2 - _p0;
		return 0.5 * Vector3d::cross(e1, e2).norm();
	}
	// Returns the midpoint of the triangle
	Vector3d Triangle::get_midpoint() 
	{
		return (_p0 + _p1 + _p2) / 3.0;
	}

	Vector3d Triangle::get_color_at()
	{
		return _material.get_colour();
	}
}

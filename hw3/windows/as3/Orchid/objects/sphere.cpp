#include "sphere.h"
namespace  Orchid
{
	Sphere::Sphere(Vector3d position, double radius, Material material)

	{
		_position = position;
		_radius = radius;
		_material = material;
	}
	ObjectIntersection Sphere::getIntersection(const Ray & ray)
	{
		Vector3f dir = ray.direction();
		Vector3f pos = ray.origin() - _position;
		Vector3f ori = ray.origin();
		double A = dir.dot(dir);
		double B = 2 * dir.dot(pos);
		double C = pos.dot(pos) - pow(_radius,2);
		double delta = pow(B, 2) - 4 * A * C;
		if (delta >= 0)
		{
			float t0 = (-B - sqrt(delta)) / (2 * A);
			float t1 = (-B + sqrt(delta)) / (2 * A);
			if (t1 < 0 && t0 < 0)
			{
				return ObjectIntersection(false, 0, Vector3d(0.0), Material(DIFF));
			}
			float t = t0*t1 >= 0? std::min(t0,t1) : std::max(t0,t1);
			Vector3f intersectPoint = ori + t * dir;
			Vector3d norm = ( _position - intersectPoint).normalized();
			return ObjectIntersection(true, t, norm, _material);
		}
		return ObjectIntersection(false, 0, Vector3d(0.0), Material(DIFF));
	}
}

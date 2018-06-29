#include "quad.h"
#include "triangle.h"

namespace Orchid
{
	Quad::Quad(
		const Vector3d & a,
		const Vector3d & b,
		const Vector3d & c,
		const Vector3d & d,
		const Material & material)
		: _a{ a }
		, _b{ b }
		, _c{ c }
		, _d{ d }
		, _material{ material }
	{
	}
	ObjectIntersection Quad::getIntersection(const Ray & ray)
	{
		Triangle tri1 = Triangle(_a, _b, _c, _material);
		ObjectIntersection isct1 = tri1.getIntersection(ray);
		if (isct1._hit) {
			return isct1;
		}
		Triangle tri2 = Triangle(_a, _c, _d, _material);
		ObjectIntersection isct2 = tri2.getIntersection(ray);
		if (isct2._hit)
		{
			return isct2;
		}
		return ObjectIntersection(0, 0, Vector3d(0.0), Material(DIFF));
	}
}

#include "material.h"
#include "../../lib/rand48/erand48.h"
#define M_PI 3.14159265358979323846

namespace Orchid
{

	Material::Material
	(MaterialType t, Vector3d colour, Vector3d emission, Texture texture)
		:_type{ t }
		, _colour{ colour }
		, _emission{ emission }
		, _texture{ texture }  //nead addd
	{

	}

	Material & Material::operator=(Material  m)
	{
		this->_type = m._type;
		this->_emission = m._emission;
		this->_texture = m._texture;
		this->_colour = m._colour;
		return *this;
	}

	Vector3d Material::get_colour_at(double u, double v) const
	{
		if (_texture.is_loaded())
			return _texture.get_pixel(u, v);
		return _colour;
	}

	Ray Material::get_reflected_ray
	(
		const Ray & r, 
		Vector3d & p, 
		const Vector3d & n,
		unsigned short * Xi
	) const
	{
		if (_type == SPEC)
		{
			double roughness = 0.8;
			Vector3d tmp = r.direction() - n * n.dot(r.direction()) * 2;
			Vector3d refRay = Vector3d(tmp.x() + (erand48(Xi) - 0.5)*roughness, tmp.y() + (erand48(Xi) - 0.5)*roughness, tmp.z() + (erand48(Xi) - 0.5)*roughness).normalized();
			return Ray(p, refRay);
		}

		if (_type == DIFF)
		{
			//std::cout << "hit diff" << std::endl;
			Vector3d l = n.dot(r.direction()) < 0 ? n : Vector3d(-1,-1,-1) * n ;
			double r1 = 2 * M_PI * erand48(Xi);
			double r2 = erand48(Xi);
			double r2s = sqrt(r2);
			Vector3d w, u, v;
			w = l;
			if (fabs(w.x()) > EPS)
			{
				u = Vector3d(0, 1, 0).cross(w).normalized();
			}
			else
			{
				u = Vector3d(1, 0, 0).cross(w).normalized();
			}
			v = w.cross(u);
			Vector3d direction = (u*cos(r1)*r2s + v * sin(r1)*r2s + w * sqrt(1 - r2)).normalized();
			return Ray(p, direction);
		}
	}
}
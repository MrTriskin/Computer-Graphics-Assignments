#include "scene.h"
#include <random> 
#include "../sample/sampler.h"
#include "../../lib/rand48/erand48.h"
namespace Orchid
{
	
	
	void Scene::add(Object * object)
	{
		_objects.push_back(object);
	}

	ObjectIntersection Scene::intersect(const Ray & ray)
	{
		ObjectIntersection isct = ObjectIntersection();
		ObjectIntersection temp;
		long size = _objects.size();

		for (int i = 0; i < size; i++)
		{
			temp = _objects.at((unsigned)i)->getIntersection(ray);

			if (temp._hit) {
				if (isct._u == 0 || temp._u < isct._u) isct = temp;
			}
		}
		return isct;
	}

	Vector3d Scene::trace_ray(const Ray & ray, int depth, unsigned short * Xi)
	{
		ObjectIntersection isct = intersect(ray);
		// if not hit, return
		if (!isct._hit)
		{
			//std::cout << "do not hit" << std::endl;
			return Vector3d();
		}
		if (isct._hit && isct._material.getType() == EMIT)
		{
			//std::cout << "hit emit" << std::endl;
			return isct._material.get_emission();
		}
		Vector3d color = isct._material.get_colour();
		// calculate max reflection
		double maxReflection = std::max(color.x(), std::max(color.y(), color.z()));
		double rnd = erand48(Xi);
		depth += 1;
		if (depth>5)
		{
			/*if (rnd<maxReflection*0.9)
			{
				color = color * (0.9 / maxReflection);
			}
			else
			{
				return isct._material.get_emission();
			}*/
			return isct._material.get_emission();
		}

		Vector3d ref = ray.origin() + ray.direction() * isct._u;
		Ray refRay = isct._material.get_reflected_ray(ray, ref, isct._normal, Xi);

		return color.multiply(trace_ray(refRay, depth, Xi));
	}




}
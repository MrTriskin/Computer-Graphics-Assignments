#include "camera.h"
#include <random>


namespace Orchid
{
	Camera::Camera
	(
		Vector3d position, Vector3d target, Vector3d up,
		int width, int height,
		double nearPalneDistance, double fov
	) :	_position{ position },
		_up{ up },
		_imageW{ width },
		_imageH{ height },
		_nearPlaneDistance{ nearPalneDistance }	{
		_cameraVerticalFOV = fov *PI / 180.0f; //ª°∂»÷∆
		_aspectRatio = (double)_imageW / _imageH;
		_cameraFwd = (target - _position).normalized();
		_cameraRight = _up.cross(_cameraFwd).normalized();
		_cameraUp = _cameraFwd.cross(_cameraRight).normalized()*(-1);///???
		_cameraHorizFOV = _cameraVerticalFOV *_aspectRatio;
		_windowTop = tan(_cameraVerticalFOV / 2.0f)*_nearPlaneDistance;
		_windowRight = tan(_cameraHorizFOV / 2.0f)*_nearPlaneDistance;
	}

	Ray Camera::get_ray(int x, int y, bool jitter, unsigned short * Xi)
	{
		double xJitter = jitter ? erand48(Xi) : 0;
		double yJitter = jitter ? erand48(Xi) : 0;
		Vector3d px = _cameraRight * (0.5 - ((x + xJitter)*_aspectRatio / _imageW)) * 2 * _windowRight;
		Vector3d py = _cameraUp * (((y + yJitter) / _imageH) - 0.5) * 2 * _windowTop;
		Vector3d direction = (_nearPlaneDistance*_cameraFwd + px + py).normalized();
		return Ray(_position, direction);
	}

}

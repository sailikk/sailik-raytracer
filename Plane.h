#ifndef _PLANE_H
#define _PLANE_H

#include "math.h"
#include "Object.h"
#include "Vect.h"
#include "Color.h"

class Plane : public Object {
	Vect normal;
	double distance;
	Color color;

public:
	Plane ();

	Plane (Vect, double, Color);

	// method functions
	virtual Vect getPlaneNormal() { return normal; }
	double getPlaneDistance() { return distance; }
	virtual Color getColor() { return color; }

	Vect getNormalAt(Vect point) {
		return normal;
	}

	double findIntersection(Ray ray) {
		Vect ray_direction = ray.getRayDirection();
		double a = ray_direction.dotProduct(normal);
		if (a == 0) {
			// ray is parallel to the plane
			return -1;
		} else {
			double b = normal.dotProduct(
				ray.getRayOrigin().vectAdd(
					normal.vectMult(distance).negative()
					// See your SO: Multiplying the normal by what
					// vector will give the center of a plane?
					// to see why this equates "center" for plane
					// with a normal (0, 1, 0)
				)
			);
			return (-1*b)/a;
			// Distance from ray origin to point intersection.
		}
	}
};

Plane::Plane () {
	normal = Vect(1, 0, 0); // UP
	// here X is up, Z is depth into scene, Y is left/right
	distance = 0;
	color = Color(0.5, 0.5, 0.5, 0);
}

Plane::Plane(Vect normalValue, double distanceValue, Color colorValue) {
	normal = normalValue;
	distance = distanceValue;
	color = colorValue;
}

#endif

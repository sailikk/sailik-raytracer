#ifndef _TRIANGLE_H
#define _TRIANGLE_H

#include "math.h"
#include "Object.h"
#include "Vect.h"
#include "Color.h"

class Triangle : public Object {
	Vect A, B, C;
	Vect normal;
	double distance;
	Color color;

public:
	Triangle ();

	Triangle (Vect, Vect, Vect, Color);

	// getter functions
	virtual Vect getTriangleNormal() {
		// Must find something that points into screen in this case
		// Use right hand rule
		Vect CA  = C.vectAdd(A.negative());
		Vect BA = B.vectAdd(A.negative());
		normal = CA.crossProduct(BA).normalize();
		return normal;
	}

	double getTriangleDistance() {
		// distance can be found by taking dot product of the normal
		// with one of the points in that plane??
		// kinda wakannai
		normal = getTriangleNormal();
		distance = normal.dotProduct(A);
		return distance;
	}

	virtual Color getColor() { return color; }

	Vect getNormalAt(Vect point) {
		normal = getTriangleNormal();
		return normal;
	}

	double findIntersection(Ray ray) {
		Vect ray_direction = ray.getRayDirection();
		Vect ray_origin = ray.getRayOrigin();
		normal = getTriangleNormal();
		distance = getTriangleDistance();

		double a = ray_direction.dotProduct(normal);

		if (a == 0) {
			// ray is parallel to the Triangle
			return -1;
		} else {
			double b = normal.dotProduct(
				ray.getRayOrigin().vectAdd(
					normal.vectMult(distance).negative()
					// See your StckOvflow: Multiplying the normal by what
					// vector will give the center of a Triangle?
					// to see why this equates "center" for Triangle
					// with a normal (0, 1, 0)
				)
			);
			double distance2plane = -1*b/a;

			double Qx = ray_direction.vectMult(distance2plane).getVectX()
			            + ray_origin.getVectX();
			double Qy = ray_direction.vectMult(distance2plane).getVectY()
			            + ray_origin.getVectY();
			double Qz = ray_direction.vectMult(distance2plane).getVectZ()
			            + ray_origin.getVectZ();

			Vect Q (Qx, Qy, Qz);

			// If the following conditions are met, we are inside triangle
			// ^ where does this test come from?
			// [CAxQA]*n >= 0
			Vect CA = C.vectAdd(A.negative());
			Vect QA = Q.vectAdd(A.negative());
			double test1 = (CA.crossProduct(QA)).dotProduct(normal);
			// ^ why dot product?? wakannai
			// [BCxQC]*n >= 0
			Vect BC = B.vectAdd(C.negative());
			Vect QC = Q.vectAdd(C.negative());
			double test2 = (BC.crossProduct(QC)).dotProduct(normal);
			// [ABxQB]*n >= 0
			Vect AB = A.vectAdd(B.negative());
			Vect QB = Q.vectAdd(B.negative());
			double test3 = (AB.crossProduct(QB)).dotProduct(normal);
			// where Q is the point of intersection with ray
			if ((test1 >= 0) && (test2 >= 0) && (test3 >= 0)) {
				// inside triangle
				// wakannai .. why do we care whether it's inside or outside
				// the triangle
				return distance2plane;
				// shortest dist from ray origin to point intersection
			} else {
				// outside triangle
				return -1;
			}
		}
	}
};

Triangle::Triangle () {
	A = Vect(1, 0, 0);
	B = Vect(0, 1, 0);
	C = Vect(0, 0, 1);
	// here X is up, Z is depth into scene, Y is left/right
	color = Color(0.5, 0.5, 0.5, 0);
}

Triangle::Triangle(Vect pA, Vect pB, Vect pC, Color colorValue) {
	A = pA;
	B = pB;
	C = pC;
	color = colorValue;
}

#endif

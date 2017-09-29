/*
  CPE 471 Lab 1 
  Base code for Rasterizer
  Example code using B. Somers' image code - writes out a sample tga
*/

#include <stdio.h>
#include <stdlib.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Image.h"
#include "types.h"

using namespace glm;

struct Sphere {
	glm::vec3 position;
	float radius;
	glm::vec3 specular;
	glm::vec3 diffuse;
	glm::vec3 ambient;
};

struct Ray {
	glm::vec3 point;
	glm::vec3 dir;
};

double bMin = 0;

vec3 intersect(Ray &r, Sphere &s) {
	double A = dot(r.dir, r.dir);
	double B = 2 * dot(r.point - s.position, r.dir);
	double C = dot(r.point - s.position, r.point - s.position) - (s.radius*s.radius);
	double discriminate = (B*B - 4 * A*C);
	double tPlus = 0, tMinus = 0;

	if (B < bMin)
		bMin = B;

	if (discriminate < 0) {
		return vec3(INT32_MAX, 0, 0);
	}

	tPlus = (-B + sqrt(discriminate)) / (2 * A);
	tMinus = (-B - sqrt(discriminate)) / (2 * A);

	vec3 collision = vec3(INT32_MAX, 0, 0);
	if (tPlus <= tMinus && tPlus >= 0) {
		collision = r.point + normalize(r.dir) * (float)tPlus;
	}
	else if (tPlus >= tMinus  && tMinus >= 0) {
		collision = r.point + normalize(r.dir) * (float)tMinus;
	}
	else if (tPlus >= 0) {
		collision = r.point + normalize(r.dir) * (float)tPlus;
	}
	else if (tMinus >= 0) {
		collision = r.point + normalize(r.dir) * (float)tMinus;
	}

	return collision;
}

int main(void) {

	// make a color
	color_t fragColor;
	fragColor.f = 1;

	Sphere sphere1 = { vec3(-6, 3, -15), 5.0f, vec3(0,0,.5), vec3(0,0,2), vec3(.1,0,.2) };
	Sphere sphere2 = { vec3(6, 3, -15), 5.0f, vec3(0,0,.5), vec3(0,0,2), vec3(.1,0,.2) };
	Sphere sphere3 = { vec3(0, 0, -15), 7.0f, vec3(.5,0,0), vec3(2,0,0), vec3(.1,0,.2) };
	Sphere sphere4 = { vec3(-2, 2, -8), 0.8f, vec3(0,0,0), vec3(0,0,0), vec3(0,0,0) };
	Sphere sphere5 = { vec3(2, 2, -8), 0.8f, vec3(0,0,0), vec3(0,0,0), vec3(0,0,0) };
	Sphere sphere6 = { vec3(0, 0, -8), 1.0f, vec3(0,0,0), vec3(0,0,0), vec3(0,0,0) };
	Sphere spheres[6] = { sphere1, sphere2, sphere3, sphere4, sphere5, sphere6 };

	vec3 eye = vec3(0, 0, 0);
	vec3 up = vec3(0, 1, 0);
	vec3 w = vec3(0, 0, -1);
	vec3 u = cross(w, up);
	vec3 v = cross(u, w);

	vec3 lightColor = vec3(1, 1, 1);
	vec3 lightPos = vec3(100, 100, 100);

	float minX = -10;
	float maxX = 10;
	float minY = -7.5;
	float maxY = 7.5;

	float incX = (maxX - minX) / 640.0f;
	float incY = (maxY - minY) / 480.0f;

	vec3 upperLeftC = vec3(minX, maxY, -4);
	vec3 centerPixel = vec3(upperLeftC.x + incX / 2, upperLeftC.y - incY / 2, upperLeftC.z);

	// make a 640x480 image (allocates buffer on the heap)
	Image img(640, 480);
	for (int j = 0; j < 480; j++) {
		for (int i = 0; i < 640; i++) {
			vec3 closestCollision = vec3(INT32_MAX, 0, 0);
			int closestSphere = -1;
			for (int n = 0; n < 6; n++) {
				Sphere sphere = spheres[n];
				Ray r = { eye, normalize(centerPixel - eye) };
				vec3 collision = intersect(r, sphere);
				if (distance(collision, eye) < distance(closestCollision, eye)) {
					closestCollision = collision;
					closestSphere = n;
				}
			}

			if (closestSphere > -1) {
				vec3 tangent = closestCollision - spheres[closestSphere].position;
				vec3 normal = normalize(tangent);
				vec3 view = normalize(centerPixel - eye);
				vec3 L = normalize(lightPos - spheres[closestSphere].position);

				vec3 R = -L + 2 * dot(L, normal) * normal;
				vec3 spec = spheres[closestSphere].specular * pow(max(0.0f, dot(R, view)), 200) * lightColor;
				vec3 diff = spheres[closestSphere].diffuse * max(0.0f, dot(L, normal)) * lightColor;
				vec3 amb = spheres[closestSphere].ambient * lightColor;
				fragColor.r = spec.r + diff.r + amb.r;
				fragColor.g = spec.g + diff.g + amb.g;
				fragColor.b = spec.b + diff.b + amb.b;
				img.pixel(i, 480 - j - 1, fragColor);
			}
			else {
				fragColor.r = .3;
				fragColor.g = .15;
				fragColor.b = 0.8;
				img.pixel(i, 480 - j - 1, fragColor);
			}
			centerPixel.x += incX;
	}
	centerPixel.y -= incY;
	centerPixel.x = upperLeftC.x + incX / 2;
}


  // write the targa file to disk
  img.WriteTga((char *)"awesome.tga", true); 
  // true to scale to max color, false to clamp to 1.0

}

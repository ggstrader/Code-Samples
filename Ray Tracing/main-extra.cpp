#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <fstream>
#include <vector>
#include <iostream>
#include <cassert>
#include <random>
#include <algorithm>
#include <Eigen>

using namespace Eigen;
using namespace std;

const float decay_rate = 0.12f;
const Vector3f amb_color = Vector3f(1.0f, 1.0f, 1.0f);
const float amb_coeff = 0.01f;

//Vector3f bgcolor(1.0f, 1.0f, 1.0f);
Vector3f bgcolor(.0f, .0f, .0f);

std::vector<Vector3f> lightPositions = { Vector3f(0.0, 150, 20)
									   , Vector3f(-130.0, 60, 200)
									   , Vector3f(50.0, 60, 100) };

float lcontrib = (float)((double)1.0 / (double)lightPositions.size());

class Sphere
{
public:
	Vector3f center;  // position of the sphere
	float radius;  // sphere radius
	Vector3f surfaceColor; // surface color
	bool isSpecular;
	Sphere(
		const Vector3f &c,
		const float &r,
		const Vector3f &sc,
		bool sp) :
		center(c), radius(r), surfaceColor(sc), isSpecular(sp)
	{
	}

	// line vs. sphere intersection (note: this is slightly different from ray vs. sphere intersection!)
	bool intersect(const Vector3f &rayOrigin, const Vector3f &rayDirection, float &t0, float &t1) const
	{
		Vector3f l = center - rayOrigin;
		float tca = l.dot(rayDirection);
		if (tca < 0) return false;
		float d2 = l.dot(l) - tca * tca;
		if (d2 > (radius * radius)) return false;
		float thc = sqrt(radius * radius - d2);
		t0 = tca - thc;
		t1 = tca + thc;

		return true;
	}
};

Vector3f diffuse(const Vector3f &L, const Vector3f &N, const Vector3f &diffuseColor, const float kd)
{
	return kd * max(L.dot(N), 0.0f)*diffuseColor * lcontrib;
}

Vector3f phong(const Vector3f &L, const Vector3f &N, const Vector3f &V, const Vector3f &R, const Vector3f &diffuseColor, const Vector3f &specularColor, const float kd, const float ks, const float alpha)
{
	Vector3f resColor = Vector3f::Zero();
	resColor = diffuse(L, N, diffuseColor, .55f);
	resColor += specularColor * ks * pow(max(R.dot(V), 0.0f), alpha) * lcontrib;
	return resColor;
}

Sphere* getNearestSphere(const Vector3f &rayOrigin, const Vector3f &rayDirection, const std::vector<Sphere> &spheres, float&t0) {
	float t1 = 0, closest = 0;
	Sphere* nearest = NULL;
	for (auto sphere : spheres) {
		if (sphere.intersect(rayOrigin, rayDirection, t0, t1))
		{
			if (closest && closest <= t0) continue;
			closest = t0;
			delete nearest;
			nearest = new Sphere(sphere); // for some reason making a pointer directly to &sphere makes the color gray
		}
	}
	t0 = closest;
	return nearest;
}
bool isObstructed(const Vector3f &rayOrigin, const Vector3f &rayDirection, const std::vector<Sphere> &spheres) {
	float t0 = 0, t1 = 0;
	for (auto sphere : spheres) {
		if (sphere.intersect(rayOrigin, rayDirection, t0, t1))
			return true;
	}
	return false;
}

Vector3f trace_help(const Vector3f &rayOrigin, const Vector3f &rayDirection, const std::vector<Sphere> &spheres, int depth) {
	Vector3f pixelColor = Vector3f::Zero();
	if (depth < 1) return pixelColor;
	depth--;
	float t0 = 0, t1 = 0;
	Sphere* nearest = getNearestSphere(rayOrigin, rayDirection, spheres, t0);
	if (!nearest) return bgcolor;
	Vector3f bounce = (rayDirection * t0) + rayOrigin;
	Vector3f normal = bounce - nearest->center;
	normal.normalize();
	Vector3f reflection = (2.0f * normal) * normal.dot(rayDirection) - rayDirection;
	reflection.normalize();

	for (auto light : lightPositions) {
		Vector3f light_direction = light - bounce;
		light_direction.normalize();
		Vector3f reverse_ray_norm = -light_direction;
		if (!isObstructed(bounce, light_direction, spheres)) {
			pixelColor += phong(light_direction, normal, reverse_ray_norm, reflection, nearest->surfaceColor, Vector3f::Ones(), lcontrib, 5.40f, 70);
			pixelColor += amb_color * amb_coeff;
		}
	}

	if (!nearest->isSpecular) return pixelColor;
	return pixelColor += (decay_rate * trace_help(bounce, -reflection, spheres, depth));
}

Vector3f trace(const Vector3f &rayOrigin, const Vector3f &rayDirection, const std::vector<Sphere> &spheres)
{
	int max_depth = 1;
	return trace_help(rayOrigin, rayDirection, spheres, max_depth);
}



void render(const std::vector<Sphere> &spheres)
{
	unsigned width = 640;
	unsigned height = 480;
	Vector3f *image = new Vector3f[width * height];
	Vector3f *pixel = image;
	float invWidth = 1 / float(width);
	float invHeight = 1 / float(height);
	float fov = 30;
	float aspectratio = width / float(height);
	float angle = tan(M_PI * 0.5f * fov / 180.f);

	// Trace rays
	for (unsigned y = 0; y < height; ++y)
	{
		for (unsigned x = 0; x < width; ++x)
		{
			float rayX = (2 * ((x + 0.5f) * invWidth) - 1) * angle * aspectratio;
			float rayY = (1 - 2 * ((y + 0.5f) * invHeight)) * angle;
			Vector3f rayDirection(rayX, rayY, -1);
			rayDirection.normalize();
			*(pixel++) = trace(Vector3f::Zero(), rayDirection, spheres);
		}
	}

	// Save result to a PPM image
	std::ofstream ofs("./out007.ppm", std::ios::out | std::ios::binary);
	ofs << "P6\n" << width << " " << height << "\n255\n";
	for (unsigned i = 0; i < width * height; ++i)
	{
		const float x = image[i](0);
		const float y = image[i](1);
		const float z = image[i](2);

		ofs << (unsigned char)(std::min(float(1), x) * 255)
			<< (unsigned char)(std::min(float(1), y) * 255)
			<< (unsigned char)(std::min(float(1), z) * 255);
	}

	ofs.close();
	delete[] image;
}


int main(int argc, char **argv)
{
	std::vector<Sphere> spheres;
	// position, radius, surface color
	spheres.push_back(Sphere(Vector3f(0.0, -10004, -20), 10000, Vector3f(0.40, 0.40, 0.40), false));
	spheres.push_back(Sphere(Vector3f(0.0, 0, -20), 4, Vector3f(1.00, 0.32, 0.36), true));
	spheres.push_back(Sphere(Vector3f(-2.65, -2.5, -15), .5, Vector3f(0.00, 0.32, 0.9), true));
	spheres.push_back(Sphere(Vector3f(5.0, -1, -15), 2, Vector3f(0.90, 0.76, 0.46), true));
	spheres.push_back(Sphere(Vector3f(5.0, -1.7, -25), 3, Vector3f(0.65, 0.77, 0.97), true));
	spheres.push_back(Sphere(Vector3f(-5.5, 0.5, -13), 3, Vector3f(0.250, 0.250, 0.250), true));

	render(spheres);
	double b;
	//std::cin >> b;
	return 0;
}

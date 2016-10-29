/** Copyright 2016 Alex Yang */
#ifndef SRC_SCENE_H_
#define SRC_SCENE_H_

#include <vector>
#include <string>
#include <memory>

#include "Sphere.h"
#include "Triangle.h"
#include "Light.h"
#include "Camera.h"
#include "Matrix.h"
#include "Ray.h"
#include "Material.h"

class Scene {
 public:
  const int kResolution = 240;
  const int kBounceLimit = 1;
  const Color kBackgroundColor = Color(0, 0, 0);

  std::vector<Sphere> spheres;
  std::vector<Triangle> triangles;
  Color ambientLight;
  std::vector<std::unique_ptr<Light>> lights;
  Camera camera;

  Matrix4 xfIn;
  Matrix4 xfOut;

  Material material;

  Scene();

  void parseLine(std::string line);
  void render();
  Color trace(const Ray& ray);
  Color trace(const Ray& ray, int bouncesLeft);
  Ray intersect(Ray ray, Sphere s);

  /* Shading */
  Color ambient(const Color& ka);
  Color diffuse(const Vector3& p, const Vector3& n, const Vector3& l, const Color& kd, const Color& intensity);
  Color specular(const Vector3& p, const Vector3& n, const Vector3& v, const Vector3& l, const Color& ks, double sp, const Color& intensity);
  double specularIncidence(const Vector3& p, const Vector3& n, const Vector3& v, const Vector3& l, double sp);
};

#endif  // SRC_SCENE_H_

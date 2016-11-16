#ifndef SRC_SCENE_H_
#define SRC_SCENE_H_

#include <vector>
#include <string>
#include <memory>

#include "Geometry.h"
#include "Light.h"
#include "Camera.h"
#include "Matrix.h"
#include "Ray.h"
#include "Material.h"
#include "Profiler.h"

class Scene {
 public:
  const int kBounceLimit = 1;
  const Color kBackgroundColor = Color(0, 0, 0);

  int resolution;
  int antialias;

  std::vector<std::unique_ptr<Geometry>> objects;
  std::vector<std::unique_ptr<Light>> lights;
  Color ambientLight;
  Camera camera;

  Matrix4 xfIn;
  Matrix4 xfOut;

  Material material;

  Profiler profiler;

  Scene(int res, int aa);

  int getWidth();
  int getHeight();

  void parseLine(std::string line);
  void parseObj(std::string filename);
  std::vector<Color> render();
  Color trace(const Ray& ray);
  Color trace(const Ray& ray, int bouncesLeft);
  Ray intersect(Ray ray, Sphere s);
  std::vector<std::pair<double, double>> jitteredGrid(int size);

  /* Shading */
  Color ambient(const Color& ka);
  Color diffuse(const Vector3& p, const Vector3& n, const Vector3& l,
    const Color& kd, const Color& intensity);
  Color specular(const Vector3& p, const Vector3& n, const Vector3& v,
    const Vector3& l, const Color& ks, double sp, const Color& intensity);
  double specularIncidence(const Vector3& p, const Vector3& n, const Vector3& v,
    const Vector3& l, double sp);
};

#endif  // SRC_SCENE_H_

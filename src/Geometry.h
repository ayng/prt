/** Copyright 2016 Alex Yang */
#ifndef SRC_GEOMETRY_H_
#define SRC_GEOMETRY_H_

#include "Material.h"
#include <Eigen/Dense>
#include "Ray.h"

static Eigen::Vector3d NAN_VECTOR(nan(""), nan(""), nan(""));


class Geometry {
 public:
  Material material;
  Eigen::Transform<double,3,Eigen::Affine> worldToObject, objectToWorld;

  Geometry();
  Geometry(Material mat, Eigen::Transform<double,3,Eigen::Affine> w2o, Eigen::Transform<double,3,Eigen::Affine> o2w);
  virtual Ray intersect(const Ray& ray) = 0;
};

class Triangle : public Geometry {
 public:
  Eigen::Vector3d a, b, c;
  Eigen::Vector3d normal;

  Triangle();
  Triangle(Eigen::Vector3d aa, Eigen::Vector3d bb, Eigen::Vector3d cc,
    Material mat, Eigen::Transform<double,3,Eigen::Affine> w2o, Eigen::Transform<double,3,Eigen::Affine> o2w);
  Ray intersect(const Ray& ray);
};

class Sphere : public Geometry {
 public:
  Eigen::Vector3d center;
  double radius;

  Sphere();
  Sphere(Eigen::Vector3d c, double r, Material mat, Eigen::Transform<double,3,Eigen::Affine> w2o, Eigen::Transform<double,3,Eigen::Affine> o2w);
  Ray intersect(const Ray& ray);
};

#endif  // SRC_GEOMETRY_H_

/** Copyright 2016 Alex Yang */
#include <cmath>
#include "Light.h"

Light::Light() {}
Light::Light(Color i) : intensity(i) {}

PointLight::PointLight() {}
PointLight::PointLight(Eigen::Vector3d p, Color i) : point(p), Light(i) { }
Eigen::Vector3d PointLight::dirToLight(const Eigen::Vector3d& p) {
  return (point - p).normalized();
}
double PointLight::distanceToLight(const Eigen::Vector3d& p) {
  return (point - p).norm();
}

DirectionalLight::DirectionalLight() {}
DirectionalLight::DirectionalLight(Eigen::Vector3d d, Color i) : dir(d), Light(i) {}
Eigen::Vector3d DirectionalLight::dirToLight(const Eigen::Vector3d& p) {
  return (-1 * dir).normalized();
}
double DirectionalLight::distanceToLight(const Eigen::Vector3d& p) {
  return INFINITY;
}

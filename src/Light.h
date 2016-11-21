#ifndef LIGHT_H
#define LIGHT_H

#include "Color.h"
#include <Eigen/Dense> 

class Light {
 public:
  Color intensity;

  Light();
  Light(Color intensity);
  virtual Eigen::Vector3d dirToLight(const Eigen::Vector3d& p) = 0;
  virtual double distanceToLight (const Eigen::Vector3d& p) = 0;
};

class PointLight : public Light {
  public:
    Eigen::Vector3d point;

    PointLight ();
    PointLight (Eigen::Vector3d p, Color i);
    Eigen::Vector3d dirToLight (const Eigen::Vector3d& p) override;
    double distanceToLight (const Eigen::Vector3d& p) override;
};

class DirectionalLight : public Light {
  public:
    Eigen::Vector3d dir;

    DirectionalLight ();
    DirectionalLight (Eigen::Vector3d d, Color i);
    Eigen::Vector3d dirToLight (const Eigen::Vector3d& p) override;
    double distanceToLight (const Eigen::Vector3d& p) override;
};

class AmbientLight {};
#endif

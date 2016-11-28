#ifndef RAY_H
#define RAY_H

#include <Eigen/Dense> 


class Ray {
 public: 
  Eigen::Vector3d point;
  Eigen::Vector3d dir;

  bool isDefined () const; 
  Ray transform (Eigen::Transform<double,3,Eigen::Affine> transformation) const; 
};


#endif

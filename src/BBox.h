#ifndef SRC_BBOX_H_
#define SRC_BBOX_H_

#include <vector>
#include "Ray.h"

class BBox {
 public:
  Eigen::Vector3d min;
  Eigen::Vector3d max;

  BBox();
  BBox(Eigen::Vector3d _min, Eigen::Vector3d _max);
  Eigen::Vector3d centroid();
  bool isHitBy(const Ray& ray);
  void expand(BBox rhs);
};
      
#endif  // SRC_BBOX_H_

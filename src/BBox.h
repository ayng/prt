#ifndef SRC_BBOX_H_
#define SRC_BBOX_H_

#include <vector>
#include "Vector.h"
#include "Ray.h"

class BBox {
 public:
  Vector3 min;
  Vector3 max;

  BBox();
  BBox(Vector3 _min, Vector3 _max);
  Vector3 centroid();
  bool isHitBy(const Ray& ray);
  void expand(BBox rhs);
};
      
#endif  // SRC_BBOX_H_

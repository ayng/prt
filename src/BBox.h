#ifndef SRC_BBOX_H_
#define SRC_BBOX_H_

#include <vector>
#include "Vector.h"

class BBox {
 public:
  Vector3 min;
  Vector3 max;

  BBox();
  BBox(Vector3 _min, Vector3 _max);
  BBox(std::vector<BBox> boxes);
};
      
#endif  // SRC_BBOX_H_

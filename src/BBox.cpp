#include <algorithm>
#include "BBox.h"

BBox::BBox() {}
BBox::BBox(Vector3 _min, Vector3 _max) : min(_min), max(_max) {}
BBox::BBox(std::vector<BBox> boxes)
  : min(INFINITY, INFINITY, INFINITY),
    max(-INFINITY, -INFINITY, -INFINITY) {
  for (BBox box : boxes) {
    min.x = std::min(min.x, box.min.x);
    min.y = std::min(min.y, box.min.y);
    min.z = std::min(min.z, box.min.z);
    max.x = std::max(max.x, box.max.x);
    max.y = std::max(max.y, box.max.y);
    max.z = std::max(max.z, box.max.z);
  }
}

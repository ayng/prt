#ifndef SRC_BVHNODE_H_
#define SRC_BVHNODE_H_

#include <vector>
#include <queue>
#include <memory>
#include <cmath>
#include <algorithm>
#include <cstdio>

#include "BBox.h"
#include "Geometry.h"
#include "Vector.h"

class BVHNode {
 public:
  std::shared_ptr<BVHNode> left, right;
  std::vector<std::shared_ptr<Geometry>> objects;
  BBox bbox;

  const int kMinPartitionSize = 4;

  BVHNode();
  BVHNode(std::vector<std::shared_ptr<Geometry>> objs, BBox bb);
  bool partition();
  void printStats();
};
      
#endif  // SRC_BVHNODE_H_

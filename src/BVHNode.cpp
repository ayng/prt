#include "BVHNode.h"

BVHNode::BVHNode()
{
}

BVHNode::BVHNode(std::vector<std::shared_ptr<Geometry>> objs, BBox bb)
  : bbox(bb), objects(objs)
{
}

bool BVHNode::partition() {
  if (objects.size() < kMinPartitionSize) {
    return false;
  }
  Vector3 mid = bbox.centroid();

  // Split along x, y, and z.
  std::vector<std::shared_ptr<Geometry>> xLeft, xRight;
  std::vector<std::shared_ptr<Geometry>> yLeft, yRight;
  std::vector<std::shared_ptr<Geometry>> zLeft, zRight;

  for (auto objPtr : objects) {
    Vector3 centroid = objPtr->bbox.centroid();

    if (centroid.x < mid.x)  xLeft.push_back(objPtr);
    else                     xRight.push_back(objPtr);

    if (centroid.y < mid.y)  yLeft.push_back(objPtr);
    else                     yRight.push_back(objPtr);

    if (centroid.z < mid.z)  zLeft.push_back(objPtr);
    else                     zRight.push_back(objPtr);
  }

  // Determine which split was the best.
  int xDiff = std::abs(xRight.size() - xLeft.size());
  int yDiff = std::abs(yRight.size() - yLeft.size());
  int zDiff = std::abs(zRight.size() - zLeft.size());
  int minDiff = std::min(xDiff, std::min(yDiff, zDiff));

  std::vector<std::shared_ptr<Geometry>> leftObjs, rightObjs;
  if (xDiff == minDiff) {
    leftObjs = xLeft;
    rightObjs = xRight;
  } else if (yDiff == minDiff) {
    leftObjs = yLeft;
    rightObjs = yRight;
  } else if (zDiff == minDiff) {
    leftObjs = zLeft;
    rightObjs = zRight;
  }
  // Initialize the resultant nodes.
  BBox leftBBox, rightBBox;
  for (auto objPtr : leftObjs) leftBBox.expand(objPtr->bbox);
  for (auto objPtr : rightObjs) rightBBox.expand(objPtr->bbox);
  left = std::make_shared<BVHNode>(leftObjs, leftBBox);
  right = std::make_shared<BVHNode>(rightObjs, rightBBox);

  return true;
}

void BVHNode::printStats()
{
  std::queue<std::shared_ptr<BVHNode>> q;
  if (left) q.push(left);
  if (right) q.push(right);
  int depth = 0, numNodes = 0, numLeaves = 0, numObjs = 0;
  while (!q.empty()) {
    depth++;
    int tempSize = q.size();
    for (int i = 0; i < tempSize; i++) {
      numNodes++;
      auto el = q.front();
      q.pop();
      if (el->left) q.push(el->left);
      if (el->right) q.push(el->right);
      if (!el->left && !el->right) {
        numObjs += el->objects.size();
        numLeaves++;
      }
    }
  }
  printf("[BVH] Max depth: %d\n", depth);
  printf("[BVH] Num nodes: %d\n", numNodes);
  printf("[BVH] Num leaves: %d\n", numLeaves);
  printf("[BVH] Num objects: %d\n", numObjs);
}

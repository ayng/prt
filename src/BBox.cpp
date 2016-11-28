#include <algorithm>
#include "BBox.h"

BBox::BBox() : min(INFINITY, INFINITY, INFINITY),
               max(-INFINITY, -INFINITY, -INFINITY)
{
}

BBox::BBox(Eigen::Vector3d _min, Eigen::Vector3d _max) : min(_min), max(_max)
{
}

bool BBox::isHitBy(const Ray& ray) {
  Eigen::Vector3d xMinIS = ray.point + ((min.x() - ray.point.x()) / ray.dir.x()) * ray.dir;
  Eigen::Vector3d xMaxIS = ray.point + ((max.x() - ray.point.x()) / ray.dir.x()) * ray.dir;
  Eigen::Vector3d yMinIS = ray.point + ((min.y() - ray.point.y()) / ray.dir.y()) * ray.dir;
  Eigen::Vector3d yMaxIS = ray.point + ((max.y() - ray.point.y()) / ray.dir.y()) * ray.dir;
  Eigen::Vector3d zMinIS = ray.point + ((min.z() - ray.point.z()) / ray.dir.z()) * ray.dir;
  Eigen::Vector3d zMaxIS = ray.point + ((max.z() - ray.point.z()) / ray.dir.z()) * ray.dir;

  bool xMinHit = min.y() < xMinIS.y() && xMinIS.y() < max.y()
              && min.z() < xMinIS.z() && xMinIS.z() < max.z();
  bool xMaxHit = min.y() < xMaxIS.y() && xMaxIS.y() < max.y()
              && min.z() < xMaxIS.z() && xMaxIS.z() < max.z();
  bool yMinHit = min.x() < yMinIS.x() && yMinIS.x() < max.x()
              && min.z() < yMinIS.z() && yMinIS.z() < max.z();
  bool yMaxHit = min.x() < yMaxIS.x() && yMaxIS.x() < max.x()
              && min.z() < yMaxIS.z() && yMaxIS.z() < max.z();
  bool zMinHit = min.x() < zMinIS.x() && zMinIS.x() < max.x()
              && min.y() < zMinIS.y() && zMinIS.y() < max.y();
  bool zMaxHit = min.x() < zMaxIS.x() && zMaxIS.x() < max.x()
              && min.y() < zMaxIS.y() && zMaxIS.y() < max.y();
  bool isHit = xMinHit || xMaxHit || yMinHit || yMaxHit || zMinHit || zMaxHit;

  return isHit;
  
}
void BBox::expand(BBox rhs)
{
  min.x() = std::min(min.x(), rhs.min.x());
  min.y() = std::min(min.y(), rhs.min.y());
  min.z() = std::min(min.z(), rhs.min.z());
  max.x() = std::max(max.x(), rhs.max.x());
  max.y() = std::max(max.y(), rhs.max.y());
  max.z() = std::max(max.z(), rhs.max.z());
}

Eigen::Vector3d BBox::centroid()
{
  return (min + max) * 0.5;
}

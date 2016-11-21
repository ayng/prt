#include "Ray.h"


bool Ray::isDefined () const {
  return !(std::isnan(point.x()) || std::isnan(point.y()) || std::isnan(point.z()));
}

Ray Ray::transform (Eigen::Transform<double,3,Eigen::Affine> transformation) const {
	Eigen::Vector3d xfPoint = transformation * point;
 	Eigen::Vector3d xfDir = transformation * (point + dir) - xfPoint;
  	return {xfPoint, xfDir};
}

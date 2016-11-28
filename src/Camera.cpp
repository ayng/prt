#include "Camera.h"

Camera::Camera () {}

Camera::Camera (const Eigen::Vector3d& _e, const Eigen::Vector3d& _bl, const Eigen::Vector3d& _br,
  const Eigen::Vector3d& _tl, const Eigen::Vector3d& _tr)
  : e(_e), bl(_bl), br(_br), tl(_tl), tr(_tr) {}

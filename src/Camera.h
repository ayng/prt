#ifndef CAMERA_H
#define CAMERA_H

#include <Eigen/Dense> 

class Camera {
  public:
    Eigen::Vector3d e, bl, br, tl, tr;
    Eigen::Transform<double,3, Eigen::Affine> transform;

    Camera ();
    Camera (const Eigen::Vector3d &_e, const Eigen::Vector3d &_bl, const Eigen::Vector3d &_br,
            const Eigen::Vector3d &_tl, const Eigen::Vector3d &_tr);
};
#endif

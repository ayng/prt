#include <cstdio>
#include <functional>
#include <memory>
#include <vector>
#include "Ray.h"
#include "Light.h"
#include "Geometry.h"


void dump (Eigen::Matrix4d mat)  {
  printf("[[ %.3f, %.3f, %.3f, %.3f ]\n", mat(0), mat(1), mat(2), mat(3));
  printf(" [ %.3f, %.3f, %.3f, %.3f ]\n", mat(4), mat(5), mat(6), mat(7));
  printf(" [ %.3f, %.3f, %.3f, %.3f ]\n", mat(8), mat(9), mat(10), mat(11));
  printf(" [ %.3f, %.3f, %.3f, %.3f ]]\n", mat(12), mat(13), mat(14), mat(15));
}

void dump(Eigen::Vector3d vec)  {
  printf("<%.3f, %.3f, %.3f>\n", vec.x(), vec.y(), vec.z());
}

void dump (Eigen::Vector4d vec)  {
  printf("< %.3f, %.3f, %.3f, %.3f >\n", vec.x(), vec.y(), vec.z(), vec.w());
}

int main() {
  {
    Eigen::Vector3d i (1, 1, 0);
    Eigen::Vector3d j (0, 1, 0);
    dump(i.cross(j));
  }
  {
    Material mat = {Color(1,1,1), Color(1,1,1), Color(1,1,1), 10, Color(1,1,1)};
    Eigen::Transform<double,3,Eigen::Affine> w2o(Eigen::Scaling(1.0,1.0,1.0));
    Eigen::Transform<double,3,Eigen::Affine> o2w(Eigen::Scaling(1.0,1.0,1.0));
    Triangle tri(Eigen::Vector3d(0,0,0), Eigen::Vector3d( 0,1,0), Eigen::Vector3d(1,0,0), mat, w2o, o2w);
    dump(tri.a);
  }
  {
    std::vector<std::reference_wrapper<Light>> lights;
    DirectionalLight dl(Eigen::Vector3d(1, 1, 1), Color(1, 1, 1));
    PointLight pl(Eigen::Vector3d(1, 1, 1), Color(1, 1, 1));
    lights.push_back(dl);
    lights.push_back(pl);
    dump(lights[0].get().dirToLight(Eigen::Vector3d(0, 0, 0)));
    dump(lights[1].get().dirToLight(Eigen::Vector3d(0, 0, 0)));
  }
  {
    std::vector<std::unique_ptr<Light>> lights;
    {
      lights.emplace_back(new DirectionalLight(Eigen::Vector3d(1, 1, 1), Color(1, 1, 1)));
    }
    dump(lights[0]->dirToLight(Eigen::Vector3d(0, 0, 0)));
  }
  {
    Eigen::Transform<double,3,Eigen::Affine> s(Eigen::Scaling(2.0, 3.0, 4.0));
    Eigen::Transform<double,3,Eigen::Affine> sInv(Eigen::Scaling(1.0/2.0, 1.0/3.0, 1.0/4.0));
    Eigen::Vector4d v(1, 1, 1, 1);
    dump(s * sInv * v);
  }
  {
    Eigen::Transform<double,3,Eigen::Affine> r(Eigen::AngleAxis<double>(M_PI / 4.0, Eigen::Vector3d::UnitX()));
    Eigen::Vector4d v(1, 1, 1, 1);
    dump(r * v);
  }
  {
    Eigen::Transform<double,3,Eigen::Affine> t(Eigen::Translation3d(1.0, 1.0, 1.0));
    Ray r = {Eigen::Vector3d(1, 1, 1), Eigen::Vector3d(1, 1, 1)};
    Ray rT = r.transform(t);
    dump(rT.point);
    dump(rT.dir);
  }
}

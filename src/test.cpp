#include <cstdio>
#include <functional>
#include <memory>
#include <vector>
#include "Matrix.h"
#include "Ray.h"
#include "Vector.h"
#include "Light.h"
#include "Geometry.h"

int main() {
  {
    Material mat = {Color(1,1,1), Color(1,1,1), Color(1,1,1), 10, Color(1,1,1)};
    Triangle tri(Vector3(0,0,0), Vector3(0,1,0), Vector3(1,0,0), mat, rotate(-45,0,0), rotate(45,0,0));
    tri.bbox.min.dump();
    tri.bbox.max.dump();
  }
  {
    BBox box1(Vector3(-1, -2, -3), Vector3(3, 2, 1));
    BBox box2(Vector3(-2, -1, 3), Vector3(5, 1, 0));
    std::vector<BBox> boxes;
    boxes.push_back(box1);
    boxes.push_back(box2);
    BBox bigBox(boxes);
    bigBox.min.dump();
    bigBox.max.dump();
  }
  {
    Vector3 i (1, 1, 0);
    Vector3 j (0, 1, 0);
    i.cross(j).dump();
  }
  {
    Material mat = {Color(1,1,1), Color(1,1,1), Color(1,1,1), 10, Color(1,1,1)};
    Triangle tri(Vector3(0,0,0), Vector3(0,1,0), Vector3(1,0,0), mat, scale(1,1,1), scale(1,1,1));
    tri.a.dump();
  }
  {
    std::vector<std::reference_wrapper<Light>> lights;
    DirectionalLight dl(Vector3(1, 1, 1), Color(1, 1, 1));
    PointLight pl(Vector3(1, 1, 1), Color(1, 1, 1));
    lights.push_back(dl);
    lights.push_back(pl);
    lights[0].get().dirToLight(Vector3(0, 0, 0)).dump();
    lights[1].get().dirToLight(Vector3(0, 0, 0)).dump();
  }
  {
    std::vector<std::unique_ptr<Light>> lights;
    {
      lights.emplace_back(new DirectionalLight(Vector3(1, 1, 1), Color(1, 1, 1)));
    }
    lights[0]->dirToLight(Vector3(0, 0, 0)).dump();
  }
  {
    Matrix4 s = scale(2, 3, 4);
    Matrix4 sInv = scale(1.0/2, 1.0/3, 1.0/4);
    Vector4 v(1, 1, 1, 1);
    s.dot(sInv).dot(v).dump();
  }
  {
    Matrix4 r = rotate(45, 0, 0);
    Vector4 v(1, 1, 1, 1);
    r.dot(v).dump();
  }
  {
    Matrix4 t = translate(1, 1, 1);
    Ray r = {Vector3(1, 1, 1), Vector3(1, 1, 1)};
    Ray rT = t.transform(r);
    rT.point.dump();
    rT.dir.dump();
  }
}

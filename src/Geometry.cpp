/** Copyright 2016 Alex Yang */
#include <cstdio>
#include "Geometry.h"

Geometry::Geometry() {}
Geometry::Geometry(Material mat, Eigen::Transform<double,3,Eigen::Affine> w2o, Eigen::Transform<double,3,Eigen::Affine> o2w)
  : worldToObject(w2o), objectToWorld(o2w) {
  material = mat;
}

Triangle::Triangle() {}
Triangle::Triangle(Eigen::Vector3d aa, Eigen::Vector3d bb, Eigen::Vector3d cc,
  Material mat,
  Eigen::Transform<double,3,Eigen::Affine> w2o,
  Eigen::Transform<double,3,Eigen::Affine> o2w)
  : a(aa), b(bb), c(cc), normal((bb - aa).cross(cc - aa).normalized()),
    Geometry(mat, w2o, o2w)
{
  // Transform triangle vertices to world space.
  Eigen::Vector3d aW = o2w * a;
  Eigen::Vector3d bW = o2w * b;
  Eigen::Vector3d cW = o2w * c;
  // Create bounding box.
  bbox.expand(BBox(aW, aW));
  bbox.expand(BBox(bW, bW));
  bbox.expand(BBox(cW, cW));
}
Ray Triangle::intersect(const Ray& ray) {
  Ray xfRay = ray.transform(worldToObject);

  // Determine t.
  double d = normal.dot(a);
  double t = (d - normal.dot(xfRay.point)) / normal.dot(xfRay.dir);
  if (t < 0) return {NAN_VECTOR, NAN_VECTOR};
  // Determine if intersection point is within the triangle.
  Eigen::Vector3d p = xfRay.point + t * xfRay.dir;
  Eigen::Vector3d pa = p - a;
  Eigen::Vector3d pb = p - b;
  Eigen::Vector3d pc = p - c;
  if ((b - a).cross(pa).dot(normal) > 0 &&
      (c - b).cross(pb).dot(normal) > 0 &&
      (a - c).cross(pc).dot(normal) > 0) {
    Eigen::Vector3d worldPoint = objectToWorld * p; 
    Eigen::Vector3d worldNormal = Eigen::Transform<double,3,Eigen::Affine>(worldToObject.matrix().transpose()) * normal; 
    return {worldPoint, worldNormal.normalized()};
  } else {
    return {NAN_VECTOR, NAN_VECTOR};
  }
}


Sphere::Sphere()
{
}

Sphere::Sphere(Eigen::Vector3d c, double r, Material mat,
  Eigen::Transform<double,3,Eigen::Affine> w2o,
  Eigen::Transform<double,3,Eigen::Affine> o2w)
  : center(c), radius(r), Geometry(mat, w2o, o2w)
{
  Eigen::Vector3d min(center.x() - radius, center.y() - radius, center.z() - radius);
  Eigen::Vector3d max(center.x() + radius, center.y() + radius, center.z() + radius);
  std::vector<Eigen::Vector3d> corners;
  corners.push_back(Eigen::Vector3d(min.x(), min.y(), min.z()));
  corners.push_back(Eigen::Vector3d(max.x(), min.y(), min.z()));
  corners.push_back(Eigen::Vector3d(min.x(), max.y(), min.z()));
  corners.push_back(Eigen::Vector3d(min.x(), min.y(), max.z()));
  corners.push_back(Eigen::Vector3d(min.x(), max.y(), max.z()));
  corners.push_back(Eigen::Vector3d(max.x(), min.y(), max.z()));
  corners.push_back(Eigen::Vector3d(max.x(), max.y(), min.z()));
  corners.push_back(Eigen::Vector3d(max.x(), max.y(), max.z()));

  for (int i = 0; i < corners.size(); i++) {
    Eigen::Vector3d xfCorner = o2w * corners[i];
    bbox.expand(BBox(xfCorner, xfCorner));
  }
}
Ray Sphere::intersect(const Ray& ray) {
  // Transform ray to the coordinate space of the sphere.
  Ray xfRay = ray.transform(worldToObject);
  xfRay.dir = xfRay.dir.normalized();

  // Calculate quadratic equation coefficients.
  Eigen::Vector3d x = xfRay.point - center;
  double a = xfRay.dir.dot(xfRay.dir);
  double b = 2 * x.dot(xfRay.dir);
  double c = x.dot(x) - radius*radius;

  // Calculate discriminant to determine number of solutions.
  double discriminant = b*b - 4*a*c;
  // If there are no real solutions, we just return undefined.
  if (discriminant < 0) return {NAN_VECTOR, NAN_VECTOR};

  // Plugging t back into R(t), we find the solutions.
  double tPlus = (-b + std::sqrt(discriminant)) / (2*a);
  double tMinus = (-b - std::sqrt(discriminant)) / (2*a);
  Eigen::Vector3d solnPlus = xfRay.point + tPlus * xfRay.dir;
  Eigen::Vector3d solnMinus = xfRay.point + tMinus * xfRay.dir;

  // We ensure the intersection is in front of the ray by checking that t > 0.
  Ray surfaceNormal;
  if (tPlus <= 0 && tMinus <= 0) {
    surfaceNormal = {NAN_VECTOR, NAN_VECTOR};
  } else if (tPlus <= 0 && tMinus > 0) {
    surfaceNormal = {solnMinus, (solnMinus - center)};
  } else if (tPlus > 0 && tMinus <= 0) {
    surfaceNormal = {solnPlus, (solnPlus - center)};
  } else {
    // The only other possibility is that both solutions have positive t-values.
    // We choose the solution that is closer to the ray's start point.
    double distancePlus = (solnPlus - xfRay.point).norm();
    double distanceMinus = (solnMinus - xfRay.point).norm();
    if (distancePlus < distanceMinus)
      surfaceNormal = {solnPlus, (solnPlus - center)};
    else
      surfaceNormal = {solnMinus, (solnMinus - center)};
  }
  Ray result = surfaceNormal.transform(objectToWorld);
  Eigen::Vector3d worldPoint = objectToWorld * surfaceNormal.point; 
  Eigen::Vector3d worldNormal = Eigen::Transform<double,3,Eigen::Affine>(worldToObject.matrix().transpose()) * surfaceNormal.dir; 
  Ray worldSurfaceNormal = {worldPoint, worldNormal};
  return worldSurfaceNormal;
}

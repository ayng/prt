/** Copyright 2016 Alex Yang */
#include <random>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <cmath>
#include <cstdio>

#include "Scene.h"
#include <pngwriter.h>
#include <Eigen/Dense> 

#include <omp.h>

Scene::Scene(int res, int aa, bool bvh, int nthr) {
  resolution = res;
  antialias = aa;
  bvhEnabled = bvh;

  omp_set_dynamic(0); // disable dynamic teams to force thread count.
  omp_set_num_threads(nthr);

  xfIn = Eigen::Scaling(1.0, 1.0, 1.0);
  xfOut = Eigen::Scaling(1.0, 1.0, 1.0);
  material =
    {Color(.5, .5, .5), Color(0, 0, 0), Color(0, 0, 0), 1, Color(0, 0, 0)};
}

int Scene::getWidth() {
  Eigen::Vector3d imagePlaneX = camera.br - camera.bl;
  double imagePlaneW = imagePlaneX.norm();
  return static_cast<int>(resolution * imagePlaneW);
}
int Scene::getHeight() {
  Eigen::Vector3d imagePlaneY = camera.tl - camera.bl;
  double imagePlaneH = imagePlaneY.norm();
  return static_cast<int>(resolution * imagePlaneH);
}

double toRadians (double d) { 
  return d * M_PI / 180.0; 
} 

void Scene::parseLine(std::string line) {
  if (line.empty()) return;
  std::istringstream iss(line);
  std::string prefix;
  iss >> prefix;

  if (prefix[0] == '#') return;

  if (prefix == "cam") {
    Eigen::Vector3d pts[5];
    for (int i = 0; i < 5; i++)
      iss >> pts[i].x() >> pts[i].y() >> pts[i].z();
    camera = Camera(pts[0], pts[1], pts[2], pts[3], pts[4]);
  } else if (prefix == "tri") {
    double x1, y1, z1, x2, y2, z2, x3, y3, z3;
    iss >> x1 >> y1 >> z1 >> x2 >> y2 >> z2 >> x3 >> y3 >> z3;
    objects.emplace_back(
      new Triangle(Eigen::Vector3d(x1, y1, z1), Eigen::Vector3d(x2, y2, z2),
                   Eigen::Vector3d(x3, y3, z3), material, xfIn, xfOut));
  } else if (prefix == "sph") {
    double cx, cy, cz, r;
    iss >> cx >> cy >> cz >> r;
    objects.emplace_back(
      new Sphere(Eigen::Vector3d(cx, cy, cz), r, material, xfIn, xfOut));
  } else if (prefix == "obj") {
    std::string filename;
    iss >> filename;
    parseObj(filename.substr(1, filename.size()-2));
  } else if (prefix == "xfz") {
    xfIn = Eigen::Scaling(1.0, 1.0, 1.0);
    xfOut = Eigen::Scaling(1.0, 1.0, 1.0);
  } else if (prefix == "xft") {
    double x, y, z;
    iss >> x >> y >> z;
    Eigen::Translation3d m(-x,-y,-z); 
    xfIn = m * xfIn;
    xfOut = xfOut * m.inverse(); 

  } else if (prefix == "xfr") {
    double x, y, z;
    iss >> x >> y >> z;
    Eigen::Vector3d axis (toRadians(-x),toRadians(-y),toRadians(-z)); 
    Eigen::Vector3d axisInverse = -1 * axis; 
    Eigen::Transform<double,3,Eigen::Affine> m (Eigen::AngleAxis<double>(axis.norm(), axis.normalized()));  
    xfIn = m * xfIn;
    xfOut = xfOut * m.inverse();
  } else if (prefix == "xfs") {
    double x, y, z;
    iss >> x >> y >> z;
    xfIn = Eigen::Scaling(1.0/x, 1.0/y, 1.0/z) * xfIn;
    xfOut = xfOut * Eigen::Scaling(x, y, z);
  } else if (prefix == "lta") {
    double r, g, b;
    iss >> r >> g >> b;
    ambientLight = Color(r, g, b);
  } else if (prefix == "ltp") {
    double x, y, z, r, g, b, falloff;
    iss >> x >> y >> z >> r >> g >> b >> falloff;
    lights.emplace_back(new PointLight(Eigen::Vector3d(x, y, z), Color(r, g, b)));
  } else if (prefix == "ltd") {
    double x, y, z, r, g, b;
    iss >> x >> y >> z >> r >> g >> b;
    lights.emplace_back(new DirectionalLight(Eigen::Vector3d(x, y, z), Color(r, g, b)));
  } else if (prefix == "mat") {
    double kar, kag, kab, kdr, kdg, kdb, ksr, ksg, ksb, ksp, krr, krg, krb;
    iss >> kar >> kag >> kab
        >> kdr >> kdg >> kdb
        >> ksr >> ksg >> ksb >> ksp
        >> krr >> krg >> krb;
    material = {
      Color(kar, kag, kab),
      Color(kdr, kdg, kdb),
      Color(ksr, ksg, ksb), ksp,
      Color(krr, krg, krb)
    };
  } else {
    std::cerr << "Warning: skipping unrecognized command \""
              << prefix
              << "\" in line:\n  > "
              << line
              << std::endl;
  }
}

void Scene::parseObj(std::string filename) {
  std::ifstream infile(filename);
  std::string line;
  std::vector<Eigen::Vector3d> vertices;
  int numVertices = 0;
  int numFaces = 0;
  while (std::getline(infile, line)) {
    std::string prefix;
    std::istringstream iss(line);
    iss >> prefix;
    if (prefix.compare("v") == 0) {
      double x, y, z;
      iss >> x >> y >> z;
      vertices.push_back(Eigen::Vector3d(x, y, z));
      numVertices++;
    } else if (prefix.compare("f") == 0) {
      int a, b, c;
      iss >> a >> b >> c;
      objects.emplace_back(
        new Triangle(vertices[a-1], vertices[b-1], vertices[c-1],
                     material, xfIn, xfOut));
      numFaces++;
    }
  }
  printf("[OBJ] \"%s\" contained %d vertices and %d faces.\n",
         filename.c_str(), numVertices, numFaces);
}

std::shared_ptr<BVHNode> Scene::generateBVH() {
  // Create the box that encompasses the entire scene.
  BBox bigBox;
  for (auto objPtr : objects) {
    bigBox.expand(objPtr->bbox);
  }

  // Initialize the root node using the constructed bbox.
  auto root = std::make_shared<BVHNode>(objects, bigBox);
  //root->objects = objects;
  //root->bbox = bigBox;

  // Add the root node to the stack of nodes to be partitioned.
  std::vector<std::shared_ptr<BVHNode>> toPart;
  toPart.push_back(root);

  // Partition nodes, adding resultant nodes onto the stack.
  // When a node refuses to partition, nothing is added to the stack.
  while (!toPart.empty()) {
    // Pop a node from the front of the queue.
    auto curPtr = toPart.back();
    toPart.pop_back();

    // Attempt to partition the node.
    bool isPartitioned = curPtr->partition();
    if (isPartitioned) {
      toPart.push_back(curPtr->left);
      toPart.push_back(curPtr->right);
    }
  }

  root->printStats();

  return root;
}

std::vector<Color> Scene::render() {
  // Determine pixel location from the image plane.
  Eigen::Vector3d imagePlaneY = camera.tl - camera.bl;
  Eigen::Vector3d imagePlaneX = camera.br - camera.bl;
  double imagePlaneH = imagePlaneY.norm();
  double imagePlaneW = imagePlaneX.norm();
  int height = static_cast<int>(resolution * imagePlaneH);
  int width = static_cast<int>(resolution * imagePlaneW);
  Eigen::Vector3d unitY = imagePlaneY.normalized();
  Eigen::Vector3d unitX = imagePlaneX.normalized();

  printf("[RENDER] Preparing %dx%d image.\n", width, height);

  // Initialize frame buffer.
  std::vector<Color> frame(width*height);

  // Generate BVH.
  if (bvhEnabled) {
    bvh = generateBVH();
  }

  printf("[RENDER] Rendering to image.\n");

  // Find rays from the pixel locations.
  double start = profiler.now();
#pragma omp parallel for collapse(2)
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      // Determine world coordinates of pixel at (x, y) of image plane.
      Eigen::Vector3d worldPixel = camera.bl
                                 + unitY * (static_cast<double>(y) / resolution)
                                 + unitX * (static_cast<double>(x) / resolution);
      std::vector<std::pair<double, double>> samples = jitteredGrid(antialias);
      for (std::pair<double, double> pt : samples) {
        Eigen::Vector3d worldPoint = worldPixel
                                   + unitX * (pt.first / resolution)
                                   + unitY * (pt.second / resolution);
        Eigen::Vector3d direction = worldPoint - camera.e;
        frame[y*width+x] = frame[y*width+x] + trace({camera.e, direction});
      }
      frame[y*width+x] = frame[y*width+x] * (1.0 / samples.size());
    }
  }
  printf("[RENDER] Completed in %.3f seconds.\n", profiler.now() - start);

  return frame;
}

std::vector<std::pair<double, double>> Scene::jitteredGrid(int size) {
  std::vector<std::pair<double, double>> res;
  if (size == 1) {
    res.push_back(std::pair<double, double>(0.5, 0.5));
    return res;
  }
  std::vector<double> increments;
  for (int i = 0; i < size; i++) {
    increments.push_back(static_cast<double>(i) / size);
  }
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      double jitterX = (static_cast<double>(std::rand()) / RAND_MAX) / size;
      double jitterY = (static_cast<double>(std::rand()) / RAND_MAX) / size;
      double x = increments[i] + jitterX;
      double y = increments[j] + jitterY;
      res.push_back(std::pair<double, double>(x, y));
    }
  }
  return res;
}

Color Scene::trace(const Ray& ray) {
  return trace(ray, kBounceLimit);
}
Color Scene::trace(const Ray& ray, int bouncesLeft) {
  if (bouncesLeft < 0) {
    return kBackgroundColor;
  }

  std::pair<Ray, Material> nearest;
  if (bvhEnabled) {
    nearest = collideBVH(ray);
  } else {
    nearest = collide(ray, objects);
  }
  Ray nearestIntersection = nearest.first;
  Material nearestMaterial = nearest.second;

  if (!nearestIntersection.isDefined()) {
    return kBackgroundColor;
  }

  Color result;

  Eigen::Vector3d p = nearestIntersection.point;
  Eigen::Vector3d n = nearestIntersection.dir.normalized();
  Eigen::Vector3d v = (ray.point - p).normalized();
  Material mat = nearestMaterial;

  // Perform phong shading, using shadow rays to check for shadows.
  result = shade(p, n, v, mat);
  // Recursively trace reflective rays.
  if (mat.kr.r + mat.kr.g + mat.kr.b != 0) {
    Eigen::Vector3d reflectedDir = (2 * n) - v;
    result = result + mat.kr * trace({p, reflectedDir}, bouncesLeft-1);
  }
  return result;
}

std::pair<Ray, Material> Scene::collideBVH(const Ray& ray) {
  double nearestDistance = INFINITY;
  Ray nearestIntersection = {NAN_VECTOR, NAN_VECTOR};
  Material nearestMaterial;

  // Traverse the tree, iterating over any subtrees whose bounding box is hit
  // by the ray.
  std::vector<std::shared_ptr<BVHNode>> stack;
  stack.push_back(bvh);
  while (!stack.empty()) {
    auto nodePtr = stack.back();
    stack.pop_back();
    if (nodePtr->bbox.isHitBy(ray)) {
      if (nodePtr->left && nodePtr->left->bbox.isHitBy(ray))
        stack.push_back(nodePtr->left);
      if (nodePtr->right && nodePtr->right->bbox.isHitBy(ray))
        stack.push_back(nodePtr->right);
      if (!nodePtr->left && !nodePtr->right) {
        auto nearest = collide(ray, nodePtr->objects);
        double distance = (nearest.first.point - ray.point).norm();
        if (distance < nearestDistance) {
          nearestDistance = distance;
          nearestIntersection = nearest.first;
          nearestMaterial = nearest.second;
        }
      }
    }
  }
  
  std::pair<Ray, Material> nearest =
    std::make_pair(nearestIntersection, nearestMaterial);
  return nearest;
}
std::pair<Ray, Material> Scene::collide(const Ray& ray,
  const std::vector<std::shared_ptr<Geometry>>& objects)
{
  double nearestDistance = INFINITY;
  Ray nearestIntersection = {NAN_VECTOR, NAN_VECTOR};
  Material nearestMaterial;
  for (int i = 0; i < objects.size(); i++) {
    Geometry& geometry = *objects[i];
    Ray intersection = geometry.intersect(ray);
    if (intersection.isDefined()) {
      double distance = (intersection.point - ray.point).norm();
      if (distance < nearestDistance && distance > 1e-6) {
        nearestIntersection = intersection;
        nearestDistance = distance;
        nearestMaterial = geometry.material;
      }
    }
  }
  
  std::pair<Ray, Material> nearest =
    std::make_pair(nearestIntersection, nearestMaterial);
  return nearest;
}

Color Scene::shade(const Eigen::Vector3d& p, const Eigen::Vector3d& n,
  const Eigen::Vector3d& v, const Material& mat)
{
  Color result = ambient(mat.ka);
  for (int i = 0; i < lights.size(); i++) {
    Light& light = *lights[i];
    Eigen::Vector3d l = light.dirToLight(p);
    Ray shadowRay = {p, l};
    // Check if there are any intersections between this point and the light.
    bool isShadowed = false;

    std::pair<Ray, Material> nearest;
    if (bvhEnabled) {
      nearest = collideBVH(shadowRay);
    } else {
      nearest = collide(shadowRay, objects);
    }
    
    if (nearest.first.isDefined()) {
      double distanceToIntersection = (nearest.first.point - p).norm();
      // If the intersection lies between the light and the point,
      // skip shading for this light.
      // The second condition prevents self-shadowing.
      if (distanceToIntersection < light.distanceToLight(p) &&
          distanceToIntersection > 1e-6) {
        isShadowed = true;
      }
    }
      
    
    if (!isShadowed) {
      result = result + diffuse(p, n, l, mat.kd, light.intensity);
      result = result + specular(p, n, v, l, mat.ks, mat.sp, light.intensity);
    }
  }

  return result;
}

Color Scene::ambient(const Color& ka) {
  return ka * ambientLight;
}
Color Scene::diffuse(const Eigen::Vector3d& p, const Eigen::Vector3d& n, const Eigen::Vector3d& l,
  const Color& kd, const Color& intensity) {
  return kd * intensity * std::max(0.0, l.dot(n));
}
Color Scene::specular(const Eigen::Vector3d& p, const Eigen::Vector3d& n, const Eigen::Vector3d& v,
  const Eigen::Vector3d& l, const Color& ks, double sp, const Color& intensity) {
  return ks * intensity * specularIncidence(p, n, v, l, sp);
}
double Scene::specularIncidence(const Eigen::Vector3d& p, const Eigen::Vector3d& n,
  const Eigen::Vector3d& v, const Eigen::Vector3d& l, double sp) {
  Eigen::Vector3d r = (-1.0 * l + 2.0 * l.dot(n) * n).normalized();
  Eigen::Vector3d h = (l + v).normalized();
  Eigen::Vector3d hProj = (h - n * h.dot(n)).normalized();
  return std::pow(std::max(0.0, r.dot(v)), sp);
}

#ifndef MATRIX_H
#define MATRIX_H

#include <initializer_list>
#include "Ray.h"

class Matrix3 {
public:
  double v[9];
  Matrix3();
  Matrix3(const Vector3& a, const Vector3& b, const Vector3& c);
  double determinant();
};

class Matrix4 {
  public:
    double v[16]; // Row-major.
    Matrix4 ();
    Matrix4 (const Matrix4& orig);
    Matrix4 (std::initializer_list<double> list);
    Matrix4 dot (const Matrix4& rhs) const;
    Vector4 dot (const Vector4& rhs) const;
    Ray transform (const Ray& rhs) const;
    Matrix4 transposed () const;
    void dump () const;
};

Matrix4 scale(double x, double y, double z);
Matrix4 rotate(double x, double y, double z);
Matrix4 translate(double x, double y, double z);

#endif

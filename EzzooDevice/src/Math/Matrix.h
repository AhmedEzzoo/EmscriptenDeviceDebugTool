#pragma once

#include "Vector.h"

// #include "assimp/Importer.hpp"
// #include "assimp/postprocess.h"
// #include "assimp/scene.h"

//#include <math.h>

#include <glm/ext.hpp>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>


//#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Geometry>

#define ZERO_MEM(a) memset(a, 0, sizeof(a))
#define ANGLETORADIAN(x) (float)(((x) * M_PI / 180.0f))
#define ANGLETODEGREE(x) (float)(((x) * 180.0f / M_PI))

namespace Ezzoo {

struct PresProInfo;

struct PersProjInfo {
  float FOV = 0.0f;
  float Width = 0.0f;
  float Height = 0.0f;
  float zNear = 0.0f;
  float zFar = 0.0f;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct OrthoProjInfo {
  float r; // right
  float l; // left
  float b; // bottom
  float t; // top
  float n; // z near
  float f; // z far

  float Width;
  float Height;

  void Print() {
    printf("Left %f   Right %f\n", l, r);
    printf("Bottom %f Top %f\n", b, t);
    printf("Near %f   Far %f\n", n, f);
  }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Quaternion {
  float x = 0.0f, y = 0.0f, z = 0.0f, w = 0.0f;

  Quaternion() {}

  Quaternion(float Angle, const Vector3f &V);

  Quaternion(float _x, float _y, float _z, float _w);

  void Normalize();

  float Dot (const Quaternion& a, const Quaternion& b);

  Quaternion QuatSlerp (const Quaternion& a, const Quaternion& b, float t);

  Quaternion Conjugate() const;

  Vector3f ToDegrees();

  bool IsZero() const;
};

Quaternion operator*(const Quaternion &l, const Quaternion &r);

Quaternion operator*(const Quaternion &q, const Vector3f &v);

////////////////////////////////////////////////////////////////////////////////////////////////////////////







template <typename T, int Rows, int Cols>
class Matrix6f 
{
  public :
    Matrix6f()
    {
      SetZeros();
    }
    
    void FillElement (int row, int column, T val)
    {
      m_Matrix6(row, column) = val;
    }

    void Transpose()
    {
      m_Matrix6.transposeInPlace();
    }

    void Identity()
    {
      m_Matrix6.setIdentity();
    }

    void Inverse () 
    {
      m_Matrix6 = m_Matrix6.inverse();
    }

    void operator= (const Matrix6f& other)
    {
        m_Matrix6 = other.m_Matrix6;
    }

    Matrix6f& operator* (const Matrix6f& other)
    {
        m_Matrix6 = m_Matrix6 * other.m_Matrix6;
        return *this;
    }

     Matrix6f& operator*= (const Matrix6f& other)
    {
        m_Matrix6 *= other.m_Matrix6;
        return *this;
    }

    Matrix6f& operator+ (const Matrix6f& other)
    {
      m_Matrix6 = m_Matrix6 + other.m_Matrix6;
      return *this;
    }

    void Print() 
    {
      std::cout << m_Matrix6 << std::endl;
    }

    private :

      void SetZeros()
      {
        m_Matrix6.setZero();
      }


      
      private:
    Eigen::Matrix<T, Rows, Cols> m_Matrix6;

};




namespace Math {

    bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale);
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Matrix4f 
{
public:
  float m[4][4] = {0.0f};

  Matrix4f() {}

  Matrix4f(float a00, float a01, float a02, float a03, float a10, float a11,
           float a12, float a13, float a20, float a21, float a22, float a23,
           float a30, float a31, float a32, float a33) {
    m[0][0] = a00;
    m[0][1] = a01;
    m[0][2] = a02;
    m[0][3] = a03;
    m[1][0] = a10;
    m[1][1] = a11;
    m[1][2] = a12;
    m[1][3] = a13;
    m[2][0] = a20;
    m[2][1] = a21;
    m[2][2] = a22;
    m[2][3] = a23;
    m[3][0] = a30;
    m[3][1] = a31;
    m[3][2] = a32;
    m[3][3] = a33;
  }

  // constructor from Assimp matrix
  // Matrix4f(const aiMatrix4x4 &AssimpMatrix) {
  //   m[0][0] = AssimpMatrix.a1;
  //   m[0][1] = AssimpMatrix.a2;
  //   m[0][2] = AssimpMatrix.a3;
  //   m[0][3] = AssimpMatrix.a4;
  //   m[1][0] = AssimpMatrix.b1;
  //   m[1][1] = AssimpMatrix.b2;
  //   m[1][2] = AssimpMatrix.b3;
  //   m[1][3] = AssimpMatrix.b4;
  //   m[2][0] = AssimpMatrix.c1;
  //   m[2][1] = AssimpMatrix.c2;
  //   m[2][2] = AssimpMatrix.c3;
  //   m[2][3] = AssimpMatrix.c4;
  //   m[3][0] = AssimpMatrix.d1;
  //   m[3][1] = AssimpMatrix.d2;
  //   m[3][2] = AssimpMatrix.d3;
  //   m[3][3] = AssimpMatrix.d4;
  // }

  // Matrix4f(const aiMatrix3x3 &AssimpMatrix) {
  //   m[0][0] = AssimpMatrix.a1;
  //   m[0][1] = AssimpMatrix.a2;
  //   m[0][2] = AssimpMatrix.a3;
  //   m[0][3] = 0.0f;
  //   m[1][0] = AssimpMatrix.b1;
  //   m[1][1] = AssimpMatrix.b2;
  //   m[1][2] = AssimpMatrix.b3;
  //   m[1][3] = 0.0f;
  //   m[2][0] = AssimpMatrix.c1;
  //   m[2][1] = AssimpMatrix.c2;
  //   m[2][2] = AssimpMatrix.c3;
  //   m[2][3] = 0.0f;
  //   m[3][0] = 0.0f;
  //   m[3][1] = 0.0f;
  //   m[3][2] = 0.0f;
  //   m[3][3] = 1.0f;
  // }

  Matrix4f(const glm::mat4 &a) {
    m[0][0] = a[0][0];
    m[0][1] = a[1][0];
    m[0][2] = a[2][0];
    m[0][3] = a[3][0];
    m[1][0] = a[0][1];
    m[1][1] = a[1][1];
    m[1][2] = a[2][1];
    m[1][3] = a[3][1];
    m[2][0] = a[0][2];
    m[2][1] = a[1][2];
    m[2][2] = a[2][2];
    m[2][3] = a[3][2];
    m[3][0] = a[0][3];
    m[3][1] = a[1][3];
    m[3][2] = a[2][3];
    m[3][3] = a[3][3];
  }

  glm::mat4 ToGLM() 
  {
    glm::mat4 a;
    a[0][0] = m[0][0];
    a[1][0] = m[0][1];
    a[2][0] = m[0][2];
    a[3][0] = m[0][3];
    a[0][1] = m[1][0];
    a[1][1] = m[1][1];
    a[2][1] = m[1][2];
    a[3][1] = m[1][3];
    a[0][2] = m[2][0];
    a[1][2] = m[2][1];
    a[2][2] = m[2][2];
    a[3][2] = m[2][3];
    a[0][3] = m[3][0];
    a[1][3] = m[3][1];
    a[2][3] = m[3][2];
    a[3][3] = m[3][3];

    return a;
  }

  void SetZero() { ZERO_MEM(m); }

  Matrix4f Transpose() const 
  {
    Matrix4f n;

    for (unsigned int i = 0; i < 4; i++) {
      for (unsigned int j = 0; j < 4; j++) {
        n.m[i][j] = m[j][i];
      }
    }

    return n;
  }

  inline void InitIdentity() {
    m[0][0] = 1.0f;
    m[0][1] = 0.0f;
    m[0][2] = 0.0f;
    m[0][3] = 0.0f;
    m[1][0] = 0.0f;
    m[1][1] = 1.0f;
    m[1][2] = 0.0f;
    m[1][3] = 0.0f;
    m[2][0] = 0.0f;
    m[2][1] = 0.0f;
    m[2][2] = 1.0f;
    m[2][3] = 0.0f;
    m[3][0] = 0.0f;
    m[3][1] = 0.0f;
    m[3][2] = 0.0f;
    m[3][3] = 1.0f;
  }

  inline Matrix4f operator*(const Matrix4f &Right) const {
    Matrix4f Ret;

    for (unsigned int i = 0; i < 4; i++) {
      for (unsigned int j = 0; j < 4; j++) {
        Ret.m[i][j] = m[i][0] * Right.m[0][j] + m[i][1] * Right.m[1][j] +
                      m[i][2] * Right.m[2][j] + m[i][3] * Right.m[3][j];
      }
    }

    return Ret;
  }

  // const Matrix4f &operator=(const Matrix4f &mat) {
  //   memcpy(m, mat.m, sizeof(mat.m));
  //
  //  return *this;
  //}
  
  Vector4f operator*(const Vector4f &v) const {
    Vector4f r;

    r.x = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w;
    r.y = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w;
    r.z = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w;
    r.w = m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w;

    return r;
  }
  
  operator const float *() const { return &(m[0][0]); }
  
  const float *data() const { return &(m[0][0]); }

  void Print(uint8_t spaces = 0) const {

    for (int i = 0; i < 4; i++) {
      for (uint8_t x = 0; x < spaces; ++x) {
        printf(" ");
      }
      printf("%f %f %f %f\n", m[i][0], m[i][1], m[i][2], m[i][3]);
    }
  }


  float Determinant() const;

  Matrix4f Inverse() const;

  void InitScaleTransform(float ScaleX, float ScaleY, float ScaleZ);
  void InitScaleTransform(float Scale);
  void InitScaleTransform(const Vector3f &Scale);

  void InitRotateTransform(float RotateX, float RotateY, float RotateZ);
  void InitRotateTransformZYX(float RotateX, float RotateY, float RotateZ);
  void InitRotateTransform(const Vector3f &Rotate);
  void InitRotateTransform(const Quaternion &quat);
  void InitRotateTransform(const glm::quat &quat);
  void InitRotationFromDir(const Vector3f &Dir);

  void InitTranslationTransform(float x, float y, float z);
  void InitTranslationTransform(const Vector3f &Pos);

  void InitCameraTransform(const Vector3f &Target, const Vector3f &Up);
  void InitCameraTransform(const Vector3f &Pos, const Vector3f &Target,
                           const Vector3f &Up);

  void InitPersProjTransform(const PresProInfo &p);
  void InitOrthoProjTransform(const OrthoProjInfo &p);

  void CalcClipPlanes(Vector4f &l, Vector4f &r, Vector4f &b, Vector4f &t,
                      Vector4f &n, Vector4f &f) const;

private:
  void InitRotationX(float RotateX);
  void InitRotationY(float RotateY);
  void InitRotationZ(float RotateZ);
};

class Matrix3f {
public:
  float m[3][3] = {0.0f};

  Matrix3f() {}

  // Initialize the matrix from the top left corner of the 4-by-4 matrix
  Matrix3f(const Matrix4f &a) {
    m[0][0] = a.m[0][0];
    m[0][1] = a.m[0][1];
    m[0][2] = a.m[0][2];
    m[1][0] = a.m[1][0];
    m[1][1] = a.m[1][1];
    m[1][2] = a.m[1][2];
    m[2][0] = a.m[2][0];
    m[2][1] = a.m[2][1];
    m[2][2] = a.m[2][2];
  }

  Vector3f operator*(const Vector3f &v) const {
    Vector3f r;

    r.x = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z;
    r.y = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z;
    r.z = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z;

    return r;
  }

  inline Matrix3f operator*(const Matrix3f &Right) const {
    Matrix3f Ret;

    for (unsigned int i = 0; i < 3; i++) {
      for (unsigned int j = 0; j < 3; j++) {
        Ret.m[i][j] = m[i][0] * Right.m[0][j] + m[i][1] * Right.m[1][j] +
                      m[i][2] * Right.m[2][j];
      }
    }

    return Ret;
  }

  Matrix3f Transpose() const {
    Matrix3f n;

    for (unsigned int i = 0; i < 3; i++) {
      for (unsigned int j = 0; j < 3; j++) {
        n.m[i][j] = m[j][i];
      }
    }

    return n;
  }

  void InitRotateTransform(float RotateX, float RotateY, float RotateZ);

  void Print() const {
    for (int i = 0; i < 3; i++) {
      printf("%f %f %f\n", m[i][0], m[i][1], m[i][2]);
    }
  }

private:
  void InitRotationX(float RotateX);
  void InitRotationY(float RotateY);
  void InitRotationZ(float RotateZ);
};

} // namespace Ezzoo

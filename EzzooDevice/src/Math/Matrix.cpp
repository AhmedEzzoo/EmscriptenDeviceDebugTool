#include "ezpch.h"

#include "Matrix.h"
#include "PresProjection.h"

namespace Ezzoo {

float Matrix4f::Determinant() const {
  return m[0][0] * m[1][1] * m[2][2] * m[3][3] -
         m[0][0] * m[1][1] * m[2][3] * m[3][2] +
         m[0][0] * m[1][2] * m[2][3] * m[3][1] -
         m[0][0] * m[1][2] * m[2][1] * m[3][3] +
         m[0][0] * m[1][3] * m[2][1] * m[3][2] -
         m[0][0] * m[1][3] * m[2][2] * m[3][1] -
         m[0][1] * m[1][2] * m[2][3] * m[3][0] +
         m[0][1] * m[1][2] * m[2][0] * m[3][3] -
         m[0][1] * m[1][3] * m[2][0] * m[3][2] +
         m[0][1] * m[1][3] * m[2][2] * m[3][0] -
         m[0][1] * m[1][0] * m[2][2] * m[3][3] +
         m[0][1] * m[1][0] * m[2][3] * m[3][2] +
         m[0][2] * m[1][3] * m[2][0] * m[3][1] -
         m[0][2] * m[1][3] * m[2][1] * m[3][0] +
         m[0][2] * m[1][0] * m[2][1] * m[3][3] -
         m[0][2] * m[1][0] * m[2][3] * m[3][1] +
         m[0][2] * m[1][1] * m[2][3] * m[3][0] -
         m[0][2] * m[1][1] * m[2][0] * m[3][3] -
         m[0][3] * m[1][0] * m[2][1] * m[3][2] +
         m[0][3] * m[1][0] * m[2][2] * m[3][1] -
         m[0][3] * m[1][1] * m[2][2] * m[3][0] +
         m[0][3] * m[1][1] * m[2][0] * m[3][2] -
         m[0][3] * m[1][2] * m[2][0] * m[3][1] +
         m[0][3] * m[1][2] * m[2][1] * m[3][0];
}

Matrix4f Matrix4f::Inverse() const {
  // Compute the reciprocal determinant
  float det = Determinant();

  if (det == 0.0f) {
    assert(0);
    return *this;
  }

  float invdet = 1.0f / det;

  Matrix4f res;
  res.m[0][0] = invdet * (m[1][1] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) +
                          m[1][2] * (m[2][3] * m[3][1] - m[2][1] * m[3][3]) +
                          m[1][3] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]));
  res.m[0][1] = -invdet * (m[0][1] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) +
                           m[0][2] * (m[2][3] * m[3][1] - m[2][1] * m[3][3]) +
                           m[0][3] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]));
  res.m[0][2] = invdet * (m[0][1] * (m[1][2] * m[3][3] - m[1][3] * m[3][2]) +
                          m[0][2] * (m[1][3] * m[3][1] - m[1][1] * m[3][3]) +
                          m[0][3] * (m[1][1] * m[3][2] - m[1][2] * m[3][1]));
  res.m[0][3] = -invdet * (m[0][1] * (m[1][2] * m[2][3] - m[1][3] * m[2][2]) +
                           m[0][2] * (m[1][3] * m[2][1] - m[1][1] * m[2][3]) +
                           m[0][3] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]));
  res.m[1][0] = -invdet * (m[1][0] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) +
                           m[1][2] * (m[2][3] * m[3][0] - m[2][0] * m[3][3]) +
                           m[1][3] * (m[2][0] * m[3][2] - m[2][2] * m[3][0]));
  res.m[1][1] = invdet * (m[0][0] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) +
                          m[0][2] * (m[2][3] * m[3][0] - m[2][0] * m[3][3]) +
                          m[0][3] * (m[2][0] * m[3][2] - m[2][2] * m[3][0]));
  res.m[1][2] = -invdet * (m[0][0] * (m[1][2] * m[3][3] - m[1][3] * m[3][2]) +
                           m[0][2] * (m[1][3] * m[3][0] - m[1][0] * m[3][3]) +
                           m[0][3] * (m[1][0] * m[3][2] - m[1][2] * m[3][0]));
  res.m[1][3] = invdet * (m[0][0] * (m[1][2] * m[2][3] - m[1][3] * m[2][2]) +
                          m[0][2] * (m[1][3] * m[2][0] - m[1][0] * m[2][3]) +
                          m[0][3] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]));
  res.m[2][0] = invdet * (m[1][0] * (m[2][1] * m[3][3] - m[2][3] * m[3][1]) +
                          m[1][1] * (m[2][3] * m[3][0] - m[2][0] * m[3][3]) +
                          m[1][3] * (m[2][0] * m[3][1] - m[2][1] * m[3][0]));
  res.m[2][1] = -invdet * (m[0][0] * (m[2][1] * m[3][3] - m[2][3] * m[3][1]) +
                           m[0][1] * (m[2][3] * m[3][0] - m[2][0] * m[3][3]) +
                           m[0][3] * (m[2][0] * m[3][1] - m[2][1] * m[3][0]));
  res.m[2][2] = invdet * (m[0][0] * (m[1][1] * m[3][3] - m[1][3] * m[3][1]) +
                          m[0][1] * (m[1][3] * m[3][0] - m[1][0] * m[3][3]) +
                          m[0][3] * (m[1][0] * m[3][1] - m[1][1] * m[3][0]));
  res.m[2][3] = -invdet * (m[0][0] * (m[1][1] * m[2][3] - m[1][3] * m[2][1]) +
                           m[0][1] * (m[1][3] * m[2][0] - m[1][0] * m[2][3]) +
                           m[0][3] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]));
  res.m[3][0] = -invdet * (m[1][0] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]) +
                           m[1][1] * (m[2][2] * m[3][0] - m[2][0] * m[3][2]) +
                           m[1][2] * (m[2][0] * m[3][1] - m[2][1] * m[3][0]));
  res.m[3][1] = invdet * (m[0][0] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]) +
                          m[0][1] * (m[2][2] * m[3][0] - m[2][0] * m[3][2]) +
                          m[0][2] * (m[2][0] * m[3][1] - m[2][1] * m[3][0]));
  res.m[3][2] = -invdet * (m[0][0] * (m[1][1] * m[3][2] - m[1][2] * m[3][1]) +
                           m[0][1] * (m[1][2] * m[3][0] - m[1][0] * m[3][2]) +
                           m[0][2] * (m[1][0] * m[3][1] - m[1][1] * m[3][0]));
  res.m[3][3] = invdet * (m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) +
                          m[0][1] * (m[1][2] * m[2][0] - m[1][0] * m[2][2]) +
                          m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]));
  return res;
}

void Matrix4f::InitScaleTransform(float ScaleX, float ScaleY, float ScaleZ) {
  m[0][0] = ScaleX;
  m[0][1] = 0.0f;
  m[0][2] = 0.0f;
  m[0][3] = 0.0f;
  m[1][0] = 0.0f;
  m[1][1] = ScaleY;
  m[1][2] = 0.0f;
  m[1][3] = 0.0f;
  m[2][0] = 0.0f;
  m[2][1] = 0.0f;
  m[2][2] = ScaleZ;
  m[2][3] = 0.0f;
  m[3][0] = 0.0f;
  m[3][1] = 0.0f;
  m[3][2] = 0.0f;
  m[3][3] = 1.0f;
}
void Matrix4f::InitScaleTransform(float Scale) {
  m[0][0] = Scale;
  m[0][1] = 0.0f;
  m[0][2] = 0.0f;
  m[0][3] = 0.0f;
  m[1][0] = 0.0f;
  m[1][1] = Scale;
  m[1][2] = 0.0f;
  m[1][3] = 0.0f;
  m[2][0] = 0.0f;
  m[2][1] = 0.0f;
  m[2][2] = Scale;
  m[2][3] = 0.0f;
  m[3][0] = 0.0f;
  m[3][1] = 0.0f;
  m[3][2] = 0.0f;
  m[3][3] = 1.0f;
}
void Matrix4f::InitScaleTransform(const Vector3f &Scale) {

  InitScaleTransform(Scale.x, Scale.y, Scale.z);
}

void Matrix4f::InitRotateTransform(float RotateX, float RotateY,
                                   float RotateZ) {
  Matrix4f rx, ry, rz;

  float x = ANGLETORADIAN(RotateX);
  float y = ANGLETORADIAN(RotateY);
  float z = ANGLETORADIAN(RotateZ);

  rx.InitRotationX(x);
  ry.InitRotationY(y);
  rz.InitRotationZ(z);

  *this = rx * ry * rz;
}
void Matrix4f::InitRotateTransformZYX(float RotateX, float RotateY,
                                      float RotateZ) {
  Matrix4f rx, ry, rz;

  float x = ANGLETORADIAN(RotateX);
  float y = ANGLETORADIAN(RotateY);
  float z = ANGLETORADIAN(RotateZ);

  rx.InitRotationX(x);
  ry.InitRotationY(y);
  rz.InitRotationZ(z);

  *this = rx * ry * rz;
}
void Matrix4f::InitRotateTransform(const Vector3f &Rotate) {
  InitRotateTransform(Rotate.x, Rotate.y, Rotate.z);
}
void Matrix4f::InitRotateTransform(const Quaternion &quat) {}
void Matrix4f::InitRotateTransform(const glm::quat &quat) {}
void Matrix4f::InitRotationFromDir(const Vector3f &Dir) {
  Vector3f Up(0.0f, 1.0f, 0.0f);
  InitCameraTransform(Dir, Up);
}

void Matrix4f::InitTranslationTransform(float x, float y, float z) {
  m[0][0] = 1.0f;
  m[0][1] = 0.0f;
  m[0][2] = 0.0f;
  m[0][3] = x;
  m[1][0] = 0.0f;
  m[1][1] = 1.0f;
  m[1][2] = 0.0f;
  m[1][3] = y;
  m[2][0] = 0.0f;
  m[2][1] = 0.0f;
  m[2][2] = 1.0f;
  m[2][3] = z;
  m[3][0] = 0.0f;
  m[3][1] = 0.0f;
  m[3][2] = 0.0f;
  m[3][3] = 1.0f;
}
void Matrix4f::InitTranslationTransform(const Vector3f &Pos) {
  InitTranslationTransform(Pos.x, Pos.y, Pos.z);
}

void Matrix4f::InitCameraTransform(const Vector3f &Target, const Vector3f &Up) {
  Vector3f N = Target;
  N.Normalize();

  Vector3f UpNorm = Up;
  UpNorm.Normalize();

  Vector3f U;
  U = UpNorm.Cross(N);
  U.Normalize();

  Vector3f V = N.Cross(U);

  m[0][0] = U.x;
  m[0][1] = U.y;
  m[0][2] = U.z;
  m[0][3] = 0.0f;
  m[1][0] = V.x;
  m[1][1] = V.y;
  m[1][2] = V.z;
  m[1][3] = 0.0f;
  m[2][0] = N.x;
  m[2][1] = N.y;
  m[2][2] = N.z;
  m[2][3] = 0.0f;
  m[3][0] = 0.0f;
  m[3][1] = 0.0f;
  m[3][2] = 0.0f;
  m[3][3] = 1.0f;
}

void Matrix4f::InitCameraTransform(const Vector3f &Pos, const Vector3f &Target,
                                   const Vector3f &Up) {
  Matrix4f cameraTranslate;
  cameraTranslate.InitTranslationTransform(-Pos.x, -Pos.y, -Pos.z);

  Matrix4f cameraRotateTransform;
  cameraRotateTransform.InitCameraTransform(Target, Up);

  *this = cameraRotateTransform * cameraTranslate;
}

void Matrix4f::InitPersProjTransform(const PresProInfo &p) {
  m[0][0] = (p.D / p.AR);
  m[0][1] = 0.0f;
  m[0][2] = 0.0f;
  m[0][3] = 0.0f;
  m[1][0] = 0.0f;
  m[1][1] = p.D;
  m[1][2] = 0.0f;
  m[1][3] = 0.0f;
  m[2][0] = 0.0f;
  m[2][1] = 0.0f;
  m[2][2] = p.A;
  m[2][3] = p.B;
  m[3][0] = 0.0f;
  m[3][1] = 0.0f;
  m[3][2] = 1.0f;
  m[3][3] = 0.0f;
}

void Matrix4f::InitOrthoProjTransform(const OrthoProjInfo &p) {}

void Matrix4f::CalcClipPlanes(Vector4f &l, Vector4f &r, Vector4f &b,
                              Vector4f &t, Vector4f &n, Vector4f &f) const {}

void Matrix4f::InitRotationX(float RotateX) {
  m[0][0] = 1.0f;
  m[0][1] = 0.0f;
  m[0][2] = 0.0f;
  m[0][3] = 0.0f;
  m[1][0] = 0.0f;
  m[1][1] = cosf(RotateX);
  m[1][2] = sinf(RotateX);
  m[1][3] = 0.0f;
  m[2][0] = 0.0f;
  m[2][1] = -sinf(RotateX);
  m[2][2] = cosf(RotateX);
  m[2][3] = 0.0f;
  m[3][0] = 0.0f;
  m[3][1] = 0.0f;
  m[3][2] = 0.0f;
  m[3][3] = 1.0f;
}
void Matrix4f::InitRotationY(float RotateY) {
  m[0][0] = cosf(RotateY);
  m[0][1] = 0.0f;
  m[0][2] = -sinf(RotateY);
  m[0][3] = 0.0f;
  m[1][0] = 0.0f;
  m[1][1] = 1.0f;
  m[1][2] = 0.0f;
  m[1][3] = 0.0f;
  m[2][0] = sinf(RotateY);
  m[2][1] = 0.0f;
  m[2][2] = cosf(RotateY);
  m[2][3] = 0.0f;
  m[3][0] = 0.0f;
  m[3][1] = 0.0f;
  m[3][2] = 0.0f;
  m[3][3] = 1.0f;
}
void Matrix4f::InitRotationZ(float RotateZ) {
  m[0][0] = cosf(RotateZ);
  m[0][1] = sinf(RotateZ);
  m[0][2] = 0.0f;
  m[0][3] = 0.0f;
  m[1][0] = -sinf(RotateZ);
  m[1][1] = cosf(RotateZ);
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







// Quaternion

Quaternion::Quaternion(float Angle, const Vector3f &V) {
  float HalfAngleInRadians = ANGLETORADIAN(Angle / 2);

  float SineHalfAngle = sinf(HalfAngleInRadians);
  float CosHalfAngle = cosf(HalfAngleInRadians);

  x = V.x * SineHalfAngle;
  y = V.y * SineHalfAngle;
  z = V.z * SineHalfAngle;
  w = CosHalfAngle;
}

Quaternion::Quaternion(float _x, float _y, float _z, float _w) {
  x = _x;
  y = _y;
  z = _z;
  w = _w;
}

void Quaternion::Normalize() {
  float length = sqrtf(x * x + y * y + z * z + w * w);

  if (length < 1e-12) 
  {
    x = 0.0f;
    y = 0.0f;
    z = 0.0f;
    w = 1.0f;

  }

  x /= length;
  y /= length;
  z /= length;
  w /= length;
}

Quaternion Quaternion::Conjugate() const {
  Quaternion ret(-x, -y, -z, w);
  return ret;
}

Quaternion Quaternion::QuatSlerp (const Quaternion& a, const Quaternion& b, float t)
{
  float dot =  Dot(a, b);

  Quaternion bb = b;
  if (dot < 0.0f)
  {
    dot = -dot;
    bb.w = -b.w; bb.x = -b.x; bb.y = -b.y; bb.z = -b.z;
  }

  const float DOT_THERSHOLD = 0.995;
  if (dot > DOT_THERSHOLD)
  {
    Quaternion res = Quaternion(a.x + t*(bb.x - a.x), a.y + t*(bb.y - a.y), a.z + t*(bb.z - a.z), a.w + t*(bb.w - a.w) );
    res.Normalize();
    return res; 
  } 

  float theta0 = acos(dot);
  float theta = theta0 * t;
  float sinTheta = sin(theta);
  float sinTheta0 = sin(theta0);
  float s0 = cos(theta) - dot * (sinTheta / sinTheta0);
  float s1 = sinTheta / sinTheta0;

  Quaternion res = Quaternion(a.x * s0 + bb.x * s1, a.y * s0 + bb.y * s1, a.z * s0 + bb.z * s1, a.w * s0 + bb.w * s1);
  res.Normalize();

  return res;
}


Vector3f Quaternion::ToDegrees() {
  float f[3];

  f[0] = atan2(x * z + y * w, x * w - y * z);
  f[1] = acos(-x * x - y * y - z * z - w * w);
  f[2] = atan2(x * z - y * w, x * w + y * z);

  f[0] = ANGLETODEGREE(f[0]);
  f[1] = ANGLETODEGREE(f[1]);
  f[2] = ANGLETODEGREE(f[2]);

  return Vector3f(f);
}

bool Quaternion::IsZero() const { return false; }

float Quaternion::Dot (const Quaternion& a,const Quaternion& b)
{
  float dot = a.x * b.x  + a.y * b.y + a.z * b.z + a.w * b.w; 
  return dot;
}

Quaternion operator*(const Quaternion &l, const Quaternion &r) {
  float w = (l.w * r.w) - (l.x * r.x) - (l.y * r.y) - (l.z * r.z);
  float x = (l.x * r.w) + (l.w * r.x) + (l.y * r.z) - (l.z * r.y);
  float y = (l.y * r.w) + (l.w * r.y) + (l.z * r.x) - (l.x * r.z);
  float z = (l.z * r.w) + (l.w * r.z) + (l.x * r.y) - (l.y * r.x);

  Quaternion ret(x, y, z, w);

  return ret;
}



Quaternion operator*(const Quaternion &q, const Vector3f &v) {
  float w = -(q.x * v.x) - (q.y * v.y) - (q.z * v.z);
  float x = (q.w * v.x) + (q.y * v.z) - (q.z * v.y);
  float y = (q.w * v.y) + (q.z * v.x) - (q.x * v.z);
  float z = (q.w * v.z) + (q.x * v.y) - (q.y * v.x);

  Quaternion ret(x, y, z, w);

  return ret;
}








void Matrix3f::InitRotateTransform(float RotateX, float RotateY,
                                   float RotateZ) {
  Matrix3f rx, ry, rz;

  float x = ANGLETORADIAN(RotateX);
  float y = ANGLETORADIAN(RotateY);
  float z = ANGLETORADIAN(RotateZ);

  rx.InitRotationX(x);
  ry.InitRotationY(y);
  rz.InitRotationZ(z);

  *this = rx * ry * rz;
}

void Matrix3f::InitRotationX(float RotateX) {
  m[0][0] = 1.0f;
  m[0][1] = 0.0f;
  m[0][2] = 0.0f;
  m[1][0] = 0.0f;
  m[1][1] = cosf(RotateX);
  m[1][2] = sinf(RotateX);
  m[2][0] = 0.0f;
  m[2][1] = -sinf(RotateX);
  m[2][2] = cosf(RotateX);
}
void Matrix3f::InitRotationY(float RotateY) {
  m[0][0] = cosf(RotateY);
  m[0][1] = 0.0f;
  m[0][2] = -sinf(RotateY);
  m[1][0] = 0.0f;
  m[1][1] = 1.0f;
  m[1][2] = 0.0f;
  m[2][0] = sinf(RotateY);
  m[2][1] = 0.0f;
  m[2][2] = cosf(RotateY);
}
void Matrix3f::InitRotationZ(float RotateZ) {
  m[0][0] = cosf(RotateZ);
  m[0][1] = sinf(RotateZ);
  m[0][2] = 0.0f;
  m[1][0] = -sinf(RotateZ);
  m[1][1] = cosf(RotateZ);
  m[1][2] = 0.0f;
  m[2][0] = 0.0f;
  m[2][1] = 0.0f;
  m[2][2] = 0.0f;
}


namespace Math {

  bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale)
  {
      // From glm::decompose in matrix_decompose.inl

		using namespace glm;
		using T = float;

		mat4 LocalMatrix(transform);

		// Normalize the matrix.
		if (epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), epsilon<T>()))
			return false;

		// First, isolate perspective.  This is the messiest.
		if (
			epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), epsilon<T>()) ||
			epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), epsilon<T>()) ||
			epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), epsilon<T>()))
		{
			// Clear the perspective partition
			LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
			LocalMatrix[3][3] = static_cast<T>(1);
		}

		// Next take care of translation (easy).
		translation = vec3(LocalMatrix[3]);
		LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);

		vec3 Row[3], Pdum3;

		// Now get scale and shear.
		for (length_t i = 0; i < 3; ++i)
			for (length_t j = 0; j < 3; ++j)
				Row[i][j] = LocalMatrix[i][j];

		// Compute X scale factor and normalize first row.
		scale.x = length(Row[0]);
		Row[0] = detail::scale(Row[0], static_cast<T>(1));
		scale.y = length(Row[1]);
		Row[1] = detail::scale(Row[1], static_cast<T>(1));
		scale.z = length(Row[2]);
		Row[2] = detail::scale(Row[2], static_cast<T>(1));

		// At this point, the matrix (in rows[]) is orthonormal.
		// Check for a coordinate system flip.  If the determinant
		// is -1, then negate the matrix and the scaling factors.
		#if 0
		Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
		if (dot(Row[0], Pdum3) < 0)
		{
			for (length_t i = 0; i < 3; i++)
			{
				scale[i] *= static_cast<T>(-1);
				Row[i] *= static_cast<T>(-1);
			}
		}
		#endif

		rotation.y = asin(-Row[0][2]);
		if (cos(rotation.y) != 0)
		{
			rotation.x = atan2(Row[1][2], Row[2][2]);
			rotation.z = atan2(Row[0][1], Row[0][0]);
		}
		else
		{
			rotation.x = atan2(-Row[2][0], Row[1][1]);
			rotation.z = 0;
		}

		return true;

	}
}


} // namespace Ezzoo
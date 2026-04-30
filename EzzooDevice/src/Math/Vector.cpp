#include "ezpch.h"

#include "Matrix.h"

namespace Ezzoo {

void Vector3f::Rotate(float Angle, const Vector3f &V) 
{
  Quaternion RotationQ(Angle, V);

  Quaternion ConjugateQ = RotationQ.Conjugate();

  Quaternion W = RotationQ * (*this) * ConjugateQ;

  x = W.x;
  y = W.y;
  z = W.z;
}
} // namespace Ezzoo
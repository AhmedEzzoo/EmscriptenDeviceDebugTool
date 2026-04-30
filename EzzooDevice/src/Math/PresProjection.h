#pragma once

#include "Matrix.h"
namespace Ezzoo {

struct PresProInfo {
  float D = 0.0f;
  float AR = 0.0f;
  float A = 0.0f;
  float B = 0.0f;
};

class PresProjection {
public:
  PresProjection();

  void SetPresProjection(float d, float ar, float a, float b);
  Matrix4f &GetPresProjection();

private:
  Matrix4f m_PresProjec;

  PresProInfo m_ProInfo;
};

} // namespace Ezzoo
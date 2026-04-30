#include "ezpch.h"

#include "PresProjection.h"
namespace Ezzoo {

PresProjection::PresProjection() {}

void PresProjection::SetPresProjection(float d, float ar, float a, float b) {
  m_ProInfo.D = d;
  m_ProInfo.AR = ar;
  m_ProInfo.A = a;
  m_ProInfo.B = b;
}

Matrix4f &PresProjection::GetPresProjection() {
  m_PresProjec.InitPersProjTransform(m_ProInfo);

  return m_PresProjec;
}

} // namespace Ezzoo
#include "ezpch.h"

// #include "Matrix.h"
#include "WorldTransform.h"

namespace Ezzoo {

WorldTransform::WorldTransform() {}

void WorldTransform::SetTranslate(float x, float y, float z) {
  m_Translate.x = x;
  m_Translate.y = y;
  m_Translate.z = z;
}
void WorldTransform::SetScale(float x, float y, float z) {
  m_Scale.x = x;
  m_Scale.y = y;
  m_Scale.z = z;
}
void WorldTransform::SetRotate(float x, float y, float z) {
  m_Rotate.x = x;
  m_Rotate.y = y;
  m_Rotate.z = z;
}
void WorldTransform::Rotate(float scale) {}

Matrix4f WorldTransform::GetTransform() {
  Matrix4f scale;
  scale.InitScaleTransform(m_Scale.x, m_Scale.y, m_Scale.z);

  Matrix4f translate;
  translate.InitTranslationTransform(m_Translate);

  Matrix4f rotate;
  rotate.InitRotateTransform(-m_Rotate.x, -m_Rotate.y, -m_Rotate.z);

  m_FinalMat = translate * rotate * scale;

  return m_FinalMat;
}

Vector3f WorldTransform::WorldPosToLocalPos(const Vector3f &worldPos) {
  Matrix4f reversedTranslation;
  reversedTranslation.InitTranslationTransform(m_Translate.Negate());

  Matrix4f reversedRotation;
  reversedRotation.InitRotateTransformZYX(-m_Rotate.x, -m_Rotate.y,
                                          -m_Rotate.z);

  Matrix4f newWorldTransform = reversedRotation * reversedTranslation;

  Vector4f pos = Vector4f(worldPos, 1.0f);

  Vector4f localPos4F = newWorldTransform * pos;
  Vector3f localPosition3F = Vector3f(localPos4F);

  return localPosition3F;
}

} // namespace Ezzoo
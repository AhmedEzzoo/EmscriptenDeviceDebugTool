#pragma once

#include "Matrix.h"

namespace Ezzoo {

class WorldTransform {
public:
  WorldTransform();

  Matrix4f GetTransform();
  void SetRotate(float x, float y, float z);
  void SetTranslate(float x, float y, float z);
  void SetScale(float x, float y, float z);

  void Rotate(float scale);


  const Vector3f &GetRotate() const { return m_Rotate; }
  const Vector3f &GetTranslate() const { return m_Translate; }
  const Vector3f &GetScale() const { return m_Scale;} 


  Vector3f WorldPosToLocalPos(const Vector3f &worldPos);

private:
  Vector3f m_Translate;
  Vector3f m_Rotate;
  Vector3f m_Scale;

  Matrix4f m_FinalMat;
};

} // namespace Ezzoo
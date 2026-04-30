#pragma once

#include "Math/Matrix.h"
#include "Math/Vector.h"

//#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>

namespace Ezzoo {

class Camera {
public:
  Camera() = default;
  Camera(float width, float height);

  Camera(float windowWidth, float windowHeight, const Vector3f &pos, const Vector3f &target, const Vector3f &up);
  ~Camera() = default;

  Matrix4f &CalculateViewMatrix();

  Matrix4f &GetViewMatrix () { return m_View;}

  const Vector3f &GetPosition() const;
  const Vector3f &GetDirection() const;

  void OnUpdate(float ts = 0.0f);
  // void OnEvent(Event &e);

  const Matrix4f &GetView() const { return m_View; }
 
  void SetPosition(const Vector3f &pos);

  void SetViewportSize (uint32_t width, uint32_t height);

  glm::mat4 m_Projection;

private:
  void KeyControls();
  void MouseControl(float xChange, float yChange);
  // bool OnMouseScrolledEvent(MouseScrolledEvent &event);
  // bool OnKeyPressedEvent(KeyPressedEvent &event);
  void Update();
  void Init();

private:
  Vector3f m_Position;
  Vector3f m_Up;
  Vector3f m_Target;
  float m_VAngle = 0.0f;
  float m_HAngle = 0.0f;
  float m_MoveSpeed = 5.0f;

  float m_WindowWidth;
  float m_WindowHeight;

  Matrix4f m_View;

  float m_TimeStep = 0.0f;

  bool m_LeftAlt = false;

  bool m_OnUpperEdge = false;
  bool m_OnLowerEdge = false;
  bool m_OnLeftEdge = false;
  bool m_OnRightEdge = false;
  Vector2f m_MousePos = Vector2f{0.0f, 0.0f};
};
} // namespace Ezzoo
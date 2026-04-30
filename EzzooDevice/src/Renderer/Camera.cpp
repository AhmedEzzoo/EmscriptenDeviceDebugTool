#include "ezpch.h"

#include "Core/Application.h"
#include "Camera.h"
#include "Platform/Windows/WindowsWindow.h"
#include "Core/Input.h"

#define EDGE_STEP 1.0f
namespace Ezzoo {

Camera::Camera(float width, float height) 
{
  m_WindowWidth = width;
  m_WindowHeight = height;
  m_Position = Vector3f(0.0f, 0.0f, 0.0f);
  m_Target = Vector3f(0.0f, 0.0f, 1.0f);
  m_Up = Vector3f(0.0f, 1.0f, 0.0f);
}
Camera::Camera(float windowWidth, float windowHeight, const Vector3f &pos, const Vector3f &target, const Vector3f &up) 
{
  m_WindowWidth = windowWidth;
  m_WindowHeight = windowHeight;

  m_Position = pos;
  m_Target = target;
  m_Up = up;

  m_Target.Normalize();
  m_Up.Normalize();

  Init();
}

void Camera::Init() 
{
  Vector3f hTarget(m_Target.x, 0.0, m_Target.z);
  hTarget.Normalize();

  float angle = ANGLETODEGREE(asin(abs(hTarget.z)));

  if (hTarget.z >= 0.0f) 
  {
    if (hTarget.x >= 0.0f) 
    {
      m_HAngle = 360.0f - angle;
    } 
    else 
    {
      m_HAngle = 180.0f + angle;
    }
  } 
  else 
  {
    if (hTarget.x >= 0.0f) 
    {
      m_HAngle = angle;
    } 
    else 
    {
      m_HAngle = 180.0f - angle;
    }
  }

  m_VAngle = -ANGLETODEGREE(asin(m_Target.y));

  m_OnUpperEdge = false;
  m_OnLowerEdge = false;
  m_OnLeftEdge = false;
  m_OnRightEdge = false;
  m_MousePos.x = (float)m_WindowWidth / 2;
  m_MousePos.y = (float)m_WindowHeight / 2;
}

Matrix4f &Camera::CalculateViewMatrix() 
{
  // m_View.InitIdentity();
  m_View.InitCameraTransform(m_Position, m_Target, m_Up);

  return m_View;
}

const Vector3f &Camera::GetPosition() const { return m_Position; }

const Vector3f &Camera::GetDirection() const { return m_Target; }

void Camera::OnUpdate(float ts) 
{
  bool ShouldUpdate = false;

  if (m_OnLeftEdge) 
  {
    m_HAngle -= EDGE_STEP;
    ShouldUpdate = true;
  } else if (m_OnRightEdge) 
  {
    m_HAngle += EDGE_STEP;
    ShouldUpdate = true;
  }

  if (m_OnUpperEdge) 
  {
    if (m_VAngle > -90.0f)
    {
      m_VAngle -= EDGE_STEP;
      ShouldUpdate = true;
    }
  } 
  else if (m_OnLowerEdge) 
  {
    if (m_VAngle < 90.0f) 
    {
      m_VAngle += EDGE_STEP;
      ShouldUpdate = true;
    }
  }

  if (ShouldUpdate) 
  {
    Update();
  }

  
  KeyControls();

  m_MousePos.x = Input::GetMouseX();
  m_MousePos.y = Input::GetMouseY();

  MouseControl((WindowsWindow::m_LastMouseX - Input::GetMouseX()),  (WindowsWindow::m_LastMouseY - Input::GetMouseY()));

  m_TimeStep = ts;
}

void Camera::KeyControls() 
{
  float velocity = m_MoveSpeed * m_TimeStep;

  if (Input::IsKeyPressed(GLFW_KEY_W)) 
  {
    m_Position += m_Target * velocity;
  }

  if (Input::IsKeyPressed(GLFW_KEY_S)) 
  {
    m_Position -= m_Target * velocity;
  }

  if (Input::IsKeyPressed(GLFW_KEY_A)) 
  {
    Vector3f left;
    left = m_Target.Cross(m_Up);
    left.Normalize();
    m_Position += left * velocity;
  }
  if (Input::IsKeyPressed(GLFW_KEY_D)) 
  {

    Vector3f right;
    right = m_Up.Cross(m_Target);
    right.Normalize();
    m_Position += right * velocity;
  }
  if (Input::IsKeyPressed(GLFW_KEY_UP)) 
  {
    m_Position.y += velocity;
  }

  if (Input::IsKeyPressed(GLFW_KEY_DOWN)) 
  {
    m_Position.y -= velocity;
  }

  if (Input::IsKeyPressed(GLFW_KEY_LEFT_ALT)) 
  {
    m_LeftAlt = true;
  } 
  else 
  {
    m_LeftAlt = false;
  }
}

void Camera::MouseControl(float xChange, float yChange) 
{

  if (m_LeftAlt) 
  {

    m_HAngle += xChange / 20.0f;
    m_VAngle -= yChange / 50.0f;

    if (xChange == 0) 
    {
      if (m_MousePos.x <= 10.0f) 
      {
        m_OnLeftEdge = true;
      } 
      else if (m_MousePos.x >= (m_WindowWidth - 10.0f)) 
      {
        m_OnRightEdge = true;
      }
    } 
    else 
    {
      m_OnLeftEdge = false;
      m_OnRightEdge = false;
    }

    if (yChange == 0) 
    {
      if (m_MousePos.y <= 10.0f) 
      {
       m_OnUpperEdge = true;
      } 
      else if (m_MousePos.y >= (m_WindowHeight - 10.0f)) 
      {  
         m_OnLowerEdge = true;
      }
    } 
    else 
    {
      m_OnUpperEdge = false;
      m_OnLowerEdge = false;
    }

    Update();
  }
}

void Camera::Update() 
{
  Vector3f Yaxis(0.0f, 1.0f, 0.0f);

  // Rotate the view vector by the horizontal angle around the vertical axis
  Vector3f View(1.0f, 0.0f, 0.0f);
  View.Rotate(m_HAngle, Yaxis);
  View.Normalize();

  // Rotate the view vector by the vertical angle around the horizontal axis
  Vector3f U = Yaxis.Cross(View);
  View.Rotate(m_VAngle, U);
  U.Normalize();

  m_Target = View;
  m_Target.Normalize();

  m_Up = m_Target.Cross(U);
  m_Up.Normalize();
}

void Camera::SetViewportSize (uint32_t width, uint32_t height)
{
  m_WindowWidth = width;
  m_WindowHeight = height;

  CalculateViewMatrix();
}

/*void Camera::OnEvent(Event &e) {
  EventDispatcher dispatch(e);
  dispatch.Dispatch<MouseScrolledEvent>(BIND(Camera::OnMouseScrolledEvent));
  dispatch.Dispatch<KeyPressedEvent>(BIND(Camera::OnKeyPressedEvent));
}*/

void Camera::SetPosition(const Vector3f &pos) { m_Position = pos; }
/*bool Camera::OnKeyPressedEvent(KeyPressedEvent &event) {
  // EZZOO_WARN("KeyPressed {}", event.GetKeyCode());
  return true;
}*/

/*bool Camera::OnMouseScrolledEvent(MouseScrolledEvent &event) {
  // EZZOO_WARN("Mouse Scrolled {}", event.GetYoffset());

  return true;
}*/

} // namespace Ezzoo
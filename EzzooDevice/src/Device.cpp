#include "Device.h"
#include "ezpch.h"

#include "Core/Core.h"
#include "Core/Layer.h"

#include "Renderer/Buffers.h"
// #include "Renderer/EditorCamera.h"
#include "Renderer/Camera.h"
#include "Renderer/FrameBuffer.h"
#include "Renderer/RendererCommand.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexArray.h"

#include "Math/PresProjection.h"
#include "Math/WorldTransform.h"

#include "Platform/OpenGL/OpenGLShader.h"

#include "Core/FusionCalculation.h"

#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"

// #ifdef __EMSCRIPTEN__

#include "emscripten/websocket.h"

char url[256] = "ws://192.168.4.1:3333/ws"; //"ws://192.168.1.50:3333/ws";
static bool s_ConnectFlag = false;

namespace EMWebSocket {

EMSCRIPTEN_WEBSOCKET_T socket;

static EM_BOOL OnOpen(int eventType, const EmscriptenWebSocketOpenEvent *e, void *userData) 
{
  printf("Socket Connected\n");
  s_ConnectFlag = true;
  return EM_TRUE;
}
static EM_BOOL OnMessage(int eventType, const EmscriptenWebSocketMessageEvent *e, void *userData) 
{
  char data[1024] = "\0";
  strcpy(data, (const char *)e->data);

  Ezzoo::Buffer buffer = Ezzoo::Buffer(1024);
  memcpy((char *)buffer.Data, data, 1024);

  ((Ezzoo::FusionCalculation *)userData)->Update(buffer);

  buffer.Release();

  return EM_TRUE;
}
static EM_BOOL OnClose(int eventType, const EmscriptenWebSocketCloseEvent *e, void *userData) 
{
  printf("Socket Closed\n");
  s_ConnectFlag = false;
  return EM_TRUE;
}

static void SendRequest(const char *data) 
{
  printf("Sent Data: %s\n", data);
  emscripten_websocket_send_utf8_text(socket, data);
}

static void Connect(const char *url, Ezzoo::FusionCalculation &fusion) 
{
  if (!emscripten_websocket_is_supported()) {
    printf("Websocket not Supported\n");
    return;
  }
  printf("trying To Connect\n");
  EmscriptenWebSocketCreateAttributes attr;

  attr.url = url;
  attr.protocols = NULL;
  attr.createOnMainThread = EM_TRUE;

  socket = emscripten_websocket_new(&attr);

  emscripten_websocket_set_onopen_callback(socket, NULL, OnOpen);
  emscripten_websocket_set_onmessage_callback(socket, (void *)&fusion, OnMessage);
  emscripten_websocket_set_onclose_callback(socket, NULL, OnClose);
}
} // namespace EMWebSocket

// #endif

class ExampleLayer : public Ezzoo::Layer {

public:
  ExampleLayer() : Layer("ExampleLayer") 
  {
    m_CubeVertexArray = Ezzoo::VertexArray::Create();
    m_CubeVertexArray->Bind();
    float cubeVertices[3 * 8] = {-0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f,
                                 0.5f,  0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f,
                                 -0.5f, -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,
                                 0.5f,  0.5f,  0.5f,  -0.5f, 0.5f,  0.5f};
    m_CubeVertexBuffer =
        Ezzoo::VertexBuffer::Create(cubeVertices, sizeof(cubeVertices));
    m_CubeVertexBuffer->SetLayout({{Ezzoo::ShaderDataType::Float3, "a_Position"}});
    m_CubeVertexArray->AddVertexBuffer(m_CubeVertexBuffer);
    uint32_t cubeIndecies[36] = {0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4,
                                 4, 0, 3, 3, 7, 4, 1, 5, 6, 6, 2, 1,
                                 4, 5, 1, 1, 0, 4, 3, 2, 6, 6, 7, 3};
    m_CubeIndexBuffer = Ezzoo::IndexBuffer::Create(cubeIndecies, sizeof(cubeIndecies) / sizeof(uint32_t));
    m_CubeVertexArray->SetIndexBuffer(m_CubeIndexBuffer);

    // floor
    m_FloorVertexArray = Ezzoo::VertexArray::Create();
    m_FloorVertexArray->Bind();
    float floorVertices[3 * 4] = {-1.0, 0.0, -1.0, 1.0,  0.0, -1.0,
                                  1.0,  0.0, 1.0,  -1.0, 0.0, 1.0};
    m_FloorVertexBuffer = Ezzoo::VertexBuffer::Create(floorVertices, sizeof(floorVertices));
    m_FloorVertexBuffer->SetLayout({{Ezzoo::ShaderDataType::Float3, "a_Position"}});
    m_FloorVertexArray->AddVertexBuffer(m_FloorVertexBuffer);
    uint32_t floorIndecies[6] = {0, 2, 1, 2, 0, 3};
    m_FloorIndexBuffer = Ezzoo::IndexBuffer::Create(floorIndecies, sizeof(floorIndecies) / sizeof(uint32_t));
    m_FloorVertexArray->SetIndexBuffer(m_FloorIndexBuffer);

#ifndef __EMSCRIPTEN__
    // auto cubeShader = m_CubeShader.Load("Cube",
    // "Resources/Shaders/CubeShader.vertex");
#else
    m_CubeShader = Ezzoo::Shader::Create("CubeShaderES", "Resources/Shaders/CubeShaderES.vertex", "Resources/Shaders/CubeShaderES.frag");
    m_EndlessShader = Ezzoo::Shader::Create("EndLessShader", "Resources/Shaders/EndLessShader.vertex","Resources/Shaders/EndLessShader.frag");
    // auto cubeShader = m_CubeShaderLib.Load("Cube",
    // "Resources/Shaders/CubeShaderES.glsl");
#endif

    m_FusionCalculation = Ezzoo::FusionCalculation(0.5f);
  }
  ~ExampleLayer() {}

  virtual void OnAttach() override 
  {

    auto &app = Ezzoo::Application::GetApplication();
    float width = (float)(app.GetWindow().GetWidth());
    float height = (float)(app.GetWindow().GetHeight());

    float distance = 1.0f / (tanf(ANGLETORADIAN(45.0f / 2.0f)));
    float aspectRation = (width / height);
    float near = 1.0f;
    float far = 100.0f;
    float zRange = near - far;
    float A = ((-far - near) / zRange);
    float B = ((2.0f * far * near) / zRange);

    m_PresProjection.SetPresProjection(distance, aspectRation, A, B);
    m_Camera = Ezzoo::Camera(width, height, Ezzoo::Vector3f(0.0f, 1.0f, -3.0f),Ezzoo::Vector3f(0.0f, 0.0f, 1.0f),  Ezzoo::Vector3f(0.0f, 1.0f, 0.0f));

    Ezzoo::FrameBufferSpecification specs;
    specs.Attachments = {Ezzoo::FrameBufferTextureFormate::RGBA8,Ezzoo::FrameBufferTextureFormate::Depth};
    specs.Width = 700;
    specs.Height = 400;
    m_FrameBuffer = Ezzoo::FrameBuffer::Create(specs);
  }
  virtual void OnDetach() override {}
  virtual void OnUpdate(Ezzoo::TimeStep ts) override 
  {
    if (Ezzoo::FrameBufferSpecification spec = m_FrameBuffer->GetSpecification(); m_ViewPortSize.x > 0.0f && m_ViewPortSize.y > 0.0f &&(spec.Width != (uint32_t)m_ViewPortSize.x ||
         spec.Height != (uint32_t)m_ViewPortSize.y)) 
    {

      m_FrameBuffer->Resize((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
      // m_CameraController.OnResize(m_ViewPortSize.x, m_ViewPortSize.y);
      m_Camera.SetViewportSize(m_ViewPortSize.x, m_ViewPortSize.y);
    }

    m_FrameBuffer->Bind();

    Ezzoo::RendererCommand::SetClearColor({0.529f, 0.808f, 0.9221f, 1.0f});
    Ezzoo::RendererCommand::ClearColor();

    // m_FrameBuffer->ClearAttachment(1, -1);
    m_Camera.OnUpdate(ts);

    float yaw = m_FusionCalculation.Yaw;
    float pitch = m_FusionCalculation.Pitch;
    float roll = m_FusionCalculation.Roll;

    /*glm::mat4 model =
        glm::translate(glm::mat4(1.0f), m_FusionCalculation.GetPosState()) *
        glm::rotate(glm::mat4(1.0f), glm::radians(yaw), {1.0f, 0.0f, 0.0f}) *
        glm::rotate(glm::mat4(1.0f), glm::radians(pitch), {0.0f, 1.0f, 0.0f}) *
        glm::rotate(glm::mat4(1.0f), glm::radians(roll), {0.0f, 0.0f, 1.0f}) *
        glm::scale(glm::mat4(1.0f), {0.3f, 0.3f, 0.3f});

    glm::mat4 mvp = m_Camera.GetViewProjection() * model;*/

    auto &posVec = m_FusionCalculation.GetPosState();
    m_CubeWorldTransform.SetTranslate(posVec.x, posVec.y, posVec.z);
    m_CubeWorldTransform.SetRotate(yaw, pitch, roll);
    m_CubeWorldTransform.SetScale(0.3f, 0.3f, 0.3f);

    Ezzoo::Matrix4f model = m_CubeWorldTransform.GetTransform();
    Ezzoo::Matrix4f proj = m_PresProjection.GetPresProjection();
    Ezzoo::Matrix4f view = m_Camera.CalculateViewMatrix();
    Ezzoo::Matrix4f mvp = proj * view * model;

    m_CubeShader->Bind();
    std::dynamic_pointer_cast<Ezzoo::OpenGLShader>(m_CubeShader)->UploadUniformMat4("u_ViewProjection", mvp.ToGLM());
    Ezzoo::Renderer::Submit(m_CubeVertexArray, m_CubeShader);
    m_CubeShader->Unbind();

    // Floor
    //m_FloorWorldTransform.SetTranslate(0.0f, 0.0f, 0.0f);
    //m_FloorWorldTransform.SetRotate(0.0f, 0.0f, 0.0f);
    //m_FloorWorldTransform.SetScale(1000.0f, 0.0f, 1000.0f);

    //model = m_FloorWorldTransform.GetTransform();
    mvp = proj * view ;//* model;

    m_EndlessShader->Bind();
    std::dynamic_pointer_cast<Ezzoo::OpenGLShader>(m_EndlessShader)->UploadUniformMat4("u_ViewProjection", mvp.ToGLM());
    std::dynamic_pointer_cast<Ezzoo::OpenGLShader>(m_EndlessShader)->UploadUniformFloat3("gCameraWorldPos", m_Camera.GetPosition().ToGLM());
    Ezzoo::Renderer::Submit(m_FloorVertexArray, m_EndlessShader);
    m_EndlessShader->Unbind();

    m_FrameBuffer->UnBind();
  }

  virtual void OnImGuiRender() override {

    // ImGui::SetNextWindowPos(ImVec2{400.0f, 300.0f});
    ImGui::Begin("ViewPort", nullptr,
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar);

    auto viewPortMinRegion = ImGui::GetWindowContentRegionMin();
    auto viewPortMaxRegion = ImGui::GetWindowContentRegionMax();
    auto viewPortOffset = ImGui::GetWindowPos();
    m_ViewPortBounds[0] = {viewPortMinRegion.x + viewPortOffset.x,
                           viewPortMinRegion.y + viewPortOffset.y};
    m_ViewPortBounds[1] = {viewPortMaxRegion.x + viewPortOffset.x,
                           viewPortMaxRegion.y + viewPortOffset.y};

    m_ViewPortHovered = ImGui::IsWindowHovered();
    m_ViewPortFocused = ImGui::IsWindowFocused();
    Ezzoo::Application::GetApplication().GetImGuiLayer()->SetBlockEvents(
        !m_ViewPortHovered && !m_ViewPortFocused);

    ImVec2 viewPortPanelSize = ImGui::GetContentRegionAvail();
    m_ViewPortSize = {viewPortPanelSize.x, viewPortPanelSize.y};
    /*if (m_ViewPortSize != *((glm::vec2*)&viewPortPanelSize) &&
    viewPortPanelSize.x > 0 && viewPortPanelSize.y > 0)
    {
            m_FrameBuffer->Resize((uint32_t)viewPortPanelSize.x,
    (uint32_t)viewPortPanelSize.y);

    }*/

    uint32_t id = m_FrameBuffer->GetColorAttachmentRendererID();
    ImGui::Image((ImTextureID)(uint64_t)(id), ImVec2{m_ViewPortSize.x, m_ViewPortSize.y}, ImVec2{0, 1}, ImVec2{1, 0});

    ImGui::End();

    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::SetNextWindowPos(ImVec2{10.0f, 40.0f});
    ImGui::SetNextWindowSize({240.0f, 280.0f});

    ImGui::Begin("ControlBoard");

    auto &trustConfig = m_FusionCalculation.GetKalmanConfig();
    ImGui::SliderFloat("GPSTrust", &trustConfig.GPSTrust, 0.0f, 1.0f);
    ImGui::SliderFloat("MPUTrust", &trustConfig.MPUTrust, 0.0f, 1.0f);
    ImGui::SliderFloat("Acc_still", &trustConfig.ACCStill, 0.0f, 1.0f);
    ImGui::SliderFloat("Statioary", &trustConfig.Stationary, 0.0f, 1.0f);
    ImGui::SliderFloat("Max_vel", &trustConfig.MaxVel, 0.0f, 10.0f);

    if (!s_ConnectFlag) {
      // InputText(const char* label, char* buf, size_t buf_size,
      // ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL,
      // void* user_data = NULL);

      ImGui::Separator();
      ImGui::Separator();

      ImGui::SetNextItemWidth(190.0f);
      ImGui::InputText("##WS_URL", url, 256);

      if (ImGui::Button("Connect", ImVec2(80.0f, 30.0f))) {
        EMWebSocket::Connect(url, m_FusionCalculation);
      }

    } else {
      if (ImGui::Button("Disconnect", ImVec2(80.0f, 30.0f))) {
        s_ConnectFlag = false;
        std::string data = R"({"Key": "Stop"})";
        EMWebSocket::SendRequest(data.c_str());
        EMWebSocket::socket = -1;
      }
    }

    ImGui::Separator();
    ImGui::Separator();

    if (ImGui::Button("StartTransmit", ImVec2(100.0f, 30.0f))) {
      std::string data = R"({"Key": "Start"})";
      EMWebSocket::SendRequest(data.c_str());
    }

    ImGui::SameLine();

    if (ImGui::Button("StopTransmit", ImVec2(100.0f, 30.0f))) {
      std::string data = R"({"Key": "Stop"})";
      EMWebSocket::SendRequest(data.c_str());
    }

    ImGui::End();

    m_FusionCalculation.OnImGuiRender();
  }
  virtual void OnEvent(Ezzoo::Event &e) override {}

private:
  Ezzoo::Ref<Ezzoo::Shader> m_CubeShader;
  Ezzoo::Ref<Ezzoo::VertexArray> m_CubeVertexArray;
  Ezzoo::Ref<Ezzoo::VertexBuffer> m_CubeVertexBuffer;
  Ezzoo::Ref<Ezzoo::IndexBuffer> m_CubeIndexBuffer;

  Ezzoo::Ref<Ezzoo::Shader> m_EndlessShader;
  Ezzoo::Ref<Ezzoo::VertexArray> m_FloorVertexArray;
  Ezzoo::Ref<Ezzoo::VertexBuffer> m_FloorVertexBuffer;
  Ezzoo::Ref<Ezzoo::IndexBuffer> m_FloorIndexBuffer;

  Ezzoo::Ref<Ezzoo::FrameBuffer> m_FrameBuffer;

  glm::vec2 m_ViewPortSize = {0.0f, 0.0f};
  glm::vec2 m_ViewPortBounds[2];
  bool m_ViewPortHovered = false;
  bool m_ViewPortFocused = false;

  // Ezzoo::Ref<Ezzoo::Texture2D> m_Texture, m_LogoTexture;

  Ezzoo::FusionCalculation m_FusionCalculation;

  Ezzoo::Camera m_Camera;
  Ezzoo::PresProjection m_PresProjection;
  Ezzoo::WorldTransform m_CubeWorldTransform;
  Ezzoo::WorldTransform m_FloorWorldTransform;
};
// #endif

namespace Ezzoo {

Device::Device(const ApplicationSpecification &specs) : Application(specs) {
  PushLayer(new ExampleLayer());
  // PushLayer(new Device2D());
}

Device::~Device() {}
} // namespace Ezzoo

Ezzoo::Application *
Ezzoo::CreatApplication(AppCommandLineArgs commandLineArgs) {
  ApplicationSpecification specs;
  specs.Name = "EzzooNut";
  // specs.path = "../SandBox";
  specs.CommandLineArgs = commandLineArgs;

  return new Device(specs);
}

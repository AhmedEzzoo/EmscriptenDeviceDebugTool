#include "Device.h"
#include "ezpch.h"

#include "Core/Core.h"
#include "Core/Layer.h"

#include "Renderer/Buffers.h"
#include "Renderer/EditorCamera.h"
#include "Renderer/FrameBuffer.h"
#include "Renderer/RendererCommand.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexArray.h"

#include "Platform/OpenGL/OpenGLShader.h"

#include "Core/FusionCalculation.h"

#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"


//#ifdef __EMSCRIPTEN__

#include "emscripten/websocket.h"


char url[256] = "ws://192.168.4.1:3333/ws";//"ws://192.168.1.50:3333/ws";
static bool s_ConnectFlag = false;

namespace EMWebSocket {

EMSCRIPTEN_WEBSOCKET_T socket;

static EM_BOOL OnOpen(int eventType, const EmscriptenWebSocketOpenEvent *e, void* userData) 
{
  printf("Socket Connected\n");
  s_ConnectFlag = true;
  return EM_TRUE;
}
static EM_BOOL OnMessage(int eventType, const EmscriptenWebSocketMessageEvent *e, void* userData) 
{
  char data [1024] = "\0";
  strcpy(data, (const char*)e->data);

  Ezzoo::Buffer buffer = Ezzoo::Buffer(1024);
  memcpy((char*)buffer.Data, data, 1024);

  ((Ezzoo::FusionCalculation*)userData)->Update(buffer);

  buffer.Release();

  return EM_TRUE;
}
static EM_BOOL OnClose(int eventType, const EmscriptenWebSocketCloseEvent *e, void* userData) 
{ 
  printf("Socket Closed\n");
  s_ConnectFlag = false;
  return EM_TRUE;
}

static void SendRequest(const char* data)
{
   printf("Sent Data: %s\n", data);
  emscripten_websocket_send_utf8_text(socket, data);
}

static void Connect(const char *url, Ezzoo::FusionCalculation& fusion) 
{
  if (!emscripten_websocket_is_supported()) 
  {
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
  emscripten_websocket_set_onmessage_callback(socket, (void*)&fusion, OnMessage);
  emscripten_websocket_set_onclose_callback(socket, NULL, OnClose);
}
} // namespace EMWebSocket

//#endif

class ExampleLayer : public Ezzoo::Layer {

public:
  ExampleLayer() : Layer("ExampleLayer"), m_EditorCamera(30.0f, 900.0f / 600.0f, 0.1f, 1000.0f)
  {
    m_VertexArray = Ezzoo::VertexArray::Create();
    m_VertexArray->Bind();
    float vertices[3 * 8] = {-0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f,
                             0.5f,  0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f,
                             -0.5f, -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,
                             0.5f,  0.5f,  0.5f,  -0.5f, 0.5f,  0.5f};
    m_VertexBuffer = Ezzoo::VertexBuffer::Create(vertices, sizeof(vertices));

    m_VertexBuffer->SetLayout({{Ezzoo::ShaderDataType::Float3, "a_Position"}});

    m_VertexArray->AddVertexBuffer(m_VertexBuffer);

    uint32_t indecies[36] = {0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4,
                             4, 0, 3, 3, 7, 4, 1, 5, 6, 6, 2, 1,
                             4, 5, 1, 1, 0, 4, 3, 2, 6, 6, 7, 3};
    m_IndexBuffer = Ezzoo::IndexBuffer::Create(indecies, sizeof(indecies) /
                                                             sizeof(uint32_t));
    m_VertexArray->SetIndexBuffer(m_IndexBuffer);

#ifndef __EMSCRIPTEN__
    // auto cubeShader = m_Shader.Load("Cube",
    // "Resources/Shaders/CubeShader.vertex");
#else
    m_Shader = Ezzoo::Shader::Create("CubeShaderES", "Resources/Shaders/CubeShaderES.vertex", "Resources/Shaders/CubeShaderES.frag");
    // auto cubeShader = m_ShaderLib.Load("Cube",
    // "Resources/Shaders/CubeShaderES.glsl");
#endif

    m_FusionCalculation = Ezzoo::FusionCalculation(0.5f);
 }
  ~ExampleLayer() {}
  
  virtual void OnAttach() override 
  {
    Ezzoo::FrameBufferSpecification specs;
    specs.Attachments = {Ezzoo::FrameBufferTextureFormate::RGBA8,
                         Ezzoo::FrameBufferTextureFormate::Depth};
    specs.Width = 700;
    specs.Height = 400;
    m_FrameBuffer = Ezzoo::FrameBuffer::Create(specs);

    // auto [mx, my] = ImGui::GetMousePos();
    //
    // mx -= m_ViewPortBounds[0].x;
    // my -= m_ViewPortBounds[0].y;
    //
    // glm::vec2 viewPortsize = m_ViewPortBounds[1] - m_ViewPortBounds[0];
    //
    // my = viewPortsize.y - my;
    // int mouseX = (int)mx;
    // int mouseY = (int)my;
    //
    // if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewPortsize.x && mouseY
    // < (int)viewPortsize.y)
    //{
    //	int pixelData = m_FrameBuffer->ReadPixel(1, mouseX, mouseY);
    //	//m_HoveredEntity = pixelData == -1 ? Entity() :
    // Entity((entt::entity)pixelData, m_ActiveScene.get());
    //	//EZZOO_CLIENT_WARNING("{0}", pixelData);
    //	//if (pixelData != -1) m_ScenePanel.SetSelectedEntity(m_HoveredEntity);
    // }

   
  }
  virtual void OnDetach() override {}
  virtual void OnUpdate(Ezzoo::TimeStep ts) override 
  {
    if (Ezzoo::FrameBufferSpecification spec = m_FrameBuffer->GetSpecification(); m_ViewPortSize.x > 0.0f && m_ViewPortSize.y > 0.0f && (spec.Width != (uint32_t)m_ViewPortSize.x ||spec.Height != (uint32_t)m_ViewPortSize.y)) 
    {

      m_FrameBuffer->Resize((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
      // m_CameraController.OnResize(m_ViewPortSize.x, m_ViewPortSize.y);
      m_EditorCamera.SetViewportSize(m_ViewPortSize.x, m_ViewPortSize.y);
    }

    m_FrameBuffer->Bind();

    Ezzoo::RendererCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
    Ezzoo::RendererCommand::ClearColor();

    // m_FrameBuffer->ClearAttachment(1, -1);
    m_EditorCamera.OnUpdate(ts);

    float posx = m_FusionCalculation.PosState.x;
    float posy = m_FusionCalculation.PosState.y;
    float posz = m_FusionCalculation.PosState.z;

    float yaw = m_FusionCalculation.Yaw;
    float pitch = m_FusionCalculation.Pitch;
    float roll = m_FusionCalculation.Roll;

    glm::mat4 model =
        glm::translate(glm::mat4(1.0f), {posx, posy, posz}) *
        glm::rotate(glm::mat4(1.0f), glm::radians(yaw), {1.0f, 0.0f, 0.0f}) *
        glm::rotate(glm::mat4(1.0f), glm::radians(pitch), {0.0f, 1.0f, 0.0f}) *
        glm::rotate(glm::mat4(1.0f), glm::radians(roll), {0.0f, 0.0f, 1.0f}) *
        glm::scale(glm::mat4(1.0f), {1.0f, 0.3f, 1.5f});

    glm::mat4 mvp = m_EditorCamera.GetViewProjection() * model;

    m_Shader->Bind();
    std::dynamic_pointer_cast<Ezzoo::OpenGLShader>(m_Shader)->UploadUniformMat4("u_ViewProjection", mvp);
    Ezzoo::Renderer::Submit(m_VertexArray, m_Shader);

    m_FrameBuffer->UnBind();
  }

  virtual void OnImGuiRender() override 
  {

    //ImGui::SetNextWindowPos(ImVec2{400.0f, 300.0f});
    ImGui::Begin("ViewPort", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar);

    auto viewPortMinRegion = ImGui::GetWindowContentRegionMin();
    auto viewPortMaxRegion = ImGui::GetWindowContentRegionMax();
    auto viewPortOffset = ImGui::GetWindowPos();
    m_ViewPortBounds[0] = {viewPortMinRegion.x + viewPortOffset.x, viewPortMinRegion.y + viewPortOffset.y};
    m_ViewPortBounds[1] = {viewPortMaxRegion.x + viewPortOffset.x, viewPortMaxRegion.y + viewPortOffset.y};

    m_ViewPortHovered = ImGui::IsWindowHovered();
    m_ViewPortFocused = ImGui::IsWindowFocused();
    Ezzoo::Application::GetApplication().GetImGuiLayer()->SetBlockEvents(!m_ViewPortHovered && !m_ViewPortFocused);

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


    ImGui::SetNextWindowPos(ImVec2{10.0f, 40.0f});    
    ImGui::Begin("ControlBoard");

     

      if (!s_ConnectFlag)
      {
        // InputText(const char* label, char* buf, size_t buf_size, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
        ImGui::InputText("WS_URL", url, 256);
        if (ImGui::Button("Connect", ImVec2(80.0f, 30.0f))) 
        {
          EMWebSocket::Connect(url, m_FusionCalculation);
        }

      }
      else 
      {
        if (ImGui::Button("Disconnect", ImVec2(80.0f, 30.0f))) 
        {
          s_ConnectFlag = false;
          std::string data = R"({"Key": "Stop"})";
          EMWebSocket::SendRequest(data.c_str());
          EMWebSocket::socket = -1;
        }
      }

        ImGui::Separator();
        ImGui::Separator();

        if (ImGui::Button("StartTransmit", ImVec2(100.0f, 30.0f))) 
        {
          std::string data = R"({"Key": "Start"})";
          EMWebSocket::SendRequest(data.c_str());
        }

        ImGui::Separator();
        ImGui::Separator();
        
        if (ImGui::Button("StopTransmit", ImVec2(100.0f, 30.0f))) 
        {
          std::string data = R"({"Key": "Stop"})";
          EMWebSocket::SendRequest(data.c_str());
        }

    ImGui::End();

    m_FusionCalculation.OnImGuiRender();
   
  }
  virtual void OnEvent(Ezzoo::Event &e) override { m_EditorCamera.OnEvent(e); }

private:
  Ezzoo::Ref<Ezzoo::Shader> m_Shader;
  Ezzoo::Ref<Ezzoo::VertexArray> m_VertexArray;
  Ezzoo::Ref<Ezzoo::VertexBuffer> m_VertexBuffer;
  Ezzoo::Ref<Ezzoo::IndexBuffer> m_IndexBuffer;

  Ezzoo::Ref<Ezzoo::FrameBuffer> m_FrameBuffer;

  glm::vec2 m_ViewPortSize = {0.0f, 0.0f};
  glm::vec2 m_ViewPortBounds[2];
  bool m_ViewPortHovered = false;
  bool m_ViewPortFocused = false;

  // Ezzoo::Ref<Ezzoo::Texture2D> m_Texture, m_LogoTexture;

  Ezzoo::FusionCalculation m_FusionCalculation;
  Ezzoo::EditorCamera m_EditorCamera;

};
// #endif

namespace Ezzoo {

Device::Device(const ApplicationSpecification &specs) : Application(specs) 
{
  PushLayer(new ExampleLayer());
  // PushLayer(new Device2D());
}

Device::~Device() {}
} // namespace Ezzoo

Ezzoo::Application *
Ezzoo::CreatApplication(AppCommandLineArgs commandLineArgs) 
{
  ApplicationSpecification specs;
  specs.Name = "EzzooNut";
  // specs.path = "../SandBox";
  specs.CommandLineArgs = commandLineArgs;

  return new Device(specs);
}

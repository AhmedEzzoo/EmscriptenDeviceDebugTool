#include "ezpch.h"


#include "Application.h"
#include "FusionCalculation.h"

#include "nlohmann/json.hpp"
using json = nlohmann::json;

#include "Log.h"
#include "GLFW/glfw3.h"
#include "imgui.h"

#include <Eigen/Dense>

namespace Ezzoo {

    
   struct KalmanPV 
   {
        Eigen::Vector3f p;
        Eigen::Vector3f v;
        Eigen::Vector3f ba;

        Eigen::Matrix<float, NX, 1> vec() const 
        {
            Eigen::Matrix<float, NX, 1> x;
            x.segment<3>(0) = p;
            x.segment<3>(3) = v;
            x.segment<3>(6) = ba;
            return x;
        }

        void fromVec(const Eigen::Matrix<float, NX, 1>& x) {
            p  = x.segment<3>(0);
            v  = x.segment<3>(3);
            ba = x.segment<3>(6);
        }
    };

   struct KalmanFilter 
   {
        Eigen::Matrix<float, NX, 1>  x;           // state vector
        Eigen::Matrix<float, NX, NX> P;          // covariance
        Eigen::Matrix<float, NX, NX> Q;          // process noise (constant parts)

        KalmanFilter() 
        {
            x.setZero();
            P.setIdentity();
            Q.setZero();

            Init(Eigen::Vector3f::Zero(), Eigen::Vector3f::Zero(), Eigen::Vector3f::Zero(), 1.0f, 0.1f, 0.01f);
        }

        void Init(const Eigen::Vector3f &p0, const Eigen::Vector3f &v0, const Eigen::Vector3f &ba0, float pos_var, float vel_var, float ba_var)
        {
            x.segment<3>(0) = p0;
            x.segment<3>(3) = v0;
            x.segment<3>(6) = ba0;

            P.setZero();
            P.block<3,3>(0,0) = Eigen::Matrix3f::Identity() * pos_var;
            P.block<3,3>(3,3) = Eigen::Matrix3f::Identity() * vel_var;
            P.block<3,3>(6,6) = Eigen::Matrix3f::Identity() * ba_var;

            Q.setZero();
        }

        void Predict(const Eigen::Quaternionf &q_wb, const Eigen::Vector3f &a_meas_body, float dt, float sigma_acc, float sigma_ba_rw)
        {
            const Eigen::Vector3f g(0.0f,0.0f,-9.80665f);

            Eigen::Vector3f p  = x.segment<3>(0);
            Eigen::Vector3f v  = x.segment<3>(3);
            Eigen::Vector3f ba = x.segment<3>(6);

            Eigen::Vector3f a_unbiased_body = a_meas_body - ba;
            Eigen::Vector3f a_world = q_wb * a_unbiased_body;
            Eigen::Vector3f a_wg = a_world + g;

            for(int i=0;i<3;i++)
            {
                if(fabs(a_wg[i]) < 0.07f)
                    a_wg[i] = 0;
            }

            Eigen::Vector3f p_pred  = p + v*dt + 0.5f *a_wg*dt*dt;
            Eigen::Vector3f v_pred  = v + a_wg*dt;
            Eigen::Vector3f ba_pred = ba;


            bool stationary = (a_wg.norm() < 0.7f);
 
            if(stationary)
            {
                v_pred.setZero();
            }
            else
            {
                v_pred *= 0.995f;
            }

            float maxVel = 3.0f;

            if(v_pred.norm() > maxVel)
            {
                v_pred = v_pred.normalized() * maxVel;
            }


            x.segment<3>(0) = p_pred;
            x.segment<3>(3) = v_pred;
            x.segment<3>(6) = ba_pred;

            Eigen::Matrix<float, NX, NX> F = Eigen::Matrix<float, NX, NX>::Identity();
            F.block<3,3>(0,3) = Eigen::Matrix3f::Identity() * dt;

            Eigen::Matrix3f R = q_wb.toRotationMatrix();
            Eigen::Matrix3f dA_dba = -R;

            F.block<3,3>(0,6) = 0.5f * dt*dt * dA_dba;
            F.block<3,3>(3,6) = dt * dA_dba;

            Eigen::Matrix<float, NX, NX> Qd = Eigen::Matrix<float, NX, NX>::Zero();
            float s2 = sigma_acc * sigma_acc;

            Qd.block<3,3>(0,0) = Eigen::Matrix3f::Identity() * (s2 * dt*dt*dt*dt / 4.0f);
            Qd.block<3,3>(0,3) = Eigen::Matrix3f::Identity() * (s2 * dt*dt*dt / 2.0f);
            Qd.block<3,3>(3,0) = Qd.block<3,3>(0,3).transpose();
            Qd.block<3,3>(3,3) = Eigen::Matrix3f::Identity() * (s2 * dt*dt);
            Qd.block<3,3>(6,6) = Eigen::Matrix3f::Identity() * (sigma_ba_rw*sigma_ba_rw * dt);

            P = F * P * F.transpose() + Qd;
            P = 0.5f * (P + P.transpose());
        }

        void UpdateVelocityZUPT(const Eigen::Vector3f& z_vel, const Eigen::Matrix3f& R_vel)
        {
            Eigen::Matrix<float,3,NX> H = Eigen::Matrix<float,3,NX>::Zero();
            H.block<3,3>(0,3) = Eigen::Matrix3f::Identity();

            Eigen::Vector3f y = z_vel - H * x;

            Eigen::Matrix3f S = H * P * H.transpose() + R_vel;
            Eigen::Matrix<float,NX,3> K = P * H.transpose() * S.inverse();

            x = x + K * y;

            Eigen::Matrix<float,NX,NX> I = Eigen::Matrix<float,NX,NX>::Identity();
            P = (I - K * H) * P;

            P = 0.5f * (P + P.transpose());
        }

        void UpdatePosition(const Eigen::Vector3f& z_pos, const Eigen::Matrix3f& R_pos)
        {
            Eigen::Matrix<float,3,NX> H = Eigen::Matrix<float,3,NX>::Zero();
            H.block<3,3>(0,0) = Eigen::Matrix3f::Identity();

            Eigen::Vector3f y = z_pos - H * x;
            Eigen::Matrix3f S = H * P * H.transpose() + R_pos;
            Eigen::Matrix<float,NX,3> K = P * H.transpose() * S.inverse();

            x = x + K * y;

            Eigen::Matrix<float,NX,NX> I = Eigen::Matrix<float,NX,NX>::Identity();
            P = (I - K * H) * P;

            P = 0.5f * (P + P.transpose());
        }
    };


    static KalmanFilter m_KalmanFilter;

/////////////////////////////////////////////////////////////////////////////////////////
    static int s_GPSTrigger = 0;

    FusionCalculation::FusionCalculation(float sampleRate)
    {
        m_Acc.x = 0.0f;
        m_Acc.y = 0.0f;
        m_Acc.z = 0.0f;

        m_Gyro.x = 0.0f;
        m_Gyro.y = 0.0f;
        m_Gyro.z = 0.0f;
        m_Mag.x = 0.0f;
        m_Mag.y = 0.0f;
        m_Mag.z = 0.0f;

    }
    FusionCalculation::~FusionCalculation()
    {

    }

    void FusionCalculation::Attach () 
    {

        //Kalman filter covariance init;
        PosState = glm::vec3(0.0f, 0.0f, 0.0f);
        VelState = glm::vec3(0.0f, 0.0f, 0.0f);
   

    }

    void FusionCalculation::Update (Buffer& buffer) 
    {   
          
        
        
        try
        {
            const char* data = buffer.As<const char>();
            json JsonObj = json::parse(data);
            
            //std::cout << data << std::endl;

            std::string flag = JsonObj["Flag"];
            if (flag == "MPU")
            {
                PlayerName = JsonObj["Player_name"];
                
                m_Acc.x    = (float)JsonObj["Acc"][0];
                m_Acc.y    = (float)JsonObj["Acc"][1];
                m_Acc.z    = (float)JsonObj["Acc"][2];
                
                m_Gyro.x   = (float)JsonObj["Gyro"][0];  
                m_Gyro.y   = (float)JsonObj["Gyro"][1];
                m_Gyro.z   = (float)JsonObj["Gyro"][2];
                
                m_Mag.x    = (float)JsonObj["Mag"][0];
                m_Mag.y    = (float)JsonObj["Mag"][1];
                m_Mag.z    = (float)JsonObj["Mag"][2];
                
                Yaw        = (float)JsonObj["Orientation"][0];
                Pitch      = (float)JsonObj["Orientation"][1];
                Roll       = (float)JsonObj["Orientation"][2];
                
                m_W        = (float)JsonObj["Quat"][0];  
                m_X        = (float)JsonObj["Quat"][1]; 
                m_Y        = (float)JsonObj["Quat"][2]; 
                m_Z        = (float)JsonObj["Quat"][3]; 
                
                
                HeartRate  = (float)JsonObj["Heart_Rate"];
            }

            else if (flag == "GPS")
            {
                //std::cout << "GPS Trigger" << std::endl;

                Long       = (float)JsonObj["Gps"][0];
                Lat        = (float)JsonObj["Gps"][1];
                //Speed      = (float)JsonObj["Gps"][2].toDouble();
                //SpeedCours = (float)JsonObj["Gps"][3].toDouble();
                
                //std::cout << s_GPSTrigger << std::endl;
                if (s_GPSTrigger < AVERAGE_SIZE)
                    ConvertLatLongToMeters(Lat, Long);
                
                s_GPSTrigger++; //= (s_GPSTrigger + 1) % AVERAGE_SIZE;
            }
        }
        catch (...)
        {
          //EZZOO_CORE_ERROR("Invalid JSON Object");
          //return;
        }

        float accXMeter = m_Acc.x * 9.80665f;
        float accYMeter = m_Acc.y * 9.80665f;
        float accZMeter = m_Acc.z * 9.80665f;        
        

        //Eigen::Vector3f nAcc = *(Eigen::Vector3f*)&acc;
        Eigen::Quaternionf eQ = Eigen::Quaternionf(m_W, m_X, m_Y, m_Z);

        float now = (float)glfwGetTime(); // seconds 
        float dt = 0.02f;
        if (LastTime !=0)
        {
            dt = (now  - LastTime) ;/// 1000000.0f;
            if (dt <= 0 || dt > 0.5f) 
            {
                dt = 0.02f;
            }
            
        }
        LastTime = now ;                        
        m_KalmanFilter.Predict(eQ, {accXMeter, accYMeter, accZMeter}, dt, 0.5f, 0.01f);
        
        
       static size_t f = 0;
       if (f ==  0 && Lat > 0.0f && Long > 0.0f)
       {
           m_RefLat = Lat;
           m_RefLng = Long;   
           f ++;
        }
        
        
        if (s_GPSTrigger >= AVERAGE_SIZE)
        {
            auto XY = ProcessGPS();
            s_GPSTrigger = 0 ;
            //auto SpeedXY = ConvertSpeedToVedctor(Speed, SpeedCours);
            //m_KalmanFilter.UpdateVelocityZUPT({SpeedXY.first, SpeedXY.second, 0.0f}, Eigen::Matrix3f::Identity() * 0.5);
            m_KalmanFilter.UpdatePosition({XY.first, XY.second, 0.0f}, Eigen::Matrix3f::Identity() * 0.5f );
        }   
        
        PosState.x = m_KalmanFilter.x(0);
        PosState.y = m_KalmanFilter.x(1);
        PosState.z = m_KalmanFilter.x(2);

        VelState.x = m_KalmanFilter.x(3);
        VelState.y = m_KalmanFilter.x(4);
        VelState.z = m_KalmanFilter.x(5);

    }

    std::pair<float, float> FusionCalculation::ConvertSpeedToVedctor(float speed, float speedCours)
    {
        float sx, sy = 0;

        float coursRad = speedCours * M_PI / 180.0f;
        sx = speed * sinf(coursRad);
        sy = speed * cosf(coursRad);

        return {sx, sy};

    }

    void FusionCalculation::ConvertLatLongToMeters (float lat, float lng)
    {
        float x, y = 0.0f;
        
        //if ((lat - m_RefLat) <= 0.00003f || (lng - m_RefLng) <= 0.00003f) return {xf, yf};

        m_Index = (m_Index + 1) % AVERAGE_SIZE;
        float Lat_Rad = lat * M_PI / 180.0f;
        float Lng_Rad = lng * M_PI / 180.0f;
        
        float Lat_Ref_Rad = m_RefLat * M_PI / 180.0f;
        float Lng_Ref_Rad = m_RefLng * M_PI / 180.0f;

        x = (Lng_Rad - Lng_Ref_Rad) * 6378137.0f * cosf(Lat_Ref_Rad);
        y = (Lat_Rad - Lat_Ref_Rad) * 6378137.0f ;

        m_xBuf[m_Index] = x;
        m_yBuf[m_Index] = y;
       
        //ProcessGPS (x, y, xf, yf);

       // return {xf, yf};

    }

    float FusionCalculation::Average(float *arr, int size)
    {
        float temp[size];
        memcpy(temp, arr, size * sizeof(float));
        
        for (size_t i = 0; i < size-1; i++)
        {
            for (size_t j = i + 1; j < size; j++)
            {
                if (temp[j] < temp[i])
                {
                    float t = temp[i];
                    temp[i] = temp[j];
                    temp[j] = t;
                }
            }
        }

        return temp[size / 2];

    }

    std::pair <float, float> FusionCalculation::ProcessGPS ()
    {
       

        //m_Index = (m_Index + 1) % AVERAGE_SIZE;

        //if (m_Index == 0) m_Filled = true;

        //float xMed = x;
        //float yMed = y;
        //if (m_Filled)
        //{
            /*xMed*/float xf = Average(m_xBuf, AVERAGE_SIZE);
            /*yMed*/float yf = Average(m_yBuf, AVERAGE_SIZE);
        //}

        //if (!m_Initialized)
        //{
        //    m_xLP = xMed;
        //    m_yLP = yMed;
        //    m_Initialized = true;
        //}

        //m_xLP = ALPHA * xMed + (1.0f - ALPHA) * m_xLP;
        //m_yLP = ALPHA * yMed + (1.0f - ALPHA) * m_yLP;

        //if ((fabs(m_xLP - xf) < DEAD_BAND)) m_xLP = xf;
        //if ((fabs(m_yLP - yf) < DEAD_BAND)) m_yLP = yf;
        //else 
        //{
        //    xf = x;//m_xLP;
        //    yf = y;//m_yLP;
        //}

        return {xf, yf};

    }

    void FusionCalculation::OnImGuiRender ()
    {
       // ImGui::SetNextWindowPos(ImVec2(1000, 100));
        //ImGui::SetNextWindowSize(ImVec2(250, 370));
    
        ImGui::SetNextWindowPos(ImVec2{685.0f, 40.0f});
        ImGui::Begin("Fusion Values");

        ImGui::InputFloat("RefLng", &m_RefLng, 0.0f, 0.0f, "%0.5f");
        ImGui::InputFloat("RefLat", &m_RefLat, 0.0f, 0.0f, "%0.5f");

        ImGui::Separator();
        ImGui::Separator();

        //ImGui::InputText("PlayerName", )
        ImGui::Text("PlayerName: %s\n", PlayerName.c_str());
        ImGui::Text("Long: %0.5f\n",  Long);
        ImGui::Text("Lat:  %0.5f\n",  Lat);
        ImGui::Text("Speed:%0.2f",    Speed);

        ImGui::Separator();
        ImGui::Separator();

        ImGui::Text("Acc:  [%0.2f, %0.2f, %0.2f]\n", m_Acc.x, m_Acc.y, m_Acc.z);
        ImGui::Text("Gyro: [%0.2f, %0.2f, %0.2f]\n", m_Gyro.x, m_Gyro.y, m_Gyro.z);
        ImGui::Text("Mag:  [%0.2f, %0.2f, %0.2f]", m_Mag.x, m_Mag.y, m_Mag.z);

        ImGui::Separator();
        ImGui::Separator();
        
        //m_BufferMutex.lock();
        ImGui::Text("Roll:   %0.2f\n", Roll);
        ImGui::Text("Pitch: %0.2f\n",  Pitch);
        ImGui::Text("Yaw:  %0.2f",     Yaw);
        //m_BufferMutex.unlock();

        ImGui::Separator();
        ImGui::Separator();

        ImGui::Text("Pos:[%0.2f, %0.2f, %0.2f]\n", m_KalmanFilter.x(0), m_KalmanFilter.x(1), m_KalmanFilter.x(2));
        ImGui::Text("Velocity:[%0.2f, %0.2f, %0.2f]\n",  m_KalmanFilter.x(3), m_KalmanFilter.x(4), m_KalmanFilter.x(5));

        ImGui::Separator();
        ImGui::Separator();

        ImGui::Text("HR: %u\n", HeartRate);

        ImGui::End();
        
    }
}
#pragma once 

//#include <mutex>
#include "Buffer.h"

#include "Core/Timer.h"
#include "KalmanFilter.h"

#define AVERAGE_SIZE 4
#define ALPHA 0.2f
#define DEAD_BAND 0.5f

namespace Ezzoo {
    
    class FusionCalculation 
    {
        public :
            FusionCalculation(float sampleRate = 0.0f);
            ~FusionCalculation();
            
            void Attach ();
            void Update (Buffer& buffer); 
            void OnImGuiRender ();

            const glm::vec3& GetPosState () const { return m_PosState;}
            const glm::vec3& GetVelState () const { return m_VelState;}
            
            Configuration& GetKalmanConfig ();
            void SetKalmanConfig(const Configuration& conf);
            
        public :  
           std::string PlayerName = "";
           
           float Speed = 0.0f;
           float SpeedCours = 0.0f;
           float Long = 0.0f;
           float Lat = 0.0f;  

           float Roll = 0.0f;
           float Pitch = 0.0f;
           float Yaw = 0.0f;
   
           
           uint16_t HeartRate = 0;
           private :
           void ConvertLatLongToMeters (float lat, float lng);
           std::pair<float, float> ConvertSpeedToVedctor(float speed, float speedCours);
           std::pair <float, float>  ProcessGPS ();
           float Average(float *arr, int size);
           private :
           
           
           float m_W = 1.0f;
            float m_X = 0.0f;
            float m_Y = 0.0f;
            float m_Z = 0.0f;

            float LastTime = 0.0f;
            //Timer m_Time ;
            
            float m_RefLng = 0.00000f; 
            float m_RefLat = 0.00000f;

            float m_xBuf[AVERAGE_SIZE];
            float m_yBuf[AVERAGE_SIZE];
            int m_Index = 0;
            
            KalmanFilter m_KalmanFilter;
            
            glm::vec3 m_PosState;
            glm::vec3 m_VelState;
            
        private :
            glm::vec3 m_Acc = glm::vec3(0.0f, 0.0f, 0.0f); 
            glm::vec3 m_Gyro = glm::vec3(0.0f, 0.0f, 0.0f); 
            glm::vec3 m_Mag = glm::vec3(0.0f, 0.0f, 0.0f);

            //std::mutex m_BufferMutex;
    };
}
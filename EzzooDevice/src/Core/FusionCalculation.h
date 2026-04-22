#pragma once 

//#include <mutex>
#include "Buffer.h"

#include "Core/Timer.h"


#define AVERAGE_SIZE 4
#define ALPHA 0.2f
#define DEAD_BAND 0.5f
constexpr int NX  = 9;



namespace Ezzoo {
    



/*struct KalmanFilter {
    // state: [p(3); v(3); ba(3)]
    Eigen::Matrix<float,9,1> x;
    Eigen::Matrix<float,9,9> P;

    // tuning / process noise
    float sigma_acc = 0.2f;      // accel noise std (m/s^2)
    float sigma_ba_rw = 1e-5f;    // bias random walk std (m/s^2 / sqrt(s))

    // gps speed measurement variance
    float R_gps_speed = 0.25f; // (m/s)^2

    // gravity (world, z-up)
    Eigen::Vector3f g = Eigen::Vector3f(0.0f, 0.0f, -9.80665f);

    KalmanFilter() {
        x.setZero();
        P.setIdentity();
        P *= 0.1f; // small initial uncertainty
    }

    // set tunings if needed
    void SetProcessNoise(float sigmaAcc, float sigmaBaRW) {
        sigma_acc = sigmaAcc;
        sigma_ba_rw = sigmaBaRW;
    }
    void SetGpsSpeedVar(float R) { R_gps_speed = R; }

    // Predict step:
    // q_wb: quaternion world <- body (rotate body -> world via q_wb * v_body)
    // a_body: accelerometer specific force in body frame (m/s^2)
    // dt: seconds
    void Predict( const Eigen::Quaternionf &q_wb, const Eigen::Vector3f &a_body, float dt) {
        // Extract state
        Eigen::Vector3f p = x.segment<3>(0);
        Eigen::Vector3f v = x.segment<3>(3);
        Eigen::Vector3f ba = x.segment<3>(6); // bias in body frame

        // 1) unbiased accel in body frame
        Eigen::Vector3f a_unbiased_body = a_body - ba;

        // 2) rotate to world
        Eigen::Matrix3f R = q_wb.toRotationMatrix(); // world <- body
        Eigen::Vector3f a_world = R * a_unbiased_body;

        // 3) add gravity (world)
        Eigen::Vector3f a_lin = a_world  + g;

        // 4) propagate mean state
        //Eigen::Vector3f p_pred = p + v * dt + 0.5f * a_lin * dt * dt;
        Eigen::Vector3f p_pred = p + v * dt + 0.5f * a_lin * dt * dt;
        Eigen::Vector3f v_pred = v + a_lin * dt;
        Eigen::Vector3f ba_pred = ba; // mean evolves as random-walk (no deterministic drift)

        x.segment<3>(0) = p_pred;
        x.segment<3>(3) = v_pred;
        x.segment<3>(6) = ba_pred;

        // 5) Build Jacobian F (9x9)
        Eigen::Matrix<float,9,9> F = Eigen::Matrix<float,9,9>::Identity();
        // dp/dv
        F.block<3,3>(0,3) = Eigen::Matrix3f::Identity() * dt;
        // dp/dba = -0.5 * dt^2 * R
        // dv/dba = -dt * R
        F.block<3,3>(0,6) = -0.5f * dt * dt * R;
        F.block<3,3>(3,6) = -dt * R;
        // ba block identity remains

        // 6) Discrete process noise Qd
        // Model: accel noise (continuous) affects pos & vel; bias RW affects ba.
        Eigen::Matrix<float,9,9> Qd = Eigen::Matrix<float,9,9>::Zero();
        double s2 = sigma_acc * sigma_acc;
        // simple isotropic design (works well in practice)
        Qd.block<3,3>(0,0) = Eigen::Matrix3f::Identity() * (s2 * dt*dt*dt*dt / 4.0f);
        Qd.block<3,3>(0,3) = Eigen::Matrix3f::Identity() * (s2 * dt*dt*dt / 2.0f);
        Qd.block<3,3>(3,0) = Qd.block<3,3>(0,3).transpose();
        Qd.block<3,3>(3,3) = Eigen::Matrix3f::Identity() * (s2 * dt*dt);
        Qd.block<3,3>(6,6) = Eigen::Matrix3f::Identity() * (sigma_ba_rw * sigma_ba_rw * dt);

        // 7) Covariance propagation: P = F P F^T + Qd
        P = F * P * F.transpose() + Qd;

        // numerical symmetrize
        P = 0.5 * (P + P.transpose());
    }

    // GPS scalar speed update (z = speed magnitude, variance R)
    // Uses linearized measurement h(x) = ||v||
    void UpdateGpsSpeed(float z_speed, float R = -1.0f) {
        if (R > 0) R_gps_speed = R;

        // get current velocity
        Eigen::Vector3f v = x.segment<3>(3);
        float vnorm = v.norm();
        if (vnorm < 1e-8) {
            // velocity nearly zero -> cannot linearize; skip or use heuristic
            return;
        }

        // H (1x9): only velocity components nonzero
        Eigen::Matrix<float,1,9> H = Eigen::Matrix<float,1,9>::Zero();
        H(0,3) = v(0) / vnorm;
        H(0,4) = v(1) / vnorm;
        H(0,5) = v(2) / vnorm;

        // innovation
        float y = z_speed - vnorm;

        // innovation covariance S
        float S = (H * P * H.transpose())(0,0) + R_gps_speed;
        if (S <= 0) S = 1e-6;

        // Kalman gain K (9x1)
        Eigen::Matrix<float,9,1> K = P * H.transpose() / S;

        // state update
        x = x + K * y;

        // covariance update (Joseph form could be used)
        Eigen::Matrix<float,9,9> I = Eigen::Matrix<float,9,9>::Identity();
        P = (I - K * H) * P;

        // symmetrize
        P = 0.5f * (P + P.transpose());
    }

    // Optional: helper that fuses GPS speed but preserves direction (heuristic) if vnorm near zero
    void UpdateGpsSpeedDirectional(float z_speed, float alpha = 0.9f) {
        Eigen::Vector3f v = x.segment<3>(3);
        float vnorm = v.norm();
        if (vnorm < 1e-6) return;
        Eigen::Vector3f v_new = v * (z_speed / vnorm);
        x.segment<3>(3) = alpha * v + (1.0 - alpha) * v_new;
    }
};*/



    class FusionCalculation 
    {
        public :
            FusionCalculation(float sampleRate = 0.0f);
            ~FusionCalculation();
            
            void Attach ();
            void Update (Buffer& buffer); 
            void OnImGuiRender ();
            
        public :  
           std::string PlayerName = "";
           
           float Speed = 0.0f;
           float SpeedCours = 0.0f;
           float Long = 0.0f;
           float Lat = 0.0f;  

           float Roll = 0.0f;
           float Pitch = 0.0f;
           float Yaw = 0.0f;
           
           //Kalman Parameter
           glm::vec3 PosState;
           glm::vec3 VelState;
           
           
           //KalmanFilter m_KalmanFilter;
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
            bool m_Filled = false;
            float xf = 0.0f;
            float yf = 0.0f;

            float m_xLP = 0.0f;
            float m_yLP = 0.0f;
            bool m_Initialized = false;

            
        private :
            glm::vec3 m_Acc = glm::vec3(0.0f, 0.0f, 0.0f); 
            glm::vec3 m_Gyro = glm::vec3(0.0f, 0.0f, 0.0f); 
            glm::vec3 m_Mag = glm::vec3(0.0f, 0.0f, 0.0f);

            //std::mutex m_BufferMutex;
    };
}
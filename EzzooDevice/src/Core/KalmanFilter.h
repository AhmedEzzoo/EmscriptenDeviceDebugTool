#pragma once 


#include <Eigen/Dense>



constexpr int NX  = 9;


namespace Ezzoo {

    struct Configuration 
    {
        float GPSTrust = 0.0f;
        float MPUTrust = 0.0f;
        float ACCStill = 0.09f;
        float Stationary = 0.9f;
        float MaxVel = 3.0f;

        Configuration (float gpsTrust = 0.5f, float mpuTrust = 0.5f) :  GPSTrust(gpsTrust), MPUTrust(mpuTrust)
        {}

        ~Configuration() {}
    };


    class KalmanFilter 
    {

        public :
        KalmanFilter (float pos_var = 1.0f, float vel_var = 0.1f, float ba_var = 0.01f);
        ~KalmanFilter() ;
        
        void Init(const Eigen::Vector3f &p0, const Eigen::Vector3f &v0, const Eigen::Vector3f &ba0);

        void SetConfig(const Configuration& conf); 

        Configuration& GetConfig (); 
  
        void Predict(const Eigen::Quaternionf &q_wb, const Eigen::Vector3f &a_meas_body, float dt, float sigma_ba_rw);
        
        void UpdateVelocityZUPT(const Eigen::Vector3f& z_vel, const Eigen::Matrix3f& R_vel);
        
        void UpdatePosition(const Eigen::Vector3f& z_pos, const Eigen::Matrix3f& R_pos);

        const glm::vec3 GetPositionState () const 
        {
            Eigen::Vector3f pos = m_X.segment<3>(0);  
            glm::vec3 glmPos (pos.x(), pos.y(), pos.z());  
            return glmPos; 
        } 
    
        const glm::vec3 GetVelocityState() const 
        {
            Eigen::Vector3f vel = m_X.segment<3>(3);  
            glm::vec3 glmVel (vel.x(), vel.y(), vel.z());  
            return glmVel; 
        }
    
        private :

            Eigen::Matrix<float, NX, 1>  m_X;           // state vector
            Eigen::Matrix<float, NX, NX> m_P;          // covariance

            float m_PosVar; 
            float m_VelVar; 
            float m_BaVar;

            Configuration m_Configuration;




    };
}
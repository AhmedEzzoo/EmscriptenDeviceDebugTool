#include "ezpch.h"

#include "KalmanFilter.h"



namespace Ezzoo {

   KalmanFilter::KalmanFilter(float pos_var, float vel_var, float ba_var): m_PosVar(pos_var), m_VelVar(vel_var), m_BaVar(ba_var)
    {
        m_X.setZero();
        m_P.setIdentity();
        
        Init(Eigen::Vector3f::Zero(), Eigen::Vector3f::Zero(), Eigen::Vector3f::Zero());
    }

    KalmanFilter::~KalmanFilter()
    {}

    void KalmanFilter::Init(const Eigen::Vector3f &p0, const Eigen::Vector3f &v0, const Eigen::Vector3f &ba0)
    {
        m_X.segment<3>(0) = p0;
        m_X.segment<3>(3) = v0;
        m_X.segment<3>(6) = ba0;

        m_P.setZero();
        m_P.block<3,3>(0,0) = Eigen::Matrix3f::Identity() * m_PosVar;
        m_P.block<3,3>(3,3) = Eigen::Matrix3f::Identity() * m_VelVar;
        m_P.block<3,3>(6,6) = Eigen::Matrix3f::Identity() * m_BaVar;
    }

    void KalmanFilter::SetConfig(const Configuration& conf) 
    { 
        m_Configuration.GPSTrust = conf.GPSTrust;
        m_Configuration.MPUTrust = conf.MPUTrust;
                
    }

    Configuration& KalmanFilter::GetConfig () 
    { 
        return m_Configuration;
    }

    void KalmanFilter::Predict(const Eigen::Quaternionf &q_wb, const Eigen::Vector3f &a_meas_body, float dt,  float sigma_ba_rw)
    {
        const Eigen::Vector3f g(0.0f,0.0f,-9.80665f);

        Eigen::Vector3f p  = m_X.segment<3>(0);
        Eigen::Vector3f v  = m_X.segment<3>(3);
        Eigen::Vector3f ba = m_X.segment<3>(6);

        Eigen::Vector3f a_unbiased_body = a_meas_body - ba;
        Eigen::Vector3f a_world = q_wb * a_unbiased_body;
        Eigen::Vector3f a_wg = a_world + g;

        for(int i=0;i<3;i++)
        {
        if(fabs(a_wg[i]) < m_Configuration.ACCStill)
                a_wg[i] = 0;
        }

        Eigen::Vector3f p_pred  = p + v*dt + 0.5f *a_wg*dt*dt;
        Eigen::Vector3f v_pred  = v + a_wg*dt;
        Eigen::Vector3f ba_pred = ba;


        bool stationary = (a_wg.norm() < m_Configuration.Stationary);
 
        if(stationary)
            v_pred.setZero();
        else
            v_pred *= 0.995f;

        float maxVel = m_Configuration.MaxVel;

        if(v_pred.norm() > maxVel)
            v_pred = v_pred.normalized() * maxVel;


        m_X.segment<3>(0) = p_pred;
        m_X.segment<3>(3) = v_pred;
        m_X.segment<3>(6) = ba_pred;

        Eigen::Matrix<float, NX, NX> F = Eigen::Matrix<float, NX, NX>::Identity();
        F.block<3,3>(0,3) = Eigen::Matrix3f::Identity() * dt;

        Eigen::Matrix3f R = q_wb.toRotationMatrix();
        Eigen::Matrix3f dA_dba = -R;

        F.block<3,3>(0,6) = 0.5f * dt*dt * dA_dba;
        F.block<3,3>(3,6) = dt * dA_dba;

        Eigen::Matrix<float, NX, NX> Qd = Eigen::Matrix<float, NX, NX>::Zero();
        float mpuTrust = m_Configuration.MPUTrust;
        float s2 = mpuTrust * mpuTrust;

        Qd.block<3,3>(0,0) = Eigen::Matrix3f::Identity() * (s2 * dt*dt*dt*dt / 4.0f);
        Qd.block<3,3>(0,3) = Eigen::Matrix3f::Identity() * (s2 * dt*dt*dt / 2.0f);
        Qd.block<3,3>(3,0) = Qd.block<3,3>(0,3).transpose();
        Qd.block<3,3>(3,3) = Eigen::Matrix3f::Identity() * (s2 * dt*dt);
        Qd.block<3,3>(6,6) = Eigen::Matrix3f::Identity() * (sigma_ba_rw*sigma_ba_rw * dt);

        m_P = F * m_P * F.transpose() + Qd;
        m_P = 0.5f * (m_P + m_P.transpose());
    }

    void KalmanFilter::UpdateVelocityZUPT(const Eigen::Vector3f& z_vel, const Eigen::Matrix3f& R_vel)
    {
        Eigen::Matrix<float,3,NX> H = Eigen::Matrix<float,3,NX>::Zero();
        H.block<3,3>(0,3) = Eigen::Matrix3f::Identity();

        Eigen::Vector3f y = z_vel - H * m_X;

        Eigen::Matrix3f S = H * m_P * H.transpose() + R_vel;
        Eigen::Matrix<float,NX,3> K = m_P * H.transpose() * S.inverse();

        m_X = m_X + K * y;

        Eigen::Matrix<float,NX,NX> I = Eigen::Matrix<float,NX,NX>::Identity();
        m_P = (I - K * H) * m_P;

        m_P = 0.5f * (m_P + m_P.transpose());
    }

    void KalmanFilter::UpdatePosition(const Eigen::Vector3f& z_pos, const Eigen::Matrix3f& R_pos)
    {

        Eigen::Matrix3f rPos = R_pos * m_Configuration.GPSTrust;

        Eigen::Matrix<float,3,NX> H = Eigen::Matrix<float,3,NX>::Zero();
        H.block<3,3>(0,0) = Eigen::Matrix3f::Identity();

        Eigen::Vector3f y = z_pos - H * m_X;
        Eigen::Matrix3f S = H * m_P * H.transpose() + rPos;
        Eigen::Matrix<float,NX,3> K = m_P * H.transpose() * S.inverse();

        m_X = m_X + K * y;

        Eigen::Matrix<float,NX,NX> I = Eigen::Matrix<float,NX,NX>::Identity();
        m_P = (I - K * H) * m_P;

        m_P = 0.5f * (m_P + m_P.transpose());
    }



}
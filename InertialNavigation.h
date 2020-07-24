////////////////////////////////////////////////////////////////////////////////////////////////////
// Class for simple timer.																		  //
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef INERTIAL_NAVIGATION_H
#define INERTIAL_NAVIGATION_H

#include <Wire.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
class CINS
{
public:
    CINS();

    void Init();
    void Tick( const float deltaTimeSec );

    // Get acceleration data
    float   GetAcclX() const { return m_acclValueX; }
    float   GetAcclY() const { return m_acclValueY; }
    float   GetAcclZ() const { return m_acclValueZ; }
    float   GetAcclS() const { return m_acclValueS; }

    // Get speed data
    float   GetSpeedX() const { return m_speedX; }
    float   GetSpeedY() const { return m_speedY; }
    float   GetSpeedZ() const { return m_speedZ; }
    float   GetSpeedS() const { return m_speedS; }
    float   GetSpeedK() const { return m_speedK; }

    // Get position data
    float   GetPosX() const { return m_posX; }
    float   GetPosY() const { return m_posY; }
    float   GetPosZ() const { return m_posZ; }
    float   GetDist() const { return m_dist; }

    // Get gyroscope data
    //int16_t GetGyroX() const { return m_gyroX; }
    //int16_t GetGyroY() const { return m_gyroY; }
    //int16_t GetGyroZ() const { return m_gyroZ; }

private:
    //MPU6050 m_device;

    void    CollectErrors();
    void    ReadAcceleration();
    void    CorrectValues();
    void    CalculateParams( const float deltaTimeSec );
    float   GetMagnitude( const float x, const float y, const float z );

    // Readable data for acceleration
    float   m_acclValueX;
    float   m_acclValueY;
    float   m_acclValueZ;
    float   m_acclValueS;

    // Errors for acceleration
    float   m_acclErrorX;
    float   m_acclErrorY;
    float   m_acclErrorZ;

    // Speed section
    float   m_speedX;
    float   m_speedY;
    float   m_speedZ;
    float   m_speedS;
    float   m_speedK;

    // Position section
    float   m_posX;
    float   m_posY;
    float   m_posZ;

    // Distance from start point
    float   m_dist;
};
////////////////////////////////////////////////////////////////////////////////////////////////////
#endif


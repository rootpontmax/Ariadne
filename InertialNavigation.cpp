#include "InertialNavigation.h"

#include <Math.h>

//#include <Wire.h>
//#include <SoftwareSerial.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
static const int    MPU = 0x68; // MPU6050 I2C address
static const int    ERROR_MEASURE_COUNT = 5000;
static const float  GRAVITY = 9.80665f;
////////////////////////////////////////////////////////////////////////////////////////////////////
CINS::CINS() :
    m_acclValueX( 0.0f ),
    m_acclValueY( 0.0f ),
    m_acclValueZ( 0.0f ),
    m_acclValueS( 0.0f ),
    m_acclErrorX( 0.0f ),
    m_acclErrorY( 0.0f ),
    m_acclErrorZ( 0.0f ),
    m_speedS( 0.0f ),
    m_speedK( 0.0f ),
    m_posX( 0.0f ),
    m_posY( 0.0f ),
    m_posZ( 0.0f ),
    m_dist( 0.0f )
{}
////////////////////////////////////////////////////////////////////////////////////////////////////
void CINS::Init()
{
    // Default range (+/- 2g)
    Wire.begin();                   // Initialize comunication
    Wire.beginTransmission( MPU );  // Start communication with MPU6050 // MPU=0x68
    Wire.write( 0x6B );             // Talk to the register 6B
    Wire.write( 0x00 );             // Make reset - place a 0 into the 6B register
    Wire.endTransmission( true );

    CollectErrors();
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void CINS::Tick( const float deltaTimeSec )
{
    ReadAcceleration();
    CorrectValues();
    CalculateParams( deltaTimeSec );

    
    

    //m_rawTempr = Wire.read() << 8 | Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
    //m_rawGyroX = Wire.read() << 8 | Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
    //m_rawGyroY = Wire.read() << 8 | Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
    //m_rawGyroZ = Wire.read() << 8 | Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)

    //Serial.print("AcX = "); Serial.print(AcX);
    //Serial.print(" | AcY = "); Serial.print(AcY);
    //Serial.print(" | AcZ = "); Serial.print(AcZ);
    //Serial.print(" | Tmp = "); Serial.print(Tmp/340.00+36.53);  //equation for temperature in degrees C from datasheet
    //Serial.print(" | GyX = "); Serial.print(GyX);
    //Serial.print(" | GyY = "); Serial.print(GyY);
    //Serial.print(" | GyZ = "); Serial.println(GyZ);
    //delay(333);
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void CINS::CollectErrors()
{
    m_acclErrorX = 0.0f;
    m_acclErrorY = 0.0f;
    m_acclErrorZ = 0.0f;

    for( int i = 0; i < ERROR_MEASURE_COUNT; ++i )
    {
        ReadAcceleration();

        m_acclErrorX += m_acclValueX;
        m_acclErrorY += m_acclValueY;
        m_acclErrorZ += m_acclValueZ;
    }

    const float typedError = static_cast< float >( ERROR_MEASURE_COUNT );
    m_acclErrorX /= typedError;
    m_acclErrorY /= typedError;
    m_acclErrorZ /= typedError;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void CINS::ReadAcceleration()
{
    Wire.beginTransmission( MPU );
    Wire.write( 0x3B );  // starting with register 0x3B (ACCEL_XOUT_H)
    Wire.endTransmission( false );
    //Wire.requestFrom( MPU, 14, true );  // request a total of 14 registers
    Wire.requestFrom( MPU, 6, true ); // Read 6 registers total, each axis value is stored in 2 registers

    const int16_t rawAcclX = Wire.read() << 8 | Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
    const int16_t rawAcclY = Wire.read() << 8 | Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
    const int16_t rawAcclZ = Wire.read() << 8 | Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)

    m_acclValueX = static_cast< float >( rawAcclX ) / 16384.0f;
    m_acclValueY = static_cast< float >( rawAcclY ) / 16384.0f;
    m_acclValueZ = static_cast< float >( rawAcclZ ) / 16384.0f;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void CINS::CorrectValues()
{
    // Correct by errors
    m_acclValueX -= m_acclErrorX;
    m_acclValueY -= m_acclErrorY;
    m_acclValueZ -= m_acclErrorZ;

    m_acclValueX *= GRAVITY;
    m_acclValueY *= GRAVITY;
    m_acclValueZ *= GRAVITY;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void CINS::CalculateParams( const float deltaTimeSec )
{
    // Abs acceleration
    m_acclValueS = GetMagnitude( m_acclValueX, m_acclValueY, m_acclValueZ );

    // Speed calculation
    m_speedX += m_acclValueX * deltaTimeSec;
    m_speedY += m_acclValueY * deltaTimeSec;
    m_speedZ += m_acclValueZ * deltaTimeSec;
    m_speedS = GetMagnitude( m_speedX, m_speedY, m_speedZ );
    m_speedK = m_speedS / 3600.0f;

    // Position calculation
    m_posX += m_speedX * deltaTimeSec;
    m_posY += m_speedY * deltaTimeSec;
    m_posZ += m_speedZ * deltaTimeSec;

    // Distance calculation
    m_dist = GetMagnitude( m_posX, m_posY, m_posZ );
}
////////////////////////////////////////////////////////////////////////////////////////////////////
float CINS::GetMagnitude( const float x, const float y, const float z )
{
    const float sqrLen = x * x + y * y + z * z;
    return sqrt( sqrLen );
}
////////////////////////////////////////////////////////////////////////////////////////////////////
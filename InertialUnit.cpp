#include "InertialUnit.h"

#include <Arduino.h>

#include "RadixSort.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
const int AVERAGE_COUNT = 100;
const int AVERAGE_BEGIN = 200;
const int CALIBRATION_COUNT = 500;
////////////////////////////////////////////////////////////////////////////////////////////////////
static const int MPU_ADDRESS = 0x68;
static const float GRAVITY = 9.81f;
////////////////////////////////////////////////////////////////////////////////////////////////////
//static const float ACCL_FULL_RANGE = 8192.0f; // [LSB/g]
static const float ACCL_FULL_RANGE = 16384.0f; // [LSB/g]
static const float GYRO_FULL_RANGE = 131.0f; // [LSB/deg/sec]
////////////////////////////////////////////////////////////////////////////////////////////////////
static const float ACCL_COEF = GRAVITY / ACCL_FULL_RANGE;
static const float GYRO_COEF = 1.0f / GYRO_FULL_RANGE;
////////////////////////////////////////////////////////////////////////////////////////////////////
CInertialUnit::CInertialUnit() :
   m_rawAcclX( 0 ),
   m_rawAcclY( 0 ),
   m_rawAcclZ( 0 ),

   m_rawGyroX( 0 ),
   m_rawGyroY( 0 ),
   m_rawGyroZ( 0 ),

   m_gyroErrorX( 0.0f ),
   m_gyroErrorY( 0.0f ),
   m_gyroErrorZ( 0.0f ),

   m_acclX( 0.0f ),
   m_acclY( 0.0f ),
   m_acclZ( 0.0f ),

   m_gyroX( 0.0f ),
   m_gyroY( 0.0f ),
   m_gyroZ( 0.0f )
{}
////////////////////////////////////////////////////////////////////////////////////////////////////
void CInertialUnit::Init()
{
   Setup();
   Calibrate();
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void CInertialUnit::Tick()
{
   //ReadAccl();
   ReadGyro();
   ConvertGyro();

   // Compensate the error of gyroscope
   m_gyroX -= m_gyroErrorX;
   m_gyroY -= m_gyroErrorY;
   m_gyroZ -= m_gyroErrorZ;
   
   /*
   Serial.print( m_gyroY );
   Serial.print( " " );
   Serial.print( m_filteredGyroY );
   Serial.print( " " );

   Serial.print( m_gyroZ );
   Serial.print( " " );
   Serial.print( m_filteredGyroZ );
   Serial.print( " " );
   */

   //Serial.print( m_gyroZ );
   //Serial.print( " " );
   //Serial.println();

   /*

   // Get acceleration in C-system
   m_acclX = static_cast< float >( m_rawAcclX ) * ACCL_COEF;
   m_acclY = static_cast< float >( m_rawAcclY ) * ACCL_COEF;
   m_acclZ = static_cast< float >( m_rawAcclZ ) * ACCL_COEF;
   */
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void CInertialUnit::Setup()
{
   // https://howtomechatronics.com/tutorials/arduino/arduino-and-mpu6050-accelerometer-and-gyroscope-tutorial/
   // https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Register-Map1.pdf

   // Start I2C
   Wire.begin();
   
   // Activate the MPU-6050
   Wire.beginTransmission( MPU_ADDRESS ); // Start communicating with the MPU-6050
   Wire.write( 0x6B ); // Talk to the register 6B.
   Wire.write( 0x00 ); // Make reset - place a 0 into the 6B register
   Wire.endTransmission( true );

   // Configure Accelerometer Sensitivity - Full Scale Range (default +/- 2g)
   Wire.beginTransmission( MPU_ADDRESS );
   Wire.write( 0x1C );   // Talk to the ACCEL_CONFIG register (1C hex)
   Wire.write( 0x00 );   // Set the register bits as 00000000 (+/- 2g full scale range)
   //Wire.write( 0x08 );    // Set the register bits as 00001000 (+/- 4g full scale range)
   //Wire.write( 0x10 );    // Set the register bits as 00010000 (+/- 8g full scale range)
   Wire.endTransmission( true );

   // Configure Gyro Sensitivity - Full Scale Range (default +/- 250deg/s)
   Wire.beginTransmission( MPU_ADDRESS );
   Wire.write( 0x1B );      // Talk to the GYRO_CONFIG register (1B hex)
   Wire.write( 0x00 );     // Set the register bits as 00000000 (250deg/s full scale)
   //Wire.write( 0x10 );     // Set the register bits as 00010000 (1000deg/s full scale)
   Wire.endTransmission( true );
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void CInertialUnit::Calibrate()
{
   float gyroWithError[CALIBRATION_COUNT];

   // For axis X
   for( int i = 0; i < CALIBRATION_COUNT; ++i )
   {
      ReadGyro();
      ConvertGyro();
      gyroWithError[i] = m_gyroX;
   }
   m_gyroErrorX = CalcGyroError( gyroWithError );


   // For axis Y
   for( int i = 0; i < CALIBRATION_COUNT; ++i )
   {
      ReadGyro();
      ConvertGyro();
      gyroWithError[i] = m_gyroY;
   }
   m_gyroErrorY = CalcGyroError( gyroWithError );

   // For axis Z
   for( int i = 0; i < CALIBRATION_COUNT; ++i )
   {
      ReadGyro();
      ConvertGyro();
      gyroWithError[i] = m_gyroZ;
   }
   m_gyroErrorZ = CalcGyroError( gyroWithError );
}
////////////////////////////////////////////////////////////////////////////////////////////////////
float CInertialUnit::CalcGyroError( float *pBuffer )
{
   float temp[CALIBRATION_COUNT];

   // Sort
   uint8_t *pSortBuffer = reinterpret_cast< uint8_t* >( pBuffer );
   uint8_t *pTempBuffer = reinterpret_cast< uint8_t* >( temp );
   RadixSort( pSortBuffer, pTempBuffer, CALIBRATION_COUNT, sizeof( float ) );

   /*
   // Output values
   for( int i = 0; i < CALIBRATION_COUNT; ++i )
   {
      Serial.print( pBuffer[i] );
      Serial.print( " " );
      Serial.println();
   }
   //*/

   // Calculate average from central part of the sorted arrays
   float avgGyro = 0.0f;
   for( int i = 0; i < AVERAGE_COUNT; ++i )
      avgGyro += pBuffer[i + AVERAGE_BEGIN];


   avgGyro = avgGyro / AVERAGE_COUNT;
   return avgGyro;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void CInertialUnit::ReadData()
{
   //Serial.println( "Reading..." );

   // Read acceleration data
   Wire.beginTransmission( MPU_ADDRESS );
   Wire.write( 0x3B ); // Start with register 0x3B (ACCEL_XOUT_H)
   Wire.endTransmission( false );
   //Wire.requestFrom( MPU_ADDRESS, 6, true );

   Wire.requestFrom( MPU_ADDRESS, 14 );
   /*
   // Wait until all the bytes are received
   if( Wire.available() < 14 )
      return;
   */

   while( Wire.available() < 14 );

   m_rawAcclX = ( Wire.read() << 8 ) | Wire.read();
   m_rawAcclY = ( Wire.read() << 8 ) | Wire.read();
   m_rawAcclZ = ( Wire.read() << 8 ) | Wire.read();
   const int16_t temp = ( Wire.read() << 8 ) | Wire.read();                                   
   m_rawGyroX = Wire.read() << 8 | Wire.read();
   m_rawGyroY = Wire.read() << 8 | Wire.read();
   m_rawGyroZ = Wire.read() << 8 | Wire.read();
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void CInertialUnit::ReadAccl()
{
   // Read acceleration data
   Wire.beginTransmission( MPU_ADDRESS );
   Wire.write( 0x3B ); // Start with register 0x3B (ACCEL_XOUT_H)
   Wire.endTransmission( false );
   Wire.requestFrom( MPU_ADDRESS, 6 );
   // Wait until all the bytes are received
   while( Wire.available() < 6 );

   m_rawAcclX = ( Wire.read() << 8 ) | Wire.read();
   m_rawAcclY = ( Wire.read() << 8 ) | Wire.read();
   m_rawAcclZ = ( Wire.read() << 8 ) | Wire.read();
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void CInertialUnit::ReadGyro()
{
   Wire.beginTransmission( MPU_ADDRESS );
   Wire.write( 0x43 ); // Gyro data first register address 0x43
   Wire.endTransmission( false );

   Wire.requestFrom( MPU_ADDRESS, 6 );
   while( Wire.available() < 6 );

   m_rawGyroX = Wire.read() << 8 | Wire.read();
   m_rawGyroY = Wire.read() << 8 | Wire.read();
   m_rawGyroZ = Wire.read() << 8 | Wire.read();
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void CInertialUnit::ConvertGyro()
{
   // Get gyroscope reading in C-system
   m_gyroX = static_cast< float >( m_rawGyroX ) * GYRO_COEF;
   m_gyroY = static_cast< float >( m_rawGyroY ) * GYRO_COEF;
   m_gyroZ = static_cast< float >( m_rawGyroZ ) * GYRO_COEF;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "InertialUnit.h"

#include <Arduino.h>

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
	//ReadData();
	ReadAccl();
	ReadGyro();

  	// Get acceleration in C-system
  	m_acclX = static_cast< float >( m_rawAcclX ) * ACCL_COEF;
  	m_acclY = static_cast< float >( m_rawAcclY ) * ACCL_COEF;
  	m_acclZ = static_cast< float >( m_rawAcclZ ) * ACCL_COEF;

  	// Get gyroscope reading in C-system
  	m_gyroX = static_cast< float >( m_rawGyroX ) * GYRO_COEF;
  	m_gyroY = static_cast< float >( m_rawGyroY ) * GYRO_COEF;
  	m_gyroZ = static_cast< float >( m_rawGyroZ ) * GYRO_COEF;

  	// Compensate the error of gyroscope
  	m_gyroX -= m_gyroErrorX;
  	m_gyroY -= m_gyroErrorY;
  	m_gyroZ -= m_gyroErrorZ;
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
  	Wire.write( 0x1C ); 	// Talk to the ACCEL_CONFIG register (1C hex)
  	Wire.write( 0x00 ); 	// Set the register bits as 00000000 (+/- 2g full scale range)
  	//Wire.write( 0x08 ); 	// Set the register bits as 00001000 (+/- 4g full scale range)
  	//Wire.write( 0x10 ); 	// Set the register bits as 00010000 (+/- 8g full scale range)
  	Wire.endTransmission( true );

	// Configure Gyro Sensitivity - Full Scale Range (default +/- 250deg/s)
  	Wire.beginTransmission( MPU_ADDRESS );
  	Wire.write( 0x1B );		// Talk to the GYRO_CONFIG register (1B hex)
  	Wire.write( 0x00 );     // Set the register bits as 00000000 (250deg/s full scale)
  	//Wire.write( 0x10 );     // Set the register bits as 00010000 (1000deg/s full scale)
  	Wire.endTransmission( true );

	/*
                                              
	//Configure the accelerometer (+/-8g)
  
	// Start communicating with the MPU-6050
  	Wire.beginTransmission(0x68);

  	// Send the requested starting register
  	Wire.write(0x1C);
  	
  	// Set the requested starting register
  	Wire.write(0x10); 
  
  	// End the transmission
  	Wire.endTransmission();
                                              
  	// Configure the gyro (500dps full scale)
  
  	// Start communicating with the MPU-6050
  	Wire.beginTransmission(0x68);

	// Send the requested starting register                                        
  	Wire.write(0x1B);
  	
  	// Set the requested starting register
  	Wire.write(0x08); 

  	// End the transmission
  	Wire.endTransmission();
  	*/
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void CInertialUnit::Calibrate()
{
	const int count = 10000;
	const float fltCount = static_cast< float >( count );

	int32_t gyroCalX = 0;
	int32_t gyroCalY = 0;
	int32_t gyroCalZ = 0;
	for( int i = 0; i < count; ++i )
	{
		//ReadData();
		ReadGyro();
		gyroCalX += m_rawGyroX;
		gyroCalY += m_rawGyroY;
		gyroCalZ += m_rawGyroZ;
	}

	Serial.print( "Gyro callibrate X summary: " );Serial.println( gyroCalX );
	Serial.print( "Gyro callibrate Y summary: " );Serial.println( gyroCalY );
	Serial.print( "Gyro callibrate Z summary: " );Serial.println( gyroCalZ );

	m_gyroErrorX = static_cast< float >( gyroCalX ) * GYRO_COEF / fltCount;
	m_gyroErrorY = static_cast< float >( gyroCalY ) * GYRO_COEF / fltCount;
	m_gyroErrorZ = static_cast< float >( gyroCalZ ) * GYRO_COEF / fltCount;

	Serial.print( "Gyro error X: " );Serial.println( m_gyroErrorX );
	Serial.print( "Gyro error Y: " );Serial.println( m_gyroErrorY );
	Serial.print( "Gyro error Z: " );Serial.println( m_gyroErrorZ );
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
  	// Wait until all the bytes are received                                       
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
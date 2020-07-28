#include "InertialUnit.h"

#include <Arduino.h>
//#include <HardwareSerial.h>
//#include <Servo.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
CInertialUnit::CInertialUnit() :
	m_acclX( 0 ),
	m_acclY( 0 ),
	m_acclZ( 0 ),
	m_gyroX( 0 ),
	m_gyroY( 0 ),
	m_gyroZ( 0 )
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
	ReadData();
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void CInertialUnit::Setup()
{
	// Start I2C
  	Wire.begin();
	
	// Activate the MPU-6050
  
  	// Start communicating with the MPU-6050
  	Wire.beginTransmission(0x68);

  	// Send the requested starting register                                       
  	Wire.write(0x6B);  

	// Set the requested starting register                                                  
	Wire.write(0x00);

	// End the transmission                                                    
	Wire.endTransmission(); 
                                              
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
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void CInertialUnit::Calibrate()
{
	const int count = 1000;
	int32_t gyroCalX = 0;
	int32_t gyroCalY = 0;
	int32_t gyroCalZ = 0;
	for( int i = 0; i < count; ++i )
	{
		ReadData();
		gyroCalX += m_gyroX;
		gyroCalY += m_gyroY;
		gyroCalZ += m_gyroZ;
	}

	Serial.print( "Gyro callibrate X: " );Serial.println( gyroCalX );
	Serial.print( "Gyro callibrate Y: " );Serial.println( gyroCalY );
	Serial.print( "Gyro callibrate Z: " );Serial.println( gyroCalZ );
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void CInertialUnit::ReadData()
{
	// Read the raw gyro and accelerometer data
 
	// Start communicating with the MPU-6050
	Wire.beginTransmission(0x68);
  	Wire.write(0x3B); // Send the requested starting register
  	Wire.endTransmission(); 

  	// Request 14 bytes from the MPU-6050                                  
  	Wire.requestFrom(0x68,14);
  	// Wait until all the bytes are received                                       
  	while( Wire.available() < 14 );
  
  	// Following statements left shift 8 bits, then bitwise OR.  
  	// Turns two 8-bit values into one 16-bit value                                       
  	m_acclX = ( Wire.read() << 8 ) | Wire.read();                                  
  	m_acclX = ( Wire.read() << 8 ) | Wire.read();                                  
  	m_acclX = ( Wire.read() << 8 ) | Wire.read();                                  
  	const int16_t temp = ( Wire.read() << 8 ) | Wire.read();                                   
  	m_gyroX = Wire.read() <<8 | Wire.read();                                 
  	m_gyroY = Wire.read() <<8 | Wire.read();                                 
  	m_gyroZ = Wire.read() <<8 | Wire.read();
}
////////////////////////////////////////////////////////////////////////////////////////////////////
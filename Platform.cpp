#include "Platform.h"

#include <Math.h>
#include <helper_3dmath.h>
#include "I2Cdev.h"
#include <MPU6050_6Axis_MotionApps20.h>

#include "Console.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
static const int    INTERRUPT_PIN = 18;
static MPU6050 		g_device;
volatile bool       g_bIsInterrupt = false;     // indicates whether MPU interrupt pin has gone high
////////////////////////////////////////////////////////////////////////////////////////////////////
static void DumpDataReady()
{
    g_bIsInterrupt = true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
CPlatform::CPlatform() :
	m_sensorFreq( 0 ),
	m_errorCount( 0 ),
	m_packetSize( 0 ),
	m_fifoCount( 0 ),
	m_bWasInit( false )
{}
////////////////////////////////////////////////////////////////////////////////////////////////////
void CPlatform::Init()
{	
	m_device.Init();

	// CRAP
	m_bWasInit = true;
	return;
	// end of CRAP

	// join I2C bus (I2Cdev library doesn't do this automatically)
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    Wire.begin();
    Wire.setClock( 400000 ); // 400kHz I2C clock. Comment this line if having compilation difficulties
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
    Fastwire::setup( 400, true );
#endif

    // Initialization
    Serial.println( "Initializing I2C devices..." );
    g_device.initialize();
    pinMode( INTERRUPT_PIN, INPUT );

    // Verify connection
    Serial.println( "Testing device connections..." );
    Serial.println( g_device.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed" );

    // Load and configure the DMP
    Serial.println( "Initializing DMP..." );
    uint8_t deviceStatus = g_device.dmpInitialize();

    // supply your own gyro offsets here, scaled for min sensitivity
    //m_device.setXGyroOffset( 220 );
    //m_device.setYGyroOffset( 76 );
    //m_device.setZGyroOffset( -85 );
    //m_device.setZAccelOffset( 1788 ); // 1688 factory default for my test chip
    //m_device.setZAccelOffset( 1688 ); // Тест

    // CRAP
    //m_device.setXAccelOffset( -800 ); // Тест
    // end of CRAP

    // make sure it worked (returns 0 if so)
    if( 0 == deviceStatus )
    {
        // turn on the DMP, now that it's ready
        Serial.println( "Enabling DMP..." );
        g_device.setDMPEnabled(true);

        // enable Arduino interrupt detection
        Serial.print("Enabling interrupt detection (Arduino external interrupt ");
        Serial.print( digitalPinToInterrupt( INTERRUPT_PIN ) );
        Serial.println(")...");
        attachInterrupt( digitalPinToInterrupt( INTERRUPT_PIN ), DumpDataReady, RISING );
        //uint8_t deviceIntStatus = m_device.getIntStatus();

        // get expected DMP packet size for later comparison
        m_packetSize = g_device.dmpGetFIFOPacketSize();

        Serial.print( "Packet size: " );
        Serial.println( m_packetSize );

        const uint8_t fullAcclRange = g_device.getFullScaleAccelRange();
        const uint8_t fullGyroRange = g_device.getFullScaleGyroRange();

        Serial.print( "Full scale acceleration range is " );
        Serial.println( fullAcclRange );
        Serial.print( "Full scale gyroscope range is " );
        Serial.println( fullGyroRange );

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        Serial.println( "DMP ready! Waiting for first interrupt..." );
        m_bWasInit = true;
    }
    else
    {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        Serial.print( "DMP Initialization failed (code " );
        Serial.print( deviceStatus );
        Serial.println( ")" );
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void CPlatform::Tick( const uint64_t microSec )
{
	m_device.Tick();
    return;
    
	if( !m_bWasInit )
        return;



    // Если было прерывание, то надо посчитать кол-во готовых данных, и сбросить флаг готовности
    if( g_bIsInterrupt )
        m_fifoCount = g_device.getFIFOCount();
    g_bIsInterrupt = false;
    ++m_sensorFreq;

    // Выходим, если данные неполные
    if( m_fifoCount < m_packetSize )
        return;

    HandleData();
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void CPlatform::ResetSensorFreqCounter()
{
	m_sensorFreq = 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void CPlatform::HandleData()
{
	// Читаем статус
    const uint8_t status = g_device.getIntStatus();

    // Переполнение FIFO буфера датчика может быть только, если код крайне неэффективный
    if( ( status & _BV( MPU6050_INTERRUPT_FIFO_OFLOW_BIT ) ) || m_fifoCount >= 1024 )
    {
        Serial.print( "FIFO overflow! " );
        Serial.println( m_fifoCount );

        // reset so we can continue cleanly
        g_device.resetFIFO();
        ++m_errorCount;
        // otherwise, check for DMP data ready interrupt (this should happen frequently)
    } 
    else if( status & _BV( MPU6050_INTERRUPT_DMP_INT_BIT ) )
        ReadDataFromSensor();
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void CPlatform::ReadDataFromSensor()
{
	Quaternion  q;              // [w, x, y, z]         quaternion container
    VectorInt16 gyro;           // [x, y, z]            gyro sensor measurements
    VectorInt16 aa;             // [x, y, z]            accel sensor measurements
    VectorInt16 aaReal;         // [x, y, z]            gravity-free accel sensor measurements
    VectorInt16 aaWorld;        // [x, y, z]            world-frame accel sensor measurements
    VectorFloat gravity;        // [x, y, z]            gravity vector
    //float ypr[3];               // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

    while( m_fifoCount >= m_packetSize )
    {
        g_device.getFIFOBytes( m_fifoBuffer, m_packetSize );
        m_fifoCount -= m_packetSize;
    
        //m_device.dmpGetAccel( &aa, m_fifoBuffer );

        g_device.dmpGetGyro( &gyro, m_fifoBuffer );
        g_device.dmpGetQuaternion( &q, m_fifoBuffer );
        g_device.dmpGetAccel( &aa, m_fifoBuffer );
        g_device.dmpGetGravity( &gravity, &q );
        g_device.dmpGetYawPitchRoll( m_ypr, &q, &gravity );
        g_device.dmpGetLinearAccel( &aaReal, &aa, &gravity );
        g_device.dmpGetLinearAccelInWorld( &aaWorld, &aaReal, &q );

        m_ypr[0] *= 180.0f / M_PI;



        // Добавляем значение в медианные фильтры
        //m_filterGyroX.Add( gyro.x );
        //m_filterGyroY.Add( gyro.y );
        //m_filterGyroZ.Add( gyro.z );

        //m_filterAcclX.Add( aaWorld.x );
        //m_filterAcclY.Add( aaWorld.y );
        //m_filterAcclZ.Add( aaWorld.z );

        //m_filterAcclX.Add( aaReal.x );
        //m_filterAcclY.Add( aaReal.y );
        //m_filterAcclZ.Add( aaReal.z );

        //m_filterAcclX.Add( aa.x );
        //m_filterAcclY.Add( aa.y );
        //m_filterAcclZ.Add( aa.z );
    }



}
////////////////////////////////////////////////////////////////////////////////////////////////////
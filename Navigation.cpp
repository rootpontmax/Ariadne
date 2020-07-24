#include "Navigation.h"

#include <Math.h>

#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"

//#include <Wire.h>
//#include <SoftwareSerial.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
static const int    INTERRUPT_PIN = 18;
//static const int    MPU = 0x68; // MPU6050 I2C address
//static const int    ERROR_MEASURE_COUNT = 5000;

////////////////////////////////////////////////////////////////////////////////////////////////////
static const float  GRAVITY = 9.80665f;
static const float  TIME_SCALE_COEF = 1000000.0f; // Microseconds in one second
static const float  ACCEL_TO_READABLE_COEF = GRAVITY / 8192.0f;
static const float  SPEED_TO_READABLE_COEF = 1.0f / 8192.0f / TIME_SCALE_COEF;
static const float  POS_TO_READABLE_COEF = SPEED_TO_READABLE_COEF / TIME_SCALE_COEF;
static const float  MS_TO_KMH_COEF = 3.6f;
////////////////////////////////////////////////////////////////////////////////////////////////////
static const int    FIXED_TIME_SHIFT = 8;
static const int    FIXED_TIME_STEP = 2 << FIXED_TIME_SHIFT;
////////////////////////////////////////////////////////////////////////////////////////////////////
MPU6050     m_device;
//MPU6050     m_device(0x69);
////////////////////////////////////////////////////////////////////////////////////////////////////
volatile bool       g_bIsInterrupt = false;     // indicates whether MPU interrupt pin has gone high
////////////////////////////////////////////////////////////////////////////////////////////////////
static void DumpDataReady()
{
    g_bIsInterrupt = true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
CNavigation::CNavigation() :
    m_realTime( 0 ),
    m_stepTime( 0 ),
    m_rawAcclX( 0 ),
    m_rawAcclY( 0 ),
    m_rawAcclZ( 0 ),
    m_rawGyroX( 0 ),
    m_rawGyroY( 0 ),
    m_rawGyroZ( 0 ),
    m_rawSpeedX( 0 ),
    m_rawSpeedY( 0 ),
    m_rawSpeedZ( 0 ),
    m_rawPosX( 0 ),
    m_rawPosY( 0 ),
    m_rawPosZ( 0 ),

    m_angleYaw( 0.0f ),
    m_anglePitch( 0.0f ),
    m_angleRoll( 0.0f ),

    m_acclX( 0.0f ),
    m_acclY( 0.0f ),
    m_acclZ( 0.0f ),
    m_acclS( 0.0f ),
    m_speedS( 0.0f ),
    m_speedK( 0.0f ),
    m_posX( 0.0f ),
    m_posY( 0.0f ),
    m_posZ( 0.0f ),
    m_dist( 0.0f ),
    m_errorCount( 0 ),
    m_sensorFreq( 0 ),
    m_packetSize( 0 ),
    m_fifoCount( 0 ),
    m_bWasInit( false )
{}
////////////////////////////////////////////////////////////////////////////////////////////////////
void CNavigation::Init()
{
    // join I2C bus (I2Cdev library doesn't do this automatically)
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    Wire.begin();
    Wire.setClock( 400000 ); // 400kHz I2C clock. Comment this line if having compilation difficulties
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
    Fastwire::setup( 400, true );
#endif

    // Initialization
    Serial.println( "Initializing I2C devices..." );
    m_device.initialize();
    pinMode( INTERRUPT_PIN, INPUT );

    // Verify connection
    Serial.println( "Testing device connections..." );
    Serial.println( m_device.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed" );

    // Load and configure the DMP
    Serial.println( "Initializing DMP..." );
    uint8_t deviceStatus = m_device.dmpInitialize();

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
        m_device.setDMPEnabled(true);

        // enable Arduino interrupt detection
        Serial.print("Enabling interrupt detection (Arduino external interrupt ");
        Serial.print( digitalPinToInterrupt( INTERRUPT_PIN ) );
        Serial.println(")...");
        attachInterrupt( digitalPinToInterrupt( INTERRUPT_PIN ), DumpDataReady, RISING );
        //uint8_t deviceIntStatus = m_device.getIntStatus();

        // get expected DMP packet size for later comparison
        m_packetSize = m_device.dmpGetFIFOPacketSize();

        Serial.print( "Packet size: " );
        Serial.println( m_packetSize );

        const uint8_t fullAcclRange = m_device.getFullScaleAccelRange();
        const uint8_t fullGyroRange = m_device.getFullScaleGyroRange();

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
void CNavigation::Tick( const uint64_t microSec )
{
    /*
    m_realTime += microSec;
    while( m_stepTime < m_realTime )
    {
        CalculateParams();
        m_stepTime += FIXED_TIME_STEP;
    }
    //*/
    HandleSensor();
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void CNavigation::ResetSensorFreqCounter()
{
    m_sensorFreq = 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void CNavigation::CalculateReadableData()
{
    m_acclX = m_rawAcclX * ACCEL_TO_READABLE_COEF;
    m_acclY = m_rawAcclY * ACCEL_TO_READABLE_COEF;
    m_acclZ = m_rawAcclZ * ACCEL_TO_READABLE_COEF;

    m_speedX = m_rawSpeedX * SPEED_TO_READABLE_COEF;
    m_speedY = m_rawSpeedY * SPEED_TO_READABLE_COEF;
    m_speedZ = m_rawSpeedZ * SPEED_TO_READABLE_COEF;

    m_posX = m_rawPosX * POS_TO_READABLE_COEF;
    m_posY = m_rawPosY * POS_TO_READABLE_COEF;
    m_posZ = m_rawPosZ * POS_TO_READABLE_COEF;

    m_acclS = GetMagnitude( m_acclX, m_acclY, m_acclZ );
    m_speedS = GetMagnitude( m_speedX, m_speedY, m_speedZ );
    m_speedK = m_speedS * MS_TO_KMH_COEF;
    m_dist = GetMagnitude( m_posX, m_posY, m_posZ );
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void CNavigation::CalculateParams()
{
    const int64_t deltaSpeedX = m_rawAcclX << FIXED_TIME_SHIFT;
    const int64_t deltaSpeedY = m_rawAcclY << FIXED_TIME_SHIFT;
    const int64_t deltaSpeedZ = m_rawAcclZ << FIXED_TIME_SHIFT;

    // Расчёт новой скорости
    m_rawSpeedX += deltaSpeedX;
    m_rawSpeedY += deltaSpeedY;
    m_rawSpeedZ += deltaSpeedZ;

    // Расчёт приращения позиции
    const int64_t deltaPosX = m_rawSpeedX << FIXED_TIME_SHIFT;
    const int64_t deltaPosY = m_rawSpeedY << FIXED_TIME_SHIFT;
    const int64_t deltaPosZ = m_rawSpeedZ << FIXED_TIME_SHIFT;

    // Расчёт новой позиции
    m_rawPosX += deltaPosX;
    m_rawPosY += deltaPosY;
    m_rawPosZ += deltaPosZ;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void CNavigation::HandleSensor()
{
    if( !m_bWasInit )
        return;

    // Если было прерывание, то надо посчитать кол-во готовых данных, и сбросить флаг готовности
    if( g_bIsInterrupt )
        m_fifoCount = m_device.getFIFOCount();
    g_bIsInterrupt = false;
    ++m_sensorFreq;

    // Выходим, если данные неполные
    if( m_fifoCount < m_packetSize )
        return;

    HandleData();
}
////////////////////////////////////////////////////////////////////////////////////////////////////
float CNavigation::GetMagnitude( const float x, const float y, const float z )
{
    const float sqrLen = x * x + y * y + z * z;
    return sqrt( sqrLen );
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void CNavigation::HandleData()
{
    // Читаем статус
    const uint8_t status = m_device.getIntStatus();

    // Переполнение FIFO буфера датчика может быть только, если код крайне неэффективный
    if( ( status & _BV( MPU6050_INTERRUPT_FIFO_OFLOW_BIT ) ) || m_fifoCount >= 1024 )
    {
        Serial.print( "FIFO overflow! " );
        Serial.println( m_fifoCount );

        // reset so we can continue cleanly
        m_device.resetFIFO();
        ++m_errorCount;
        // otherwise, check for DMP data ready interrupt (this should happen frequently)
    } 
    else if( status & _BV( MPU6050_INTERRUPT_DMP_INT_BIT ) )
        ReadDataFromSensor();
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void CNavigation::ReadDataFromSensor()
{
    Quaternion  q;              // [w, x, y, z]         quaternion container
    VectorInt16 gyro;           // [x, y, z]            gyro sensor measurements
    VectorInt16 aa;             // [x, y, z]            accel sensor measurements
    VectorInt16 aaReal;         // [x, y, z]            gravity-free accel sensor measurements
    VectorInt16 aaWorld;        // [x, y, z]            world-frame accel sensor measurements
    VectorFloat gravity;        // [x, y, z]            gravity vector
    float ypr[3];               // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector


    while( m_fifoCount >= m_packetSize )
    {
        m_device.getFIFOBytes( m_fifoBuffer, m_packetSize );
        m_fifoCount -= m_packetSize;
    
        //m_device.dmpGetAccel( &aa, m_fifoBuffer );

        m_device.dmpGetGyro( &gyro, m_fifoBuffer );
        m_device.dmpGetQuaternion( &q, m_fifoBuffer );
        m_device.dmpGetAccel( &aa, m_fifoBuffer );
        m_device.dmpGetGravity( &gravity, &q );
        m_device.dmpGetYawPitchRoll( ypr, &q, &gravity );
        m_device.dmpGetLinearAccel( &aaReal, &aa, &gravity );
        m_device.dmpGetLinearAccelInWorld( &aaWorld, &aaReal, &q );

        // Добавляем значение в медианные фильтры
        m_filterGyroX.Add( gyro.x );
        m_filterGyroY.Add( gyro.y );
        m_filterGyroZ.Add( gyro.z );

        //m_filterAcclX.Add( aaWorld.x );
        //m_filterAcclY.Add( aaWorld.y );
        //m_filterAcclZ.Add( aaWorld.z );

        m_filterAcclX.Add( aaReal.x );
        m_filterAcclY.Add( aaReal.y );
        m_filterAcclZ.Add( aaReal.z );

        //m_filterAcclX.Add( aa.x );
        //m_filterAcclY.Add( aa.y );
        //m_filterAcclZ.Add( aa.z );
    }

#ifdef USE_LONG_MEDIAN_FILTER
    m_filterGyroX.Sort();
    m_filterGyroY.Sort();
    m_filterGyroZ.Sort();

    m_filterAcclX.Sort();
    m_filterAcclY.Sort();
    m_filterAcclZ.Sort();
#endif

    m_rawGyroX = m_filterGyroX.Get();
    m_rawGyroY = m_filterGyroY.Get();
    m_rawGyroZ = m_filterGyroZ.Get();

    m_rawAcclX = m_filterAcclX.Get();
    m_rawAcclY = m_filterAcclY.Get();
    m_rawAcclZ = m_filterAcclZ.Get();

    // Last saved Yaw, Pitch and Roll
    m_angleYaw = ypr[0];
    m_anglePitch = ypr[1];
    m_angleRoll = ypr[2];

    m_acceleration.x = m_rawAcclX / 8192.0f;
    m_acceleration.y = m_rawAcclY / 8192.0f;
    m_acceleration.z = m_rawAcclZ / 8192.0f;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
// Class for inertial navigation.                                                                 //
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef NAVIGATION_H
#define NAVIGATION_H

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include <Wire.h>
#endif

#define USE_LONG_MEDIAN_FILTER


#ifdef USE_LONG_MEDIAN_FILTER
#include "MedianFilter.h"
#else
#include "MedianFilter3.h"
#endif

#include <helper_3dmath.h>



////////////////////////////////////////////////////////////////////////////////////////////////////
class CNavigation
{
public:
    CNavigation();

    void Init();
    void Tick( const uint64_t microSec );
    void ResetSensorFreqCounter();
    void CalculateReadableData();

    // Получение данных
    VectorFloat GetAcceleration() const { return m_accelerationFlt; }

    // Сырые данные с акселерометров
    int16_t     GetRawAcclX() const { return m_rawAcclX; }
    int16_t     GetRawAcclY() const { return m_rawAcclY; }
    int16_t     GetRawAcclZ() const { return m_rawAcclZ; }

    int16_t     GetRawGyroX() const { return m_rawGyroX; }
    int16_t     GetRawGyroY() const { return m_rawGyroY; }
    int16_t     GetRawGyroZ() const { return m_rawGyroZ; }

    int64_t     GetRawSpeedX() const { return m_rawSpeedX; }
    int64_t     GetRawSpeedY() const { return m_rawSpeedY; }
    int64_t     GetRawSpeedZ() const { return m_rawSpeedZ; }

    int64_t     GetRawPosX() const { return m_rawPosX; }
    int64_t     GetRawPosY() const { return m_rawPosY; }
    int64_t     GetRawPosZ() const { return m_rawPosZ; }



    // Get acceleration data
    float       GetAcclX() const { return m_acclX; }
    float       GetAcclY() const { return m_acclY; }
    float       GetAcclZ() const { return m_acclZ; }
    float       GetAcclS() const { return m_acclS; }

    // Get speed data
    float       GetSpeedX() const { return m_speedX; }
    float       GetSpeedY() const { return m_speedY; }
    float       GetSpeedZ() const { return m_speedZ; }
    float       GetSpeedS() const { return m_speedS; }
    float       GetSpeedK() const { return m_speedK; }

    // Get position data
    float       GetPosX() const { return m_posX; }
    float       GetPosY() const { return m_posY; }
    float       GetPosZ() const { return m_posZ; }
    float       GetDist() const { return m_dist; }

    // Получение данных об углах поворота
    float       GetAngleYaw() const { return m_angleYaw; }
    float       GetAnglePitch() const { return m_anglePitch; }
    float       GetAngleRoll() const { return m_angleRoll; }

    // Get gyroscope data
    //int16_t GetGyroX() const { return m_gyroX; }
    //int16_t GetGyroY() const { return m_gyroY; }
    //int16_t GetGyroZ() const { return m_gyroZ; }

    // System information
    int32_t GetErrorCount() const { return m_errorCount; }
    int32_t GetSensorFreq() const { return m_sensorFreq; }

private:
    // Функции обновления
    void    CalculateParams();
    void    HandleSensor();

    // Физическая секция
    void    CorrectValues();
    float   GetMagnitude( const float x, const float y, const float z );

    // Секция работы с датчиком
    void    HandleData();
    void    ReadDataFromSensor();

#ifdef USE_LONG_MEDIAN_FILTER
    static const int MEDIAN_FILTER_SIZE = 3;

    CMedianFilter< int16_t, MEDIAN_FILTER_SIZE >    m_filterGyroX;
    CMedianFilter< int16_t, MEDIAN_FILTER_SIZE >    m_filterGyroY;
    CMedianFilter< int16_t, MEDIAN_FILTER_SIZE >    m_filterGyroZ;
    
    CMedianFilter< int16_t, MEDIAN_FILTER_SIZE >    m_filterAcclX;
    CMedianFilter< int16_t, MEDIAN_FILTER_SIZE >    m_filterAcclY;
    CMedianFilter< int16_t, MEDIAN_FILTER_SIZE >    m_filterAcclZ;
#else
    CMedianFilter3< int16_t >   m_filterAcclX;
    CMedianFilter3< int16_t >   m_filterAcclY;
    CMedianFilter3< int16_t >   m_filterAcclZ;

    CMedianFilter3< int16_t >   m_filterGyroX;
    CMedianFilter3< int16_t >   m_filterGyroY;
    CMedianFilter3< int16_t >   m_filterGyroZ;
#endif

    // Время
    uint64_t    m_realTime;
    uint64_t    m_stepTime;

    // Показания акселерометра
    int16_t     m_rawAcclX;
    int16_t     m_rawAcclY;
    int16_t     m_rawAcclZ;

    // Показания гироскопа
    int16_t     m_rawGyroX;
    int16_t     m_rawGyroY;
    int16_t     m_rawGyroZ;

    // Непреведённые показания скорости
    int64_t     m_rawSpeedX;
    int64_t     m_rawSpeedY;
    int64_t     m_rawSpeedZ;

    // Непреведённые показания позиции
    int64_t     m_rawPosX;
    int64_t     m_rawPosY;
    int64_t     m_rawPosZ;

    // Углы поворота
    float       m_angleYaw;
    float       m_anglePitch;
    float       m_angleRoll;

    // Ускорение
    VectorInt16 m_accelerationRaw;
    VectorFloat m_accelerationFlt;


    // Ускорение по осям и модуль
    float       m_acclX;
    float       m_acclY;
    float       m_acclZ;
    float       m_acclS;

    // Скорость по осям, модуль и модуль скорости в [км/ч]
    float       m_speedX;
    float       m_speedY;
    float       m_speedZ;
    float       m_speedS;
    float       m_speedK;

    // Позиции по осям
    float       m_posX;
    float       m_posY;
    float       m_posZ;

    // Пройденное расстояние
    float       m_dist;

    // Кол-во ошибок переполнения
    int32_t     m_errorCount;
    int32_t     m_sensorFreq;

    // Данные для чтения датчиков
    uint16_t    m_packetSize; 
    uint16_t    m_fifoCount;    // count of all bytes currently in FIFO
    uint8_t     m_fifoBuffer[64]; // FIFO storage buffer

    // Флаги
    bool        m_bWasInit;
};
////////////////////////////////////////////////////////////////////////////////////////////////////
#endif


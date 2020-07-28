////////////////////////////////////////////////////////////////////////////////////////////////////
// Class for inertial platform.																	  //
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ARIADNE_PLATFORM_H
#define ARIADNE_PLATFORM_H

#include "InertialUnit.h"

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include <Wire.h>
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////
class CPlatform
{
public:
	CPlatform();

	void Init();
	void Tick( const uint64_t microSec );
	void ResetSensorFreqCounter();

	// Получение данных
    int32_t GetErrorCount() const { return m_errorCount; }
    int32_t GetSensorFreq() const { return m_sensorFreq; }
    float 	GetYaw() const { return m_ypr[0]; }
    float 	GetPitch() const { return m_ypr[1]; }
    float 	GetRoll() const { return m_ypr[2]; }
	bool WasInit() const { return m_bWasInit; }

	// CRAP
	int16_t	GetRawAcclX() const { return m_device.GetRawAcclX(); }
	int16_t	GetRawAcclY() const { return m_device.GetRawAcclY(); }
	int16_t	GetRawAcclZ() const { return m_device.GetRawAcclZ(); }

	int16_t	GetRawGyroX() const { return m_device.GetRawGyroX(); }
	int16_t	GetRawGyroY() const { return m_device.GetRawGyroY(); }
	int16_t	GetRawGyroZ() const { return m_device.GetRawGyroZ(); }


	float	GetAcclX() const { return m_device.GetAcclX(); }
	float	GetAcclY() const { return m_device.GetAcclY(); }
	float	GetAcclZ() const { return m_device.GetAcclZ(); }
	// end of CRAP


private:

	// Секция работы с датчиком
    void    HandleData();
    void    ReadDataFromSensor();

    CInertialUnit	m_device;

    uint8_t     m_fifoBuffer[64]; // FIFO storage buffer

	int32_t     m_sensorFreq;	// Частота опроса датчиков
	int32_t     m_errorCount;	// Число ошибок
    uint16_t    m_packetSize;	// Данные для чтения датчиков
    uint16_t    m_fifoCount;    // count of all bytes currently in FIFO

    // Параметры ориентации
    float 		m_ypr[3];	// [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector


    // Флаги
    bool        m_bWasInit;
	
};
////////////////////////////////////////////////////////////////////////////////////////////////////
#endif
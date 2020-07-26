////////////////////////////////////////////////////////////////////////////////////////////////////
// Class for inertial platform.																	  //
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ARIADNE_PLATFORM_H
#define ARIADNE_PLATFORM_H

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


private:

	// Секция работы с датчиком
    void    HandleData();
    void    ReadDataFromSensor();

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
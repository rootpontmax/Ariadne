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
    
	bool WasInit() const { return m_bWasInit; }

private:

	// Секция работы с датчиком
    void    HandleData();
    void    ReadDataFromSensor();

	int32_t     m_sensorFreq;	// Частота опроса датчиков
	int32_t     m_errorCount;	// Число ошибок
    uint16_t    m_packetSize;	// Данные для чтения датчиков
    uint16_t    m_fifoCount;    // count of all bytes currently in FIFO

    // Флаги
    bool        m_bWasInit;
	
};
////////////////////////////////////////////////////////////////////////////////////////////////////
#endif
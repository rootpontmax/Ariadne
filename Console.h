#ifndef ARIADNE_CONSOLE_H
#define ARIADNE_CONSOLE_H

#include <stdint.h>

//#define DEBUG

////////////////////////////////////////////////////////////////////////////////////////////////////
class CDisplay;
////////////////////////////////////////////////////////////////////////////////////////////////////
enum EConsoleType
{
	// Raw acceleration
	CONSOLE_TYPE_RAW_ACCL_X,
	CONSOLE_TYPE_RAW_ACCL_Y,
	CONSOLE_TYPE_RAW_ACCL_Z,

	// Raw gyroscope
	CONSOLE_TYPE_RAW_GYRO_X,
	CONSOLE_TYPE_RAW_GYRO_Y,
	CONSOLE_TYPE_RAW_GYRO_Z,

	// Acceleration
	CONSOLE_TYPE_ACCL_X,
	CONSOLE_TYPE_ACCL_Y,
	CONSOLE_TYPE_ACCL_Z,
	CONSOLE_TYPE_ACCL_MODULE,

	// Gyroscope readings
	CONSOLE_TYPE_GYRO_X,
	CONSOLE_TYPE_GYRO_Y,
	CONSOLE_TYPE_GYRO_Z,

	// Angles
	CONSOLE_TYPE_ANGLE_YAW,
	CONSOLE_TYPE_ANGLE_PITCH,
	CONSOLE_TYPE_ANGLE_ROLL,

	// System section
	CONSOLE_TYPE_SYSTEM_FREQ,
	CONSOLE_TYPE_UPTIME_SEC,
	CONSOLE_TYPE_COUNT
};
////////////////////////////////////////////////////////////////////////////////////////////////////
enum EDataType
{
	DATA_TYPE_NONE,
	DATA_TYPE_TIME,
	DATA_TYPE_FLT,
	DATA_TYPE_INT,
	DATA_TYPE_COUNT
};
////////////////////////////////////////////////////////////////////////////////////////////////////
class CConsole
{
public:

	CConsole( CDisplay *pDisplay );

	void	Init();
	void	RegisterText( const char *pString );
	void	RegisterData( const EConsoleType type, const EDataType dataType, const char *pString );
	void	RegisterSpace();
	void	SetWorkMode();

	void	SetData( const EConsoleType type, const float data );

	void	DrawHeader();
	void	DrawInfo();
	void 	DrawData();
private:

	static const uint16_t TEXT_POS_X = 10;
	static const uint16_t DATA_POS_X = 160;
	static const uint16_t TEXT_STEP_X = 5;
	static const uint16_t TEXT_STEP_Y = 10;
	static const uint16_t CLEAR_SIZE_X = 70;
	static const uint16_t CLEAR_SIZE_Y = 7;


	struct SItem
	{
		SItem();

		float		fltData;
		int32_t		intData;

		uint16_t	posY;
		const char *pString;
		EDataType	type;
	};

	void	DrawData( const uint16_t y, const int16_t value );
	void 	DrawData( const uint16_t y, const int32_t value );
	void 	DrawData( const uint16_t y, const int64_t value );
	void 	DrawData( const uint16_t y, const float value );
	void 	DrawTime( const uint16_t y, const int32_t timeSec );

	CDisplay   * const m_pDisplay;

	SItem		m_item[CONSOLE_TYPE_COUNT];
	uint16_t 	m_posX;
	uint16_t 	m_posY;
	char		m_buffer[16];
};
////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // ARIADNE_CONSOLE_H
#ifndef ARIADNE_CONSOLE_H
#define ARIADNE_CONSOLE_H

#include <stdint.h>

//#define DEBUG

////////////////////////////////////////////////////////////////////////////////////////////////////
class CDisplay;
////////////////////////////////////////////////////////////////////////////////////////////////////
enum EConsoleType
{
	// Raw acceleration system
	CONSOLE_TYPE_RAW_ACCEL_X,
	CONSOLE_TYPE_RAW_ACCEL_Y,
	CONSOLE_TYPE_RAW_ACCEL_Z,


	// System section
	CONSOLE_TYPE_FIFO_OVERFLOW_COUNT,
	CONSOLE_TYPE_SYSTEM_FREQ,
	CONSOLE_TYPE_SENSOR_FREQ,

	CONSOLE_TYPE_COUNT
};
////////////////////////////////////////////////////////////////////////////////////////////////////
class CConsole
{
public:

	CConsole( CDisplay *pDisplay );

	void	Init();
	void	RegisterText( const char *pString );
	void	RegisterData( const EConsoleType type, const char *pString );
	void	RegisterSpace();
	void	SetWorkMode();

	void	DrawHeader();
	void	DrawInfo();
	void 	DrawData();
private:

	static const uint16_t TEXT_POS_X = 10;
	static const uint16_t DATA_POS_X = 160;
	static const uint16_t TEXT_STEP_Y = 10;
	static const uint16_t CLEAR_SIZE_X = 70;
	static const uint16_t CLEAR_SIZE_Y = 7;

	struct SItem
	{
		SItem();

		float		fltData;

		uint16_t	posY;
		const char *pString;
		bool		bWasInit;
	};

	void	DrawData( const uint16_t y, const int16_t value );
	void 	DrawData( const uint16_t y, const int32_t value );
	void 	DrawData( const uint16_t y, const int64_t value );
	void 	DrawData( const uint16_t y, const float value );

	CDisplay   * const m_pDisplay;

	SItem		m_item[CONSOLE_TYPE_COUNT];
	uint16_t 	m_posX;
	uint16_t 	m_posY;
};
////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // ARIADNE_CONSOLE_H
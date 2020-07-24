#include "Console.h"

#include <Timer.h>
#include <Display.h>
#include "Navigation.h"

//#define DEBUG
#define SHOW_RAW_DATA

/*
////////////////////////////////////////////////////////////////////////////////////////////////////
static const uint16_t TEXT_POS_X = 10;
static const uint16_t DATA_POS_X = 160;
static const uint16_t TEXT_STEP_Y = 10;
static const uint16_t CLEAR_SIZE_X = 70;
static const uint16_t CLEAR_SIZE_Y = 7;
static const uint16_t INFO_COLOR = CDisplay::COLOR_WHITE;
static const uint16_t DATA_COLOR = CDisplay::COLOR_GREEN;
#ifdef DEBUG
static const uint16_t BACK_COLOR = CDisplay::COLOR_RED;
static const uint16_t FILL_COLOR = CDisplay::COLOR_MAGENTA;
#else
static const uint16_t BACK_COLOR = CDisplay::COLOR_BLACK;
static const uint16_t FILL_COLOR = CDisplay::COLOR_BLACK;
#endif
*/
////////////////////////////////////////////////////////////////////////////////////////////////////
CNavigation		g_navigation;
CTimer			g_fpsTimer( true );
CTimer			g_screenTimer( true );
CDisplay		g_display;
CConsole		g_console( &g_display );
int32_t	 		g_cyclesCount = 0;
uint64_t		g_timeMicroSeconds;
////////////////////////////////////////////////////////////////////////////////////////////////////
uint16_t		g_posAclX;
uint16_t		g_posAclY;
uint16_t		g_posAclZ;
uint16_t		g_posAclS;

uint16_t		g_posSpdX;
uint16_t		g_posSpdY;
uint16_t		g_posSpdZ;
uint16_t		g_posSpdS;
uint16_t		g_posSpdK;

uint16_t		g_posPosX;
uint16_t		g_posPosY;
uint16_t		g_posPosZ;
uint16_t		g_posDist;

uint16_t		g_posSystemFreq;
uint16_t		g_posSensorFreq;
uint16_t		g_posErrorCount;
/*
////////////////////////////////////////////////////////////////////////////////////////////////////
static uint16_t PutTextAndShiftPos( uint16_t& x, uint16_t& y, const char *pStr )
{
	const uint16_t retY = y;
	g_display.DrawText( x, y, pStr );
	y += TEXT_STEP_Y;
	return retY;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
static void DrawData( const uint16_t y, const int16_t value )
{
	g_display.FillRect( DATA_POS_X, y, CLEAR_SIZE_X, CLEAR_SIZE_Y, BACK_COLOR );
	g_display.DrawInt( DATA_POS_X, y, value );
}
////////////////////////////////////////////////////////////////////////////////////////////////////
static void DrawData( const uint16_t y, const int32_t value )
{
	g_display.FillRect( DATA_POS_X, y, CLEAR_SIZE_X, CLEAR_SIZE_Y, BACK_COLOR );
	g_display.DrawInt( DATA_POS_X, y, value );
}
////////////////////////////////////////////////////////////////////////////////////////////////////
static void DrawData( const uint16_t y, const int64_t value )
{
	g_display.FillRect( DATA_POS_X, y, CLEAR_SIZE_X, CLEAR_SIZE_Y, BACK_COLOR );
	g_display.DrawLong( DATA_POS_X, y, value );
}
////////////////////////////////////////////////////////////////////////////////////////////////////
static void DrawData( const uint16_t y, const float value )
{
	g_display.FillRect( DATA_POS_X, y, CLEAR_SIZE_X, CLEAR_SIZE_Y, BACK_COLOR );
	g_display.DrawFlt( DATA_POS_X, y, value );
}
////////////////////////////////////////////////////////////////////////////////////////////////////
static void ShowData()
{
#ifdef SHOW_RAW_DATA
	DrawData( g_posAclX, g_navigation.GetRawAcclX() );
	DrawData( g_posAclY, g_navigation.GetRawAcclY() );
	DrawData( g_posAclZ, g_navigation.GetRawAcclZ() );

	DrawData( g_posSpdX, g_navigation.GetRawGyroX() );
	DrawData( g_posSpdY, g_navigation.GetRawGyroY() );
	DrawData( g_posSpdZ, g_navigation.GetRawGyroZ() );

	DrawData( g_posPosX, g_navigation.GetAngleYaw() );
	DrawData( g_posPosY, g_navigation.GetAnglePitch() );
	DrawData( g_posPosZ, g_navigation.GetAngleRoll() );
#else
	g_navigation.CalculateReadableData();
	DrawData( g_posAclX, g_navigation.GetAcclX() );
	DrawData( g_posAclY, g_navigation.GetAcclY() );
	DrawData( g_posAclZ, g_navigation.GetAcclZ() );
	DrawData( g_posAclS, g_navigation.GetAcclS() );

	DrawData( g_posSpdX, g_navigation.GetSpeedX() );
	DrawData( g_posSpdY, g_navigation.GetSpeedY() );
	DrawData( g_posSpdZ, g_navigation.GetSpeedZ() );
	DrawData( g_posSpdS, g_navigation.GetSpeedS() );
	DrawData( g_posSpdK, g_navigation.GetSpeedK() );

	DrawData( g_posPosX, g_navigation.GetPosX() );
	DrawData( g_posPosY, g_navigation.GetPosY() );
	DrawData( g_posPosZ, g_navigation.GetPosZ() );
	DrawData( g_posDist, g_navigation.GetDist() );
#endif

	DrawData( g_posErrorCount, g_navigation.GetErrorCount() );
	DrawData( g_posSystemFreq, g_cyclesCount );
	DrawData( g_posSensorFreq, g_navigation.GetSensorFreq() );
}
*/
////////////////////////////////////////////////////////////////////////////////////////////////////
static void InitConsole()
{
	g_console.DrawHeader();

#ifdef SHOW_RAW_DATA
	g_console.RegisterData( CONSOLE_TYPE_RAW_ACCEL_X, "Raw acceleration X" );
	g_console.RegisterData( CONSOLE_TYPE_RAW_ACCEL_Y, "Raw acceleration Y" );
	g_console.RegisterData( CONSOLE_TYPE_RAW_ACCEL_Z, "Raw acceleration Z" );
	g_console.RegisterSpace();
#else
#endif

	// System section
	g_console.RegisterData( CONSOLE_TYPE_FIFO_OVERFLOW_COUNT, "Errors count" );
	g_console.RegisterData( CONSOLE_TYPE_SYSTEM_FREQ, "System freq (Hz)" );
	g_console.RegisterData( CONSOLE_TYPE_SENSOR_FREQ, "Sensor freq (Hz)" );
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void setup()
{
	//Serial.begin( 9600 );
	Serial.begin( 115200 );
	
	//fdevopen( &serial_putc, 0 );

	//g_application.Init();

	



	//Serial.begin( 9600 );
	g_console.Init();
	
	

	// Init
	g_navigation.Init();

	InitConsole();


	// Draw information
	//uint16_t posX = TEXT_POS_X;
	//uint16_t posY = TEXT_POS_X;

	
/*
#ifdef SHOW_RAW_DATA
	g_posAclX = PutTextAndShiftPos( posX, posY, "Raw acceleration X" );
	g_posAclY = PutTextAndShiftPos( posX, posY, "Raw acceleration Y" );
	g_posAclZ = PutTextAndShiftPos( posX, posY, "Raw acceleration Z" );
	MakeSpace( posY );

	g_posSpdX = PutTextAndShiftPos( posX, posY, "Raw gyro X" );
	g_posSpdY = PutTextAndShiftPos( posX, posY, "Raw gyro Y" );
	g_posSpdZ = PutTextAndShiftPos( posX, posY, "Raw gyro Z" );
	MakeSpace( posY );

	g_posPosX = PutTextAndShiftPos( posX, posY, "Angle yaw" );
	g_posPosY = PutTextAndShiftPos( posX, posY, "Angle pitch" );
	g_posPosZ = PutTextAndShiftPos( posX, posY, "Angle roll" );
	MakeSpace( posY );
#else
	// Acceleration section
	g_posAclX = PutTextAndShiftPos( posX, posY, "Acceleration X (m/s2)" );
	g_posAclY = PutTextAndShiftPos( posX, posY, "Acceleration Y (m/s2)" );
	g_posAclZ = PutTextAndShiftPos( posX, posY, "Acceleration Z (m/s2)" );
	g_posAclS = PutTextAndShiftPos( posX, posY, "Acceleration   (m/s2)" );
	MakeSpace( posY );

	// Speed section
	g_posSpdX = PutTextAndShiftPos( posX, posY, "Speed X (m/s)" );
	g_posSpdY = PutTextAndShiftPos( posX, posY, "Speed Y (m/s)" );
	g_posSpdZ = PutTextAndShiftPos( posX, posY, "Speed Z (m/s)" );
	g_posSpdS = PutTextAndShiftPos( posX, posY, "Speed   (m/s)" );
	g_posSpdK = PutTextAndShiftPos( posX, posY, "Speed   (km/h)" );
	MakeSpace( posY );

	// Position section
	g_posPosX = PutTextAndShiftPos( posX, posY, "Position X (m)" );
	g_posPosY = PutTextAndShiftPos( posX, posY, "Position Y (m)" );
	g_posPosZ = PutTextAndShiftPos( posX, posY, "Position Z (m)" );
	g_posDist = PutTextAndShiftPos( posX, posY, "Distance   (m)" );
	MakeSpace( posY );
#endif

	// System section
	g_posErrorCount = PutTextAndShiftPos( posX, posY, "Errors count" );
	g_posSystemFreq = PutTextAndShiftPos( posX, posY, "System freq (Hz)" );
	g_posSensorFreq = PutTextAndShiftPos( posX, posY, "Sensor freq (Hz)" );
	*/


	/*
	// Copyright section
	g_display.DrawText( posX, posY, "Ariadne test project" ); posY += TEXT_STEP_Y;
	posY += TEXT_STEP_Y;

	g_display.SetTextColor( CDisplay::COLOR_WHITE );

	// Acceleration section
	g_display.DrawText( posX, posY, "Acceleration X" ); posY += TEXT_STEP_Y;
	g_display.DrawText( posX, posY, "Acceleration Y" ); posY += TEXT_STEP_Y;
	g_display.DrawText( posX, posY, "Acceleration Z" ); posY += TEXT_STEP_Y;
	g_display.DrawText( posX, posY, "Acceleration" ); posY += TEXT_STEP_Y;
	posY += TEXT_STEP_Y;

	// Speed section
	g_display.DrawText( posX, posY, "Speed X" ); posY += TEXT_STEP_Y;
	g_display.DrawText( posX, posY, "Speed Y" ); posY += TEXT_STEP_Y;
	g_display.DrawText( posX, posY, "Speed Z" ); posY += TEXT_STEP_Y;
	g_display.DrawText( posX, posY, "Speed (m/s)" ); posY += TEXT_STEP_Y;
	g_display.DrawText( posX, posY, "Speed (km/h)" ); posY += TEXT_STEP_Y;
	posY += TEXT_STEP_Y;
	*/

	// Set different back and text color for data information
	g_console.DrawInfo();
	g_console.SetWorkMode();
	
	//g_display.SetBackColor( BACK_COLOR );

	g_screenTimer.Start( 2000, true );
	g_fpsTimer.Start( 1000, true );

	g_timeMicroSeconds = micros();
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void loop()
{
	// Calculate delta time
	const uint64_t thisTimeMicrosSeconds = micros();
	const uint64_t deltaTimeMicroSeconds = thisTimeMicrosSeconds - g_timeMicroSeconds;
	g_timeMicroSeconds = thisTimeMicrosSeconds;

	// Tick the navigation system
	g_navigation.Tick( deltaTimeMicroSeconds );

	// Show information once a second
	if( g_fpsTimer.IsReady() )
	{
		g_console.DrawData();
		g_cyclesCount = 0;
		g_navigation.ResetSensorFreqCounter();
	}
	++g_cyclesCount;
}
////////////////////////////////////////////////////////////////////////////////////////////////////

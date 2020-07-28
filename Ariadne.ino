#include "Console.h"

#include <Timer.h>
#include <Display.h>
#include "Platform.h"

//#define DEBUG
#define SHOW_RAW_DATA

////////////////////////////////////////////////////////////////////////////////////////////////////
//CNavigation		g_navigation;
CTimer			g_fpsTimer( true );
CTimer			g_screenTimer( true );
CDisplay		g_display;
CConsole		g_console( &g_display );
CPlatform		g_platform;
int32_t	 		g_cyclesCount = 0;
uint64_t		g_timeMicroSeconds;
uint64_t		g_uptimeMicroSeconds = 0;
uint64_t		g_uptimeCyclesCounts = 0;
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
////////////////////////////////////////////////////////////////////////////////////////////////////
static void InitConsole()
{
	g_console.DrawHeader();

#ifdef SHOW_RAW_DATA
	g_console.RegisterData( CONSOLE_TYPE_RAW_ACCEL_X, DATA_TYPE_INT, "Raw acceleration X" );
	g_console.RegisterData( CONSOLE_TYPE_RAW_ACCEL_Y, DATA_TYPE_INT, "Raw acceleration Y" );
	g_console.RegisterData( CONSOLE_TYPE_RAW_ACCEL_Z, DATA_TYPE_INT, "Raw acceleration Z" );
	g_console.RegisterSpace();
#else

#endif

	/*
	// Angles
	g_console.RegisterData( CONSOLE_TYPE_ANGLE_YAW, DATA_TYPE_FLT, "Yaw" );
	g_console.RegisterData( CONSOLE_TYPE_ANGLE_PITCH, DATA_TYPE_FLT, "Pitch" );
	g_console.RegisterData( CONSOLE_TYPE_ANGLE_ROLL, DATA_TYPE_FLT, "Roll" );
	g_console.RegisterSpace();
	*/

	// System section
	g_console.RegisterData( CONSOLE_TYPE_FIFO_OVERFLOW_COUNT, DATA_TYPE_INT, "Errors count" );
	g_console.RegisterData( CONSOLE_TYPE_SYSTEM_FREQ, DATA_TYPE_INT, "System freq (Hz)" );
	g_console.RegisterData( CONSOLE_TYPE_SENSOR_FREQ, DATA_TYPE_INT, "Sensor freq (Hz)" );
	g_console.RegisterData( CONSOLE_TYPE_UPTIME_SEC, DATA_TYPE_TIME, "Uptime (HH:MM:SS)" );
}
////////////////////////////////////////////////////////////////////////////////////////////////////
static void SetDataToConsole()
{
	// Raw data
#ifdef SHOW_RAW_DATA
	const int16_t acclX = g_platform.GetAcclX();
	const int16_t acclY = g_platform.GetAcclY();
	const int16_t acclZ = g_platform.GetAcclZ();
	g_console.SetData( CONSOLE_TYPE_RAW_ACCEL_X, acclX );
	g_console.SetData( CONSOLE_TYPE_RAW_ACCEL_Y, acclY );
	g_console.SetData( CONSOLE_TYPE_RAW_ACCEL_Z, acclZ );
#endif

	/*
	// Angle section
	const float angleYaw = g_platform.GetYaw();
    const float anglePitch = g_platform.GetPitch();
    const float angleRoll = g_platform.GetRoll();
    g_console.SetData( CONSOLE_TYPE_ANGLE_YAW, angleYaw );
    g_console.SetData( CONSOLE_TYPE_ANGLE_PITCH, anglePitch );
    g_console.SetData( CONSOLE_TYPE_ANGLE_ROLL, angleRoll );
    */

    // System section
	const int32_t errorCount = g_platform.GetErrorCount();
    const int32_t sensorFreq = g_platform.GetSensorFreq();
    const uint32_t uptimeSec = g_uptimeMicroSeconds / 1000000;// + 86390; // for test

    g_console.SetData( CONSOLE_TYPE_FIFO_OVERFLOW_COUNT, errorCount );
    g_console.SetData( CONSOLE_TYPE_SYSTEM_FREQ, g_cyclesCount );
    g_console.SetData( CONSOLE_TYPE_SENSOR_FREQ, sensorFreq );
    g_console.SetData( CONSOLE_TYPE_UPTIME_SEC, uptimeSec );
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void setup()
{
	Serial.begin( 115200 );

	g_console.Init();
	g_platform.Init();

	if( !g_platform.WasInit() )
		return;
	
	InitConsole();
	g_console.DrawInfo();
	g_console.SetWorkMode();
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
	g_uptimeMicroSeconds += deltaTimeMicroSeconds;

	// Tick the navigation system
	//g_platform.Tick( deltaTimeMicroSeconds );

	// Show information once a second
	if( g_fpsTimer.IsReady() )
	{
		SetDataToConsole();
		g_console.DrawData();
		g_cyclesCount = 0;
		g_platform.ResetSensorFreqCounter();
	}
	++g_cyclesCount;
	++g_uptimeCyclesCounts;
}
////////////////////////////////////////////////////////////////////////////////////////////////////

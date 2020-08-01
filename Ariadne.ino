#include "Console.h"

#include <Timer.h>
#include <Display.h>
#include "InertialUnit.h"
#include "Platform.h"

//#define DEBUG
#define SHOW_RAW_DATA

////////////////////////////////////////////////////////////////////////////////////////////////////
//CNavigation		g_navigation;
CTimer			g_fpsTimer( true );
CTimer			g_screenTimer( true );
CDisplay		g_display;
CConsole		g_console( &g_display );
CInertialUnit	g_inertialUnit;
CPlatform		g_platform( &g_inertialUnit );
int32_t	 		g_cyclesCount = 0;
uint64_t		g_timeMicroSeconds;
uint64_t		g_uptimeMicroSeconds = 0;
uint64_t		g_uptimeCyclesCounts = 0;
////////////////////////////////////////////////////////////////////////////////////////////////////
static void InitConsole()
{
	g_console.DrawHeader();

#ifdef SHOW_RAW_DATA
	g_console.RegisterData( CONSOLE_TYPE_RAW_ACCL_X, DATA_TYPE_INT, "Raw acceleration X" );
	g_console.RegisterData( CONSOLE_TYPE_RAW_ACCL_Y, DATA_TYPE_INT, "Raw acceleration Y" );
	g_console.RegisterData( CONSOLE_TYPE_RAW_ACCL_Z, DATA_TYPE_INT, "Raw acceleration Z" );
	g_console.RegisterSpace();

	g_console.RegisterData( CONSOLE_TYPE_RAW_GYRO_X, DATA_TYPE_INT, "Raw gyroscope X" );
	g_console.RegisterData( CONSOLE_TYPE_RAW_GYRO_Y, DATA_TYPE_INT, "Raw gyroscope Y" );
	g_console.RegisterData( CONSOLE_TYPE_RAW_GYRO_Z, DATA_TYPE_INT, "Raw gyroscope Z" );
	g_console.RegisterSpace();
#else

#endif

	g_console.RegisterData( CONSOLE_TYPE_ACCL_X, DATA_TYPE_FLT, "Acceleration X (m/s2)" );
	g_console.RegisterData( CONSOLE_TYPE_ACCL_Y, DATA_TYPE_FLT, "Acceleration Y (m/s2)" );
	g_console.RegisterData( CONSOLE_TYPE_ACCL_Z, DATA_TYPE_FLT, "Acceleration Z (m/s2)" );
	g_console.RegisterData( CONSOLE_TYPE_ACCL_MODULE, DATA_TYPE_FLT, "Acceleration (m/s2)" );
	g_console.RegisterSpace();

	g_console.RegisterData( CONSOLE_TYPE_GYRO_X, DATA_TYPE_FLT, "Gyroscope X (deg/s)" );
	g_console.RegisterData( CONSOLE_TYPE_GYRO_Y, DATA_TYPE_FLT, "Gyroscope Y (deg/s)" );
	g_console.RegisterData( CONSOLE_TYPE_GYRO_Z, DATA_TYPE_FLT, "Gyroscope Z (deg/s)" );
	g_console.RegisterSpace();

	g_console.RegisterData( CONSOLE_TYPE_ANGLE_SPEED_X, DATA_TYPE_FLT, "Angle speed X (deg/s)" );
	g_console.RegisterData( CONSOLE_TYPE_ANGLE_SPEED_Y, DATA_TYPE_FLT, "Angle speed Y (deg/s)" );
	g_console.RegisterData( CONSOLE_TYPE_ANGLE_SPEED_Z, DATA_TYPE_FLT, "Angle speed Z (deg/s)" );
	g_console.RegisterSpace();
	
	// Angles
	g_console.RegisterData( CONSOLE_TYPE_ANGLE_X, DATA_TYPE_FLT, "Angle X (deg)" );
	g_console.RegisterData( CONSOLE_TYPE_ANGLE_Y, DATA_TYPE_FLT, "Angle Y (deg)" );
	g_console.RegisterData( CONSOLE_TYPE_ANGLE_Z, DATA_TYPE_FLT, "Angle Z (deg)" );
	g_console.RegisterSpace();

	// System section
	g_console.RegisterData( CONSOLE_TYPE_SYSTEM_FREQ, DATA_TYPE_INT, "System freq (Hz)" );
	g_console.RegisterData( CONSOLE_TYPE_UPTIME_SEC, DATA_TYPE_TIME, "Uptime (HH:MM:SS)" );
}
////////////////////////////////////////////////////////////////////////////////////////////////////
static void SetDataToConsole()
{
	// Raw data
#ifdef SHOW_RAW_DATA
	const int16_t rawAcclX = g_inertialUnit.GetRawAcclX();
	const int16_t rawAcclY = g_inertialUnit.GetRawAcclY();
	const int16_t rawAcclZ = g_inertialUnit.GetRawAcclZ();
	g_console.SetData( CONSOLE_TYPE_RAW_ACCL_X, rawAcclX );
	g_console.SetData( CONSOLE_TYPE_RAW_ACCL_Y, rawAcclY );
	g_console.SetData( CONSOLE_TYPE_RAW_ACCL_Z, rawAcclZ );

	const int16_t rawGyroX = g_inertialUnit.GetRawGyroX();
	const int16_t rawGyroY = g_inertialUnit.GetRawGyroY();
	const int16_t rawGyroZ = g_inertialUnit.GetRawGyroZ();
	g_console.SetData( CONSOLE_TYPE_RAW_GYRO_X, rawGyroX );
	g_console.SetData( CONSOLE_TYPE_RAW_GYRO_Y, rawGyroY );
	g_console.SetData( CONSOLE_TYPE_RAW_GYRO_Z, rawGyroZ );
#endif

	const float acclX = g_inertialUnit.GetAcclX();
	const float acclY = g_inertialUnit.GetAcclY();
	const float acclZ = g_inertialUnit.GetAcclZ();
	const float acclModule = sqrt( acclX * acclX + acclY * acclY + acclZ * acclZ );
	g_console.SetData( CONSOLE_TYPE_ACCL_X, acclX );
	g_console.SetData( CONSOLE_TYPE_ACCL_Y, acclY );
	g_console.SetData( CONSOLE_TYPE_ACCL_Z, acclZ );
	g_console.SetData( CONSOLE_TYPE_ACCL_MODULE, acclModule );

	g_console.SetData( CONSOLE_TYPE_GYRO_X, g_inertialUnit.GetGyroX() );
	g_console.SetData( CONSOLE_TYPE_GYRO_Y, g_inertialUnit.GetGyroY() );
	g_console.SetData( CONSOLE_TYPE_GYRO_Z, g_inertialUnit.GetGyroZ() );

	g_console.SetData( CONSOLE_TYPE_ANGLE_SPEED_X, g_platform.GetAngleSpeedX() );
	g_console.SetData( CONSOLE_TYPE_ANGLE_SPEED_Y, g_platform.GetAngleSpeedY() );
	g_console.SetData( CONSOLE_TYPE_ANGLE_SPEED_Z, g_platform.GetAngleSpeedZ() );

	g_console.SetData( CONSOLE_TYPE_ANGLE_X, g_platform.GetAngleX() );
	g_console.SetData( CONSOLE_TYPE_ANGLE_Y, g_platform.GetAngleY() );
	g_console.SetData( CONSOLE_TYPE_ANGLE_Z, g_platform.GetAngleZ() );



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
    const uint32_t uptimeSec = g_uptimeMicroSeconds / 1000000;// + 86390; // for test
    g_console.SetData( CONSOLE_TYPE_SYSTEM_FREQ, g_cyclesCount );
    g_console.SetData( CONSOLE_TYPE_UPTIME_SEC, uptimeSec );
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void setup()
{
	//Serial.begin( 9600 );
	Serial.begin( 115200 );
	//Serial.println("----------------------------------------");

	g_console.Init();
	g_inertialUnit.Init();
	
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

	const float timeSec = static_cast< float >( deltaTimeMicroSeconds ) / 1000000.0f;

	// Tick the navigation system
	g_platform.Tick( timeSec );

	// Show information once a second
	if( g_fpsTimer.IsReady() )
	{
		SetDataToConsole();
		g_console.DrawData();
		g_cyclesCount = 0;
	}
	++g_cyclesCount;
	++g_uptimeCyclesCounts;

	// CRAP
	//delay(3);
	// end of CRAP
	//Serial.println();
}
////////////////////////////////////////////////////////////////////////////////////////////////////

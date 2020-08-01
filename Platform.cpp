#include "Platform.h"

#include "InertialUnit.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
CPlatform::CPlatform( CInertialUnit *pDevice ) :
	m_pDevice( pDevice ),

	m_angleSpeedX( 0.0f ),
	m_angleSpeedY( 0.0f ),
	m_angleSpeedZ( 0.0f ),

	m_angleX( 0.0f ),
	m_angleY( 0.0f ),
	m_angleZ( 0.0f )
{}
////////////////////////////////////////////////////////////////////////////////////////////////////
void CPlatform::Tick( const float timeSec )
{
	m_pDevice->Tick();

	const float gyroX = m_pDevice->GetGyroX();
	const float gyroY = m_pDevice->GetGyroY();
	const float gyroZ = m_pDevice->GetGyroZ();

	// First average filter
	m_angleSpeedAverageFilterX.Add( gyroX );
	m_angleSpeedAverageFilterY.Add( gyroY );
	m_angleSpeedAverageFilterZ.Add( gyroZ );

	const float avgGyroX = m_angleSpeedAverageFilterX.Calc( 0.0f );
	const float avgGyroY = m_angleSpeedAverageFilterY.Calc( 0.0f );
	const float avgGyroZ = m_angleSpeedAverageFilterZ.Calc( 0.0f );

	m_angleSpeedMedianFilterX.Add( avgGyroX );
	m_angleSpeedMedianFilterY.Add( avgGyroY );
	m_angleSpeedMedianFilterZ.Add( avgGyroZ );

	m_angleSpeedX = m_angleSpeedMedianFilterX.Get();
	m_angleSpeedY = m_angleSpeedMedianFilterY.Get();
	m_angleSpeedZ = m_angleSpeedMedianFilterZ.Get();

	const float angleDeltaX = m_angleSpeedX * timeSec;
	const float angleDeltaY = m_angleSpeedY * timeSec;
	const float angleDeltaZ = m_angleSpeedZ * timeSec;

	m_angleX += angleDeltaX;
	m_angleY += angleDeltaY;
	m_angleZ += angleDeltaZ;

	/*
	Serial.print( gyroX );
	Serial.print(" ");
	Serial.print( avgGyroX );
	Serial.print(" ");
	Serial.print( m_angleSpeedX );
	Serial.print(" ");
	Serial.println();
	//*/
}
////////////////////////////////////////////////////////////////////////////////////////////////////
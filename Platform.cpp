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

	m_angleSpeedFilterX.Add( m_pDevice->GetGyroX() );
	m_angleSpeedFilterY.Add( m_pDevice->GetGyroY() );
	m_angleSpeedFilterZ.Add( m_pDevice->GetGyroZ() );

	m_angleSpeedX = m_angleSpeedFilterX.Get();
	m_angleSpeedY = m_angleSpeedFilterY.Get();
	m_angleSpeedZ = m_angleSpeedFilterZ.Get();

	const float angleDeltaX = m_angleSpeedX * timeSec;
	const float angleDeltaY = m_angleSpeedY * timeSec;
	const float angleDeltaZ = m_angleSpeedZ * timeSec;

	m_angleX += angleDeltaX;
	m_angleY += angleDeltaY;
	m_angleZ += angleDeltaZ;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
// Class for inertial platform.																	  //
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ARIADNE_PLATFORM_H
#define ARIADNE_PLATFORM_H

#include <Arduino.h>
#include <MedianFilter3.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
class CInertialUnit;
////////////////////////////////////////////////////////////////////////////////////////////////////
class CPlatform
{
public:
	CPlatform( CInertialUnit *pDevice );

	void Tick( const float timeSec );

	float	GetAngleSpeedX() const { return m_angleSpeedX; }
	float	GetAngleSpeedY() const { return m_angleSpeedY; }
	float	GetAngleSpeedZ() const { return m_angleSpeedZ; }

	float	GetAngleX() const { return m_angleX; }
	float	GetAngleY() const { return m_angleY; }
	float	GetAngleZ() const { return m_angleZ; }

private:

	CMedianFilter3< float >	m_angleSpeedFilterX;
	CMedianFilter3< float >	m_angleSpeedFilterY;
	CMedianFilter3< float >	m_angleSpeedFilterZ;

	float m_angleSpeedX;
	float m_angleSpeedY;
	float m_angleSpeedZ;

	float m_angleX;
	float m_angleY;
	float m_angleZ;

	CInertialUnit * const m_pDevice;
};
////////////////////////////////////////////////////////////////////////////////////////////////////
#endif
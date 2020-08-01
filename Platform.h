////////////////////////////////////////////////////////////////////////////////////////////////////
// Class for inertial platform.																	  //
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ARIADNE_PLATFORM_H
#define ARIADNE_PLATFORM_H

#include <Arduino.h>
#include <AverageFilter.h>
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

	static const int AVERAGE_FILTER_GYRO = 5;

	
	CAverageFilter< float, AVERAGE_FILTER_GYRO > m_angleSpeedAverageFilterX;
	CAverageFilter< float, AVERAGE_FILTER_GYRO > m_angleSpeedAverageFilterY;
	CAverageFilter< float, AVERAGE_FILTER_GYRO > m_angleSpeedAverageFilterZ;

	CMedianFilter3< float >	m_angleSpeedMedianFilterX;
	CMedianFilter3< float >	m_angleSpeedMedianFilterY;
	CMedianFilter3< float >	m_angleSpeedMedianFilterZ;

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
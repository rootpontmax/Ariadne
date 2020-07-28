////////////////////////////////////////////////////////////////////////////////////////////////////
// Class of Inertial unit.																		  //
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ARIADNE_INERTIAL_UNIT_H
#define ARIADNE_INERTIAL_UNIT_H

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include <Wire.h>
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
class CInertialUnit
{
public:
	CInertialUnit();

	void	Init();
	void	Tick();

	int16_t	GetRawAcclX() const { return m_rawAcclX; }
	int16_t	GetRawAcclY() const { return m_rawAcclY; }
	int16_t	GetRawAcclZ() const { return m_rawAcclZ; }

	float	GetAcclX() const { return m_acclX; }
	float 	GetAcclY() const { return m_acclY; }
	float	GetAcclZ() const { return m_acclZ; }

	int16_t	GetRawGyroX() const { return m_gyroX; }
	int16_t	GetRawGyroY() const { return m_gyroY; }
	int16_t	GetRawGyroZ() const { return m_gyroZ; }

private:

	// Aux init methods
	void 	Setup();
	void	Calibrate();

	// Working functions
	void	ReadData();

	// Acceleration: raw and normalized
	int16_t	m_rawAcclX;
	int16_t	m_rawAcclY;
	int16_t	m_rawAcclZ;

	float	m_acclX;
	float	m_acclY;
	float	m_acclZ;

	int16_t	m_gyroX;
	int16_t	m_gyroY;
	int16_t	m_gyroZ;
};
////////////////////////////////////////////////////////////////////////////////////////////////////
#endif
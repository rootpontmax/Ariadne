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

	int16_t	GetRawGyroX() const { return m_rawGyroX; }
	int16_t	GetRawGyroY() const { return m_rawGyroY; }
	int16_t	GetRawGyroZ() const { return m_rawGyroZ; }


	float	GetAcclX() const { return m_acclX; }
	float 	GetAcclY() const { return m_acclY; }
	float	GetAcclZ() const { return m_acclZ; }

	float	GetGyroX() const { return m_gyroX; }
	float 	GetGyroY() const { return m_gyroY; }
	float	GetGyroZ() const { return m_gyroZ; }

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

	int16_t	m_rawGyroX;
	int16_t	m_rawGyroY;
	int16_t	m_rawGyroZ;

	float	m_acclX;
	float	m_acclY;
	float	m_acclZ;

	float	m_gyroX;
	float	m_gyroY;
	float	m_gyroZ;
};
////////////////////////////////////////////////////////////////////////////////////////////////////
#endif
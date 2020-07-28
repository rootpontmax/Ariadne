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

	int16_t	GetAcclX() const { return m_acclX; }
	int16_t	GetAcclY() const { return m_acclY; }
	int16_t	GetAcclZ() const { return m_acclZ; }

	int16_t	GetGyroX() const { return m_gyroX; }
	int16_t	GetGyroY() const { return m_gyroY; }
	int16_t	GetGyroZ() const { return m_gyroZ; }

private:

	// Aux init methods
	void 	Setup();
	void	Calibrate();

	// Working functions
	void	ReadData();

	int16_t	m_acclX;
	int16_t	m_acclY;
	int16_t	m_acclZ;

	int16_t	m_gyroX;
	int16_t	m_gyroY;
	int16_t	m_gyroZ;
};
////////////////////////////////////////////////////////////////////////////////////////////////////
#endif
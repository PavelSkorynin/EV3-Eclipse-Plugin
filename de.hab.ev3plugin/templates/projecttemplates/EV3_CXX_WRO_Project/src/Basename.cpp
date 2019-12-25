//============================================================================
// Name        : $(baseName).cpp
// Author      : $(author)
// Version     :
// Copyright   : $(copyright)
// Description : EV3 WRO C++ template
//============================================================================

#include <ev3.h>
#include "move.hpp"

const int RED_COLOR = 0;
const int YELLOW_COLOR = 0;
const int GREEN_COLOR = 0;
const int BLUE_COLOR = 0;

int colors[4];

int main()
{
	auto ev3 = new ev3::EV3();

	// Инициализация устройств
	ev3->initSensors(ev3::Sensor::Mode::COLOR_REFLECT, ev3::Sensor::Mode::COLOR_REFLECT, ev3::Sensor::Mode::COLOR_RGB);
	auto leftLight = ev3->getReflectedLightSensor(ev3::Sensor::Port::P1);
	auto rightLight = ev3->getReflectedLightSensor(ev3::Sensor::Port::P2);
	auto colorSensor = ev3->getColorSensor(ev3::Sensor::Port::P3);
	auto leftMotor = ev3->getMotor(ev3::Motor::Port::A);
	auto rightMotor = ev3->getMotor(ev3::Motor::Port::B);

	// калибровка
	leftLight->setMinValue(0);
	leftLight->setMaxValue(100);
	rightLight->setMinValue(0);
	rightLight->setMaxValue(100);
	colorSensor->setMaxRValue(150);
	colorSensor->setMaxGValue(150);
	colorSensor->setMaxBValue(150);
	colors[RED_COLOR] = 0;
	colors[YELLOW_COLOR] = 40;
	colors[GREEN_COLOR] = 130;
	colors[BLUE_COLOR] = 240;

	// движение
	while (getColor(ev3, colorSensor, colors, 4, 0.1f) != RED_COLOR) {
		moveByEncoder(ev3, leftMotor, rightMotor, 20);
	}
	stop(ev3, leftMotor, rightMotor);

	delete ev3;
}

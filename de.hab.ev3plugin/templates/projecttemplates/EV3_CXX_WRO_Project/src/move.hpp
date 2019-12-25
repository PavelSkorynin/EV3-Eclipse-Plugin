/*
 * move_on_line.hpp
 *
 *  Created on: 30 сент. 2019 г.
 *      Author: Pavel
 */

#pragma once

#include <ev3.h>
#include <Motor.h> // для моторов
#include <Sensor.h> // для датчиков
#include <memory>

// остановка моторов
void stop(ev3::EV3 *ev3, ev3::MotorPtr leftMotor, ev3::MotorPtr rightMotor);
// движение вперёд на определённое расстояние в градусах енкодера
void moveByEncoder(ev3::EV3 *ev3, ev3::MotorPtr leftMotor,
		ev3::MotorPtr rightMotor, int encoderDistance, int maxPower = 70);
// движение по дуге на определённое расстояние в градусах енкодера
void moveByEncoderArc(ev3::EV3 *ev3, ev3::MotorPtr leftMotor,
		ev3::MotorPtr rightMotor, int encoderDistanceLeft, int encoderDistanceRight, int maxPower = 70);
// плавная остановка за определённое число градусов енкодера
void stopByEncoder(ev3::EV3 *ev3, ev3::MotorPtr leftMotor,
		ev3::MotorPtr rightMotor, int encoderDistance, int maxPower = 70);
// движение по линии на определённое расстояние в градусах енкодера
void moveOnLine(ev3::EV3 *ev3, ev3::MotorPtr leftMotor,
		ev3::MotorPtr rightMotor, ev3::SensorPtr leftLight,
		ev3::SensorPtr rightLight, int encoderDistance, int maxPower = 70);
// движение по линии на определённое расстояние в градусах енкодера
void moveOnLineToCross(ev3::EV3 *ev3, ev3::MotorPtr leftMotor,
		ev3::MotorPtr rightMotor, ev3::SensorPtr leftLight,
		ev3::SensorPtr rightLight, int numberOfCrosses = 1, int maxPower = 70);
// остановка по линии. Движение вдоль линии до определённого значения енкодера
void stopOnLine(ev3::EV3 *ev3, ev3::MotorPtr leftMotor,
		ev3::MotorPtr rightMotor, ev3::SensorPtr leftLight,
		ev3::SensorPtr rightLight, int encoderDistance, int maxPower = 70);
// поворот по енкодеру
void rotateByEncoder(ev3::EV3 *ev3, ev3::MotorPtr leftMotor,
		ev3::MotorPtr rightMotor, int numberOfDegrees, int maxPower = 70);
// поворот до линии
void rotateToLine(ev3::EV3 *ev3, ev3::MotorPtr leftMotor,
		ev3::MotorPtr rightMotor, ev3::SensorPtr leftLight,
		ev3::SensorPtr rightLight, int numberOfDegrees, int maxPower = 70);
// выравнивание на линии на месте
void alignOnLine(ev3::EV3 *ev3, ev3::MotorPtr leftMotor,
		ev3::MotorPtr rightMotor, ev3::SensorPtr leftLight,
		ev3::SensorPtr rightLight, int maxPower = 70);

const int BLACK_COLOR = -1;
const int WHITE_COLOR = -2;
const int NO_COLOR = -3;


// считывание цвета. Возвращает индекс цвета в массиве, который передан в качестве параметра.
int getColor(ev3::EV3 *ev3, std::shared_ptr<ev3::ColorSensor> colorSensor,
		int* colors, size_t colorsCount, float duration = 0.1f);
// упрощённая версия getColor с определёнными заранее цветами:
// {355 /*RED*/, 36 /*YELLOW*/, 140 /*GREEN*/, 220 /*BLUE*/}
int getColor(ev3::EV3 *ev3, std::shared_ptr<ev3::ColorSensor> colorSensor, float duration = 0.1f);

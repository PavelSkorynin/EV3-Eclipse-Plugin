/*
 * move_on_line.cpp
 *
 *  Created on: 30 сент. 2019 г.
 *      Author: Pavel
 */

#include "move.hpp"

#include <PID.h>
#include <Sensor.h> // для датчиков
#include <Motor.h> // для моторов
#include <cmath>
#include <EV3Math.h>

//	auto file = fopen("/home/root/lms2012/prjs/lesson3/log.csv", "w");

void stop(ev3::EV3 *ev3, ev3::MotorPtr leftMotor, ev3::MotorPtr rightMotor) {
	leftMotor->setPower(0);
	rightMotor->setPower(0);
	ev3->runLoop([&](float timestamp) -> bool {
		return abs(leftMotor->getActualSpeed()) + abs(rightMotor->getActualSpeed()) > 2;
	});
}

void moveByEncoder(ev3::EV3 *ev3, ev3::MotorPtr leftMotor, ev3::MotorPtr rightMotor, int encoderDistance, int maxPower) {
	moveByEncoderArc(ev3, leftMotor, rightMotor, encoderDistance, encoderDistance, maxPower);
}

// движение по дуге на определённое расстояние в градусах енкодера
void moveByEncoderArc(ev3::EV3 *ev3, ev3::MotorPtr leftMotor,
		ev3::MotorPtr rightMotor, int encoderDistanceLeft, int encoderDistanceRight, int maxPower) {
	bool toggleLeft = false, toggleRight = false;
	if (encoderDistanceLeft < 0) {
		leftMotor->toggleDirection();
		encoderDistanceLeft = -encoderDistanceLeft;
		toggleLeft = true;
	}
	if (encoderDistanceRight < 0) {
		rightMotor->toggleDirection();
		encoderDistanceRight = -encoderDistanceRight;
		toggleRight = true;
	}
	leftMotor->resetEncoder();
	rightMotor->resetEncoder();
	// инициализируем ПД-регулятор со значениями по умолчанию
	ev3::PID pd(0.4f, 0.0f, 1.2f);

	float scaleLeft = 1, scaleRight = 1;

	if (encoderDistanceLeft < encoderDistanceRight) {
		scaleLeft = (float)encoderDistanceLeft / encoderDistanceRight;
	} else {
		scaleRight = (float)encoderDistanceRight / encoderDistanceLeft;
	}

	// устанавливаем правило для вычисления ошибки в ПД-регуляторе
	// пересчёт значений будет происходить автоматически при обновлении ПД
	pd.setError(ev3::WireF(leftMotor->getEncoderWire()) * scaleRight - ev3::WireF(rightMotor->getEncoderWire()) * scaleLeft);

	if (scaleLeft * scaleRight > 0.5f) {
		// устанавливаем правила для обновления скорости моторов
		// пересчёт значений будет происходить автоматически при обновлении выходов в EV3
		leftMotor->setPower(ev3::WireI([&] () -> int {
			if (pd.getPower() > 0) {
				auto delta = ev3::clamp<float>(maxPower * pd.getPower() / 50.0f, -abs(maxPower), abs(maxPower));
				return (maxPower - delta) * scaleLeft;
			}
			return maxPower * scaleLeft;
		}));
		rightMotor->setPower(ev3::WireI([&] () -> int {
			if (pd.getPower() < 0) {
				auto delta = ev3::clamp<float>(maxPower * pd.getPower() / 50.0f, -abs(maxPower), abs(maxPower));
				return (maxPower + delta) * scaleRight;
			}
			return maxPower * scaleRight;
		}));
	} else {
		// устанавливаем правила для обновления скорости моторов
		// пересчёт значений будет происходить автоматически при обновлении выходов в EV3
		leftMotor->setPower(ev3::WireI([&] () -> int {
			if (encoderDistanceLeft < encoderDistanceRight) {
				return ev3::clamp<float>(-pd.getPower() * maxPower / 50.0f, -abs(maxPower), abs(maxPower));
			} else {
				return maxPower;
			}
		}));
		rightMotor->setPower(ev3::WireI([&] () -> int {
			if (encoderDistanceLeft < encoderDistanceRight) {
				return maxPower;
			} else {
				return ev3::clamp<float>(pd.getPower() * maxPower / 50.0f, -abs(maxPower), abs(maxPower));
			}
		}));

	}

	// запускаем основной цикл. Он выполняется синхронно, пока лямбда внутри него не вернёт false
	// показания датчиков и выходы на моторы обновляются автоматически, поэтому нам остаётся только
	// обновить ПД-регулятор
	ev3->runLoop([&](float timestamp) -> bool {

		pd.update(timestamp);

		// останавливаемся, когда среднее значение на енкодерах достигло желаемого
		return leftMotor->getEncoder() < encoderDistanceLeft || rightMotor->getEncoder() < encoderDistanceRight;
	});

	leftMotor->setPower(leftMotor->getPower());
	rightMotor->setPower(rightMotor->getPower());

	if (toggleLeft) {
		leftMotor->toggleDirection();
	}
	if (toggleRight) {
		rightMotor->toggleDirection();
	}
}

void stopByEncoder(ev3::EV3 *ev3, ev3::MotorPtr leftMotor, ev3::MotorPtr rightMotor, int encoderDistance, int maxPower) {
	leftMotor->resetEncoder();
	rightMotor->resetEncoder();

	int minPower = 7;

    ev3::PID pdLeft(0.5f, 0, 1.2f);
    ev3::PID pdRight(0.5f, 0, 1.2f);

    // устанавливаем правило для вычисления ошибки в ПД-регуляторе
    // пересчёт значений будет происходить автоматически при обновлении ПД
    pdLeft.setError(ev3::WireF(encoderDistance) - ev3::WireF(leftMotor->getEncoderWire()));
    pdRight.setError(ev3::WireF(encoderDistance) - ev3::WireF(rightMotor->getEncoderWire()));

	// устанавливаем правила для обновления скорости моторов
	// пересчёт значений будет происходить автоматически при обновлении выходов в EV3
	leftMotor->setPower(ev3::WireI([&] () -> int {
		const float anchor = 50;
		const float scale = 0.75f;
		const float value = pdLeft.getPower() / anchor;
		if (value > 0) {
			return ev3::clamp<int>(powf(value, 1.2f) * anchor * scale + minPower, -abs(maxPower), abs(maxPower));
		} else {
			return ev3::clamp<int>(-powf(-value, 1.2f) * anchor * scale - minPower, -abs(maxPower), abs(maxPower));
		}
	}));
	rightMotor->setPower(ev3::WireI([&] () -> int {
		const float anchor = 50;
		const float scale = 0.75f;
		const float value = pdRight.getPower() / anchor;
		if (value > 0) {
			return ev3::clamp<int>(powf(value, 1.2f) * anchor * scale + minPower, -abs(maxPower), abs(maxPower));
		} else {
			return ev3::clamp<int>(-powf(-value, 1.2f) * anchor * scale - minPower, -abs(maxPower), abs(maxPower));
		}
	}));

	// запускаем основной цикл. Он выполняется синхронно, пока лямбда внутри него не вернёт false
	// показания датчиков и выходы на моторы обновляются автоматически, поэтому нам остаётся только
	// обновить ПД-регулятор
	ev3->runLoop([&](float timestamp) -> bool {
        pdLeft.update(timestamp);
        pdRight.update(timestamp);

        // останавливаемся, когда среднее значение на енкодерах достигло желаемого
		return abs(leftMotor->getEncoder() + rightMotor->getEncoder() - encoderDistance * 2) > 4 || (abs(leftMotor->getActualSpeed()) + abs(rightMotor->getActualSpeed())) > minPower;
	});
	stop(ev3, leftMotor, rightMotor);
}

void moveOnLine(ev3::EV3 *ev3, ev3::MotorPtr leftMotor, ev3::MotorPtr rightMotor, ev3::SensorPtr leftLight, ev3::SensorPtr rightLight, int encoderDistance, int maxPower) {
	leftMotor->resetEncoder();
	rightMotor->resetEncoder();

	// инициализируем ПД-регулятор со значениями по умолчанию
	ev3::PID pd(0.3f, 0, 0.9f);

	// устанавливаем правило для вычисления ошибки в ПД-регуляторе
	// пересчёт значений будет происходить автоматически при обновлении ПД
	pd.setError(rightLight->getValueWire() - leftLight->getValueWire());

	// устанавливаем правила для обновления скорости моторов
	// пересчёт значений будет происходить автоматически при обновлении выходов в EV3
	leftMotor->setPower(std::function<int()>([&] () -> int {
		auto delta = ev3::clamp<float>(maxPower * pd.getPower() / 50.0f, -abs(maxPower), abs(maxPower));
		if (delta > 0) {
			return maxPower - delta;
		}
		return maxPower;
	}));
	rightMotor->setPower(std::function<int()>([&] () -> int {
		auto delta = ev3::clamp<float>(maxPower * pd.getPower() / 50.0f, -abs(maxPower), abs(maxPower));
		if (pd.getPower() < 0) {
			return maxPower + delta;
		}
		return maxPower;
	}));

	// запускаем основной цикл. Он выполняется синхронно, пока лямбда внутри него не вернёт false
	// показания датчиков и выходы на моторы обновляются автоматически, поэтому нам остаётся только
	// обновить ПД-регулятор
	ev3->runLoop([&](float timestamp) -> bool {

		pd.update(timestamp);

		// останавливаемся, когда среднее значение на енкодерах достигло желаемого
		return (leftMotor->getEncoder() + rightMotor->getEncoder()) < encoderDistance * 2;
	});
	leftMotor->setPower(leftMotor->getPower());
	rightMotor->setPower(rightMotor->getPower());
}

void moveOnLineToCross(ev3::EV3 *ev3, ev3::MotorPtr leftMotor, ev3::MotorPtr rightMotor, ev3::SensorPtr leftLight, ev3::SensorPtr rightLight, int numberOfCrosses, int maxPower) {
	for (int cross = 0; cross < numberOfCrosses; ++cross) {
		if (cross != 0) {
			// немного проедем вперёд, чтобы выйти на линию без перекрёстка
			moveOnLine(ev3, leftMotor, rightMotor, leftLight, rightLight, 100, maxPower);
		}

		leftMotor->resetEncoder();
		rightMotor->resetEncoder();

		// инициализируем ПД-регулятор со значениями по умолчанию
		ev3::PID pd(0.4f, 0.0f, 1.2f);

		// устанавливаем правило для вычисления ошибки в ПД-регуляторе
		// пересчёт значений будет происходить автоматически при обновлении ПД
		pd.setError(rightLight->getValueWire() - leftLight->getValueWire());

		// устанавливаем правила для обновления скорости моторов
		// пересчёт значений будет происходить автоматически при обновлении выходов в EV3
		leftMotor->setPower(std::function<int()>([&] () -> int {
			auto delta = ev3::clamp<float>(maxPower * pd.getPower() / 50.0f, -abs(maxPower), abs(maxPower));
			if (pd.getPower() > 0) {
				return maxPower - delta;
			}
			return maxPower;
		}));
		rightMotor->setPower(std::function<int()>([&] () -> int {
			auto delta = ev3::clamp<float>(maxPower * pd.getPower() / 50.0f, -abs(maxPower), abs(maxPower));
			if (pd.getPower() < 0) {
				return maxPower + delta;
			}
			return maxPower;
		}));

		// инициализация окна с данными датчиков
		const int windowWidth = 20;
		int leftLightValues[windowWidth];
		int rightLightValues[windowWidth];
		int prevLeftEncoder = leftMotor->getEncoder();
		int prevRightEncoder = rightMotor->getEncoder();
		for (int i = 0; i < windowWidth; ++i) {
			leftLightValues[i] = leftLight->getValue();
			rightLightValues[i] = rightLight->getValue();
		}


		// запускаем основной цикл. Он выполняется синхронно, пока лямбда внутри него не вернёт false
		// показания датчиков и выходы на моторы обновляются автоматически, поэтому нам остаётся только
		// обновить ПД-регулятор
		ev3->runLoop([&](float timestamp) -> bool {

			pd.update(timestamp);

			int encoder = leftMotor->getEncoder();
			for (int i = prevLeftEncoder + 1; i <= encoder; ++i) {
				leftLightValues[i % windowWidth] = leftLight->getValue();
			}
			prevLeftEncoder = encoder;
			if (prevLeftEncoder < 0) {
				prevLeftEncoder -= (prevLeftEncoder / windowWidth - 1) * windowWidth;
			}
			int delta = leftLightValues[prevLeftEncoder % windowWidth] - leftLightValues[(prevLeftEncoder + 1) % windowWidth];
			if (delta < -25) {
				// яркость снизилась на 30 за windowWidth градусов енкодера
				int sum = 0;
				for (int i = 0; i < windowWidth; ++i) {
					sum += leftLightValues[i];
				}
				if (sum / windowWidth < 30) {
					return false;
				}
			}

			encoder = rightMotor->getEncoder();
			for (int i = prevRightEncoder + 1; i <= encoder; ++i) {
				rightLightValues[i % windowWidth] = rightLight->getValue();
			}
			prevRightEncoder = encoder;
			if (prevRightEncoder < 0) {
				prevRightEncoder -= (prevRightEncoder / windowWidth - 1) * windowWidth;
			}
			delta = rightLightValues[prevRightEncoder % windowWidth] - rightLightValues[(prevRightEncoder + 1) % windowWidth];
			if (delta < -25) {
				// яркость снизилась на 30 за windowWidth градусов енкодера
				int sum = 0;
				for (int i = 0; i < windowWidth; ++i) {
					sum += rightLightValues[i];
				}
				if (sum / windowWidth < 30) {
					return false;
				}
			}
			return true;
		});

	}

	leftMotor->setPower(leftMotor->getPower());
	rightMotor->setPower(rightMotor->getPower());
}

// остановка по линии. Движение вдоль линии до определённого значения енкодера
void stopOnLine(ev3::EV3 *ev3, ev3::MotorPtr leftMotor, ev3::MotorPtr rightMotor, ev3::SensorPtr leftLight, ev3::SensorPtr rightLight, int encoderDistance, int maxPower) {
	leftMotor->resetEncoder();
	rightMotor->resetEncoder();

	// инициализируем ПД-регулятор со значениями по умолчанию
	ev3::PID pd(0.4f, 0, 1.2f);

	// устанавливаем правило для вычисления ошибки в ПД-регуляторе
	// пересчёт значений будет происходить автоматически при обновлении ПД
	pd.setError(rightLight->getValueWire() - leftLight->getValueWire());

    ev3::PID pdEncoder(0.5f, 0.0f, 1.2f);

    // устанавливаем правило для вычисления ошибки в ПД-регуляторе
    // пересчёт значений будет происходить автоматически при обновлении ПД
    pdEncoder.setError(ev3::WireF(encoderDistance) - (ev3::WireF(leftMotor->getEncoderWire()) + ev3::WireF(leftMotor->getEncoderWire())) / 2.0f);

    const int minPower = 7;
	const float anchor = 50;
	const float scale = 1.0f;
	// устанавливаем правила для обновления скорости моторов
	// пересчёт значений будет происходить автоматически при обновлении выходов в EV3
	leftMotor->setPower(std::function<int()>([&] () -> int {
		const float value = pdEncoder.getPower() / anchor;
		float relativePower = 0;
		if (value > 0) {
			relativePower = ev3::clamp<int>(powf(value, 0.8f) * anchor * scale + minPower, -abs(maxPower), abs(maxPower));
		} else {
			relativePower = ev3::clamp<int>(-powf(-value, 0.8f) * anchor * scale - minPower, -abs(maxPower), abs(maxPower));
		}

		auto delta = ev3::clamp<float>(relativePower * pd.getPower() / 50.0f, -abs(relativePower), abs(relativePower));
		if (delta > 0) {
			return relativePower - delta;
		}
		return relativePower;
	}));
	rightMotor->setPower(std::function<int()>([&] () -> int {
		const float value = pdEncoder.getPower() / anchor;
		float relativePower = 0;
		if (value > 0) {
			relativePower = ev3::clamp<int>(powf(value, 0.8f) * anchor * scale + minPower, -abs(maxPower), abs(maxPower));
		} else {
			relativePower = ev3::clamp<int>(-powf(-value, 0.8f) * anchor * scale - minPower, -abs(maxPower), abs(maxPower));
		}

		auto delta = ev3::clamp<float>(relativePower * pd.getPower() / 50.0f, -abs(relativePower), abs(relativePower));
		if (delta < 0) {
			return relativePower + delta;
		}
		return relativePower;
	}));

	// запускаем основной цикл. Он выполняется синхронно, пока лямбда внутри него не вернёт false
	// показания датчиков и выходы на моторы обновляются автоматически, поэтому нам остаётся только
	// обновить ПД-регулятор
	ev3->runLoop([&](float timestamp) -> bool {

		pdEncoder.update(timestamp);
		pd.update(timestamp);

		// останавливаемся, когда среднее значение на енкодерах достигло желаемого
		return pdEncoder.getPower() > 4 || (abs(leftMotor->getActualSpeed()) + abs(rightMotor->getActualSpeed())) > 3;
	});
	stop(ev3, leftMotor, rightMotor);
}

// поворот по енкодеру
void rotateByEncoder(ev3::EV3 *ev3, ev3::MotorPtr leftMotor, ev3::MotorPtr rightMotor, int numberOfDegrees, int maxPower) {
	moveByEncoderArc(ev3, leftMotor, rightMotor, numberOfDegrees, -numberOfDegrees, maxPower);
}

// поворот до линии
void rotateToLine(ev3::EV3 *ev3, ev3::MotorPtr leftMotor, ev3::MotorPtr rightMotor, ev3::SensorPtr leftLight, ev3::SensorPtr rightLight, int numberOfDegrees, int maxPower) {
	if (numberOfDegrees < 0) {
		leftMotor->toggleDirection();
	} else {
		rightMotor->toggleDirection();
	}

	// поворот до линии
	leftMotor->resetEncoder();
	rightMotor->resetEncoder();
	ev3::PID pd(0.4f, 0.0f, 1.2f);

	pd.setError(ev3::WireF(leftMotor->getEncoderWire()) - ev3::WireF(rightMotor->getEncoderWire()));

	leftMotor->setPower(ev3::WireI([&] () -> int {
		if (pd.getPower() > 0) {
			auto delta = ev3::clamp<float>(maxPower * pd.getPower() / 50.0f, -abs(maxPower), abs(maxPower));
			return maxPower - delta;
		}
		return maxPower;
	}));
	rightMotor->setPower(ev3::WireI([&] () -> int {
		if (pd.getPower() < 0) {
			auto delta = ev3::clamp<float>(maxPower * pd.getPower() / 50.0f, -abs(maxPower), abs(maxPower));
			return maxPower + delta;
		}
		return maxPower;
	}));

	ev3->runLoop([&](float timestamp) -> bool {

		pd.update(timestamp);

		return leftLight->getValue() > 70 && rightLight->getValue() > 70;
	});

	if (numberOfDegrees < 0) {
		leftMotor->toggleDirection();
	} else {
		rightMotor->toggleDirection();
	}

	leftMotor->setPower(leftMotor->getPower());
	rightMotor->setPower(rightMotor->getPower());

	alignOnLine(ev3, leftMotor, rightMotor, leftLight, rightLight, maxPower);
}

// выравнивание на линии на месте
void alignOnLine(ev3::EV3 *ev3, ev3::MotorPtr leftMotor, ev3::MotorPtr rightMotor, ev3::SensorPtr leftLight, ev3::SensorPtr rightLight, int maxPower) {
	leftMotor->resetEncoder();
	rightMotor->resetEncoder();

	// инициализируем ПД-регулятор со значениями по умолчанию
	ev3::PID pd(0.16f, 0.0f, 0.8f);

	// устанавливаем правило для вычисления ошибки в ПД-регуляторе
	// пересчёт значений будет происходить автоматически при обновлении ПД
	pd.setError(rightLight->getValueWire() - leftLight->getValueWire());

	// устанавливаем правила для обновления скорости моторов
	// пересчёт значений будет происходить автоматически при обновлении выходов в EV3
	leftMotor->setPower(std::function<int()>([&] () -> int {
		auto delta = ev3::clamp<float>(maxPower * pd.getPower() / 50.0f, -abs(maxPower), abs(maxPower));
		return -delta;
	}));
	rightMotor->setPower(std::function<int()>([&] () -> int {
		auto delta = ev3::clamp<float>(maxPower * pd.getPower() / 50.0f, -abs(maxPower), abs(maxPower));
		return delta;
	}));

	// запускаем основной цикл. Он выполняется синхронно, пока лямбда внутри него не вернёт false
	// показания датчиков и выходы на моторы обновляются автоматически, поэтому нам остаётся только
	// обновить ПД-регулятор
	ev3->runLoop([&](float timestamp) -> bool {

		pd.update(timestamp);

		// останавливаемся, когда на датчиках света будет одинаковое значение и скорость на моторах будет минимальна
		return abs(leftLight->getValue() - rightLight->getValue()) > 20 || (abs(leftMotor->getActualSpeed()) + abs(rightMotor->getActualSpeed())) > 24;
	});
	stop(ev3, leftMotor, rightMotor);
}

int getColor(ev3::EV3 *ev3, std::shared_ptr<ev3::ColorSensor> colorSensor,
		int* colors, size_t colorsCount, float duration) {

	static std::vector<int> detectedColors;

	float startTimestamp = ev3->timestamp();

	// цвета, которые будем искать
	detectedColors.clear();

	// цикл для чтения цвета
	ev3->runLoop([&](float timestamp) {
		auto hsv = colorSensor->getHSVColor();

		if (hsv.h == 0 && hsv.s == 0 && hsv.v == 0) {
			detectedColors.emplace_back(NO_COLOR);
		} else if (hsv.v < 10) {
			detectedColors.emplace_back(BLACK_COLOR);
		} else if (hsv.s < 20 && hsv.v > 60) {
			detectedColors.emplace_back(WHITE_COLOR);
		} else {
			int minDistance = 1000;
			int colorIndex = 0;
			for (size_t i = 0; i < colorsCount; ++i) {
				// ищем, к какому цвету ближе всего увиденный
				// находим минимальное расстояние от h до цвета на циклической шкале
				int dist1 = abs(hsv.h - colors[i]);
				int dist2 = abs(hsv.h - colors[i] + 360);
				int dist3 = abs(hsv.h - colors[i] - 360);
				int minDist = std::min(dist1, std::min(dist2, dist3));
				if (minDist < minDistance) {
					colorIndex = i;
					minDistance = minDist;
				}
			}

			detectedColors.emplace_back(colorIndex);
		}

		return timestamp - startTimestamp < duration;
	});

	// инициализируем моды
	static std::vector<int> modes;
	modes.clear();
	modes.insert(modes.end(), colorsCount + 3, 0);
	// собираем статистику
	for (auto color : detectedColors) {
		if (color < 0) {
			color = colorsCount - color - 1;
		}
		modes[color]++;
	}

	// среди найденных цветов ищем наиболее встречаемый
	int maxCount = 0;
	int selectedIndex = 0;
	for (size_t i = 0; i < colorsCount + 3; ++i) {
		if (modes[i] > maxCount) {
			maxCount = modes[i];
			selectedIndex = i;
		}
	}
	if (selectedIndex >= (int)colorsCount) {
		selectedIndex = (int)colorsCount - selectedIndex - 1;
	}

	return selectedIndex;
}

// упрощённая версия getColor с определёнными заранее цветами:
// {355 /*RED*/, 36 /*YELLOW*/, 140 /*GREEN*/, 220 /*BLUE*/}
int getColor(ev3::EV3 *ev3, std::shared_ptr<ev3::ColorSensor> colorSensor, float duration) {
	static int colors[] = {355 /*RED*/, 36 /*YELLOW*/, 140 /*GREEN*/, 220 /*BLUE*/};

	return getColor(ev3, colorSensor, colors, 4, duration);
}

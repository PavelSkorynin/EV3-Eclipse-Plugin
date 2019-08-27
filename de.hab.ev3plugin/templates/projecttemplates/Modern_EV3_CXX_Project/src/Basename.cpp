//============================================================================
// Name        : $(baseName).cpp
// Author      : $(author)
// Version     :
// Copyright   : $(copyright)
// Description : Modern EV3 Hello World in C++
//============================================================================

// Подключаем стандартные зависимости C++
#include <string>

// Подключаем зависимости EV3 API
#include <ev3.h> // для блока EV3
#include <PD.h> // для ПД-регулятора
#include <Sensor.h> // для датчиков
#include <Motor.h> // для моторов

// точка запуска программы
int main()
{
	// создаём экземпляр EV3 для работы с блоком
	// не забыть потом удалить его!
	auto ev3 = new ev3::EV3();

	// отображаем приветственное сообщение на экране
	std::string greeting("Modern EV3 Hello World!");
	ev3->lcdPrintf(ev3::EV3::Color::BLACK, "%s\n", greeting.c_str());
	// ждём одну секунду
	ev3->wait(1);

	// получаем моторы, подключённые к портам A и B
	auto motorA = ev3->getMotor(ev3::Motor::Port::A);
	auto motorB = ev3->getMotor(ev3::Motor::Port::B);

	// получаем затчики цвета, подключенные к портам 1 и 2
	auto light1 = ev3->getSensor(ev3::Sensor::Port::P1, ev3::Sensor::Mode::COLOR_REFLECT);
	auto light2 = ev3->getSensor(ev3::Sensor::Port::P2, ev3::Sensor::Mode::COLOR_REFLECT);

	// инициализируем ПД-регулятор со значениями по умолчанию
	ev3::PD pd;

	// устанавливаем правило для вычисления ошибки в ПД-регуляторе
	// пересчёт значений будет происходить автоматически при обновлении ПД
	pd.setError(light1->getValue() - light2->getValue());

	// устанавливаем правила для обновления скорости моторов
	// пересчёт значений будет происходить автоматически при обновлении выходов в EV3
	motorA->setSpeed(50 + pd.getPower());
	motorB->setSpeed(50 - pd.getPower());

	// запускаем основной цикл. Он выполняется синхронно, пока лямбда внутри него не вернёт false
	// показания датчиков и выходы на моторы обновляются автоматически, поэтому нам остаётся только
	// обновить ПД-регулятор
	ev3->runLoop([&](float timestamp) -> bool {
		pd.update(timestamp);

		// останавливаемся, когда прошло 10 секунд от старта
		return timestamp < 10;
	});

	// удаляем EV3 и выходим из программы
	delete ev3;
	return 0;
}

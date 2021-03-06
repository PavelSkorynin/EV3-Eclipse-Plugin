//============================================================================
// Name        : $(baseName).cpp
// Author      : $(author)
// Version     :
// Copyright   : $(copyright)
// Description : Hello World in C++
//============================================================================

#include <ev3.h>
#include <string>

int main()
{
  auto ev3 = new ev3::EV3();

  std::string greeting("Hello World!");

  ev3->lcdPrintf(ev3::Color::BLACK, "%s\n", greeting.c_str());
  ev3->wait(2);

  delete ev3;
}

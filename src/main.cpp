#include <Arduino.h>
#include "mini-printer.h"
#include "test.h"

void setup()
{
  Serial.begin(115200);
  jx2r_init();
}

void loop()
{
  test_case1();
  test_case2();

  while (1)
  {
    delay(1000);
  }
}
#include <Arduino.h>
#include "mini-printer.h"

void test_printer(void)
{
  uint8_t buffer[48];

  for (int i = 0; i < 48; i++)
  {
    buffer[i] = 0x33;
  }

  jx2r_print_one_line(buffer, 48);
}

void setup()
{
  jx2r_init();
}

void loop()
{

  // jx2r_motor_run(100);
  for(int i = 0; i < 50; i++) {
    test_printer();
  }

  while (1)
  {
    delay(1000);
  }
}
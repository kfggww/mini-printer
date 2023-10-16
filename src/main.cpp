#include <Arduino.h>
#include "printer.h"
#include "test.h"

void setup()
{
  Serial.begin(115200);
  printer_init();

  xTaskCreate(printting_task, "printting_task", 1024, NULL, 2, NULL);
  xTaskCreate(check_paper_task, "check_paper_task", 1024, NULL, 1, NULL);
  xTaskCreate(check_temp_task, "check_temp_task", 1024, NULL, 1, NULL);
  xTaskCreate(check_battery_task, "check_battery_task", 1024, NULL, 1, NULL);
}

void loop()
{
  while (1)
  {
    delay(5000);
  }
}
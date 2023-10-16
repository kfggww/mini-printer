#include "printer.h"

void printting_task(void *arg)
{
    for (;;)
    {
        if (jx2r_printer_ready())
        {
            jx2r_print_one_line(printer_des.buffer[printer_des.read_ptr], 48);
            printer_des.read_ptr = (printer_des.read_ptr + 1) % PRINTER_MAX_NLINE;
            printer_des.size--;
            Serial.println("printer task run!!!");
        }
        delay(10);
    }
}

void check_temp_task(void *arg)
{
    for (;;)
    {
        printer_des.temp = jx2r_check_temp();
        delay(100);
    }
}

void check_paper_task(void *arg)
{
    for (;;)
    {
        printer_des.lackof_paper = jx2r_lackof_paper();
        delay(20);
    }
}

void check_battery_task(void *arg)
{
    for (;;)
    {
        printer_des.battery = analogReadMilliVolts(PIN_POWER_ADC) / 1000.0 * 2;
        delay(100);
    }
}
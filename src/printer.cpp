#include "printer.h"

PrinterDes printer_des;

void printer_des_init(void)
{
    memset(&printer_des, 0, sizeof(printer_des));
    printer_des.lackof_paper = true;
    printer_des.free = PRINTER_MAX_NLINE;
    printer_des.buffer_mutex = xSemaphoreCreateMutex();
}

void printer_init(void)
{
    printer_des_init();
    jx2r_init();
    ble_init();
}
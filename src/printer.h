#ifndef PRINTER_H
#define PRINTER_H

#include <Arduino.h>

#define PRINTER_MAX_NLINE 50

enum PinDefs
{
    // Motor pins
    PIN_MOTOR_AP = 23,
    PIN_MOTOR_BP = 21,
    PIN_MOTOR_AN = 22,
    PIN_MOTOR_BN = 19,

    // SPI pins
    PIN_SPI_SCK = 15,
    PIN_SPI_SDA = 13,

    // STB pins
    PIN_STB1 = 14,
    PIN_STB2 = 27,
    PIN_STB3 = 26,
    PIN_STB4 = 25,
    PIN_STB5 = 33,
    PIN_STB6 = 32,

    // Data latch pin
    PIN_LAT = 12,

    // Printer heat power enable pin
    PIN_VH_EN = 17,

    // Sensors pin
    PIN_POWER_ADC = 34,
    PIN_THER = 36,
    PIN_PHINT = 35,
};

enum MotorDirs
{
    Motor_Dir1,
    Motor_Dir2,
};

/* JX2R motor APIs */
void jx2r_motor_init(void);
void jx2r_motor_run(uint8_t nsteps, MotorDirs dir = Motor_Dir1);
void jx2r_motor_stop(void);

/* JX2R printer APIs */
void jx2r_spi_init(void);
void jx2r_spi_send(uint8_t *data, uint8_t len);
bool jx2r_printer_ready(void);
void jx2r_print_one_line(uint8_t *data, uint8_t len = 48);
void jx2r_power_on(void);
void jx2r_power_off(void);
void jx2r_init(void);

/* JX2R sensor APIs */
float jx2r_check_temp(void);
bool jx2r_lackof_paper(void);

/* Printer data */
struct PrinterDes
{
    uint8_t buffer[PRINTER_MAX_NLINE][48];
    uint8_t read_ptr;
    uint8_t write_ptr;
    uint8_t size;

    float temp;
    float battery;
    bool lackof_paper;

    bool connected;
};

extern PrinterDes printer_des;

void printer_des_init(void);
void printer_init(void);

/* BLE settings */
void ble_init(void);

/* Tasks */
void printting_task(void *arg);
void check_temp_task(void *arg);
void check_paper_task(void *arg);
void check_battery_task(void *arg);

#endif
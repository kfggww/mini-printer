#ifndef MINI_PRINTER_H
#define MINI_PRINTER_H

#include <Arduino.h>

enum PinDefs
{
    // motor pins
    Pin_Motor_AP = 23,
    Pin_Motor_BP = 21,
    Pin_Motor_AN = 22,
    Pin_Motor_BN = 19,

    // spi pins
    Pin_SPI_SCK = 15,
    Pin_SPI_SDA = 13,

    // stb pins
    Pin_STB1 = 14,
    Pin_STB2 = 27,
    Pin_STB3 = 26,
    Pin_STB4 = 25,
    Pin_STB5 = 33,
    Pin_STB6 = 32,

    // latch pin
    Pin_LAT = 12,

    // vh enable pin
    Pin_VH_EN = 17,
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
void jx2r_print_one_line(uint8_t *data, uint8_t len = 48);
void jx2r_power_on(void);
void jx2r_power_off(void);
void jx2r_init(void);

#endif
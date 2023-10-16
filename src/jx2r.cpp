#include <SPI.h>
#include "printer.h"

#define JX2R_HEAT_TIME_US 2000
#define JX2R_COOLING_TIME_US 200

static uint8_t stb_pins[6] = {
    PIN_STB1,
    PIN_STB2,
    PIN_STB3,
    PIN_STB4,
    PIN_STB5,
    PIN_STB6,
};
static uint8_t motor_pins[4] = {
    PIN_MOTOR_AP,
    PIN_MOTOR_BP,
    PIN_MOTOR_AN,
    PIN_MOTOR_BN,
};
static uint8_t motor_steps[][4] = {
    {1, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 1, 1},
    {0, 0, 1, 0},
    {0, 1, 1, 0},
    {0, 1, 0, 0},
    {1, 1, 0, 0},
    {1, 0, 0, 0},
};

void jx2r_motor_init(void)
{
    for (int i = 0; i < 4; i++)
    {
        pinMode(motor_pins[i], OUTPUT);
        digitalWrite(motor_pins[i], LOW);
    }
}

void jx2r_motor_run(uint8_t nsteps, MotorDirs dir)
{
    static uint8_t index = 0;

    while (nsteps > 0)
    {
        // run one step
        for (int i = 0; i < 4; i++)
        {
            digitalWrite(motor_pins[i], motor_steps[index][i]);
        }
        delay(2);

        nsteps--;
        if (dir == Motor_Dir1)
        {
            index = (index + 1) % 8;
        }
        else
        {
            index = index == 0 ? 7 : index - 1;
        }
    }
}

void jx2r_motor_stop(void)
{
    for (int i = 0; i < 4; i++)
    {
        digitalWrite(motor_pins[i], LOW);
    }
}

static SPIClass hspi = SPIClass(HSPI);
static SPISettings spi_settings = SPISettings(1000000, SPI_MSBFIRST, SPI_MODE0);

void jx2r_spi_init(void)
{
    hspi.begin(PIN_SPI_SCK, -1, PIN_SPI_SDA, -1);
    hspi.setFrequency(2000000);
}

void jx2r_spi_send(uint8_t *data, uint8_t len)
{
    hspi.beginTransaction(spi_settings);
    hspi.transfer(data, len);
    hspi.endTransaction();

    digitalWrite(PIN_LAT, LOW);
    delayMicroseconds(1);
    digitalWrite(PIN_LAT, HIGH);
}

bool jx2r_printer_ready(void)
{
    return printer_des.connected && printer_des.size > 0 && printer_des.temp < 50 && !printer_des.lackof_paper;
}

void jx2r_print_one_line(uint8_t *data, uint8_t len)
{
    jx2r_power_on();
    jx2r_spi_send(data, len);

    // Need to be on the right place first
    jx2r_motor_run(1);

    // Heat the paper
    for (int i = 0; i < 6; i++)
    {
        digitalWrite(stb_pins[i], HIGH);
        delayMicroseconds(JX2R_HEAT_TIME_US);
        digitalWrite(stb_pins[i], LOW);
        delayMicroseconds(JX2R_COOLING_TIME_US);
    }

    // Go on for the next line, since that the motor walk four steps per line
    jx2r_motor_run(3);
    jx2r_motor_stop();

    jx2r_power_off();
}

void jx2r_power_on(void)
{
    digitalWrite(PIN_VH_EN, HIGH);
}

void jx2r_power_off(void)
{
    digitalWrite(PIN_VH_EN, LOW);
}

void jx2r_init(void)
{
    jx2r_motor_init();

    pinMode(PIN_SPI_SCK, OUTPUT);
    pinMode(PIN_SPI_SDA, OUTPUT);
    jx2r_spi_init();

    for (int i = 0; i < 6; i++)
    {
        pinMode(stb_pins[i], OUTPUT);
        digitalWrite(stb_pins[i], LOW);
    }

    pinMode(PIN_LAT, OUTPUT);
    digitalWrite(PIN_LAT, HIGH);

    pinMode(PIN_VH_EN, OUTPUT);
    digitalWrite(PIN_VH_EN, HIGH);

    analogReadResolution(12);
    pinMode(PIN_PHINT, INPUT);
}

float jx2r_check_temp(void)
{

    const float B = 3950;
    const float R25 = 30;
    const float T25 = 273.15 + 25;

    float tx = 0.0;

    uint32_t v = analogReadMilliVolts(PIN_THER);
    float rx = 10.0 * v / (3300.0 - v);
    tx = B * T25 / (T25 * log(rx / R25) + B);

    return tx - 273.15;
}

bool jx2r_lackof_paper(void)
{
    return digitalRead(PIN_PHINT) == HIGH;
}
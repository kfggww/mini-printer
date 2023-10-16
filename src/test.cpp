#include "printer.h"

void test_case1(void)
{
    uint8_t buffer[48];

    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            memset(buffer, 0, 48);
            memset(buffer + 8 * i, 0x55, 8);
            jx2r_print_one_line(buffer);
        }
    }
}

void test_case2(void)
{
    uint8_t buffer[48];

    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            memset(buffer, 0, 48);
            memset(buffer + (5 - i) * 8, 0x55, 8);
            jx2r_print_one_line(buffer);
        }
    }
}
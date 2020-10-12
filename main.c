#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

struct flags
{
   uint8_t negative;
   uint8_t overflow;
   uint8_t brk;
   uint8_t decimal;
   uint8_t interrupt;
   uint8_t zero;
   uint8_t carry;


};

struct registers
{
   uint16_t counter;
   uint8_t accumulator;
   uint8_t x_register;
   uint8_t y_register;
   uint8_t status;
   uint8_t pStack;


};

int main()
{
    struct flags crr_flags;
    struct flags *p_crr_flags = &crr_flags;
    p_crr_flags->negative = 1;
    printf("%d", p_crr_flags->overflow);
    return 0;
}

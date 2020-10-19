#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

uint8_t cycles [256] =
{
        7, 6, 0, 8, 3, 3, 5, 5, 3, 2, 2, 2, 4, 4, 6, 6, //0x
        2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7, //1x
        6, 6, 0, 8, 3, 3, 5, 5, 4, 2, 2, 2, 4, 4, 6, 6, //2x
        2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7, //3x
        6, 6, 0, 8, 3, 3, 5, 5, 3, 2, 2, 2, 3, 4, 6, 6, //4x
        2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7, //5x
        6, 6, 0, 8, 3, 3, 5, 5, 4, 2, 2, 2, 5, 4, 6, 6, //6x
        2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7, //7x
        2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4, //8x
        2, 6, 0, 6, 4, 4, 4, 4, 2, 5, 2, 5, 5, 5, 5, 5, //9x
        2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4, //Ax
        2, 5, 0, 5, 4, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, //Bx
        2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6, //Cx
        2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7, //Dx
        2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6, //Ex
        2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7, //Fx

};


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
   uint8_t crrent_opcode;   //
   uint16_t data;            // Opcode + Data


   uint16_t counter;
   uint8_t accumulator;
   uint8_t x_register;
   uint8_t y_register;
   uint8_t status;
   uint8_t pStack;

   struct flags flags_now;

};

uint8_t recount_status (struct registers *Registers)
{
    Registers->status = (Registers->flags_now.negative << 7) | (Registers->flags_now.overflow << 6)  | (Registers->flags_now.brk << 4)
                        | (Registers->flags_now.decimal << 3)  | (Registers->flags_now.interrupt << 2) |  (Registers->flags_now.zero << 1)
                        | Registers->flags_now.carry;
    return (Registers->status);
}
uint8_t read_Byte (uint16_t Address)
{
    return 1;
}

uint8_t write_Byte (uint16_t Address, uint8_t Byte)
{
    return 1;
}

uint8_t fetch_byte (struct registers *Registers)
{
    read_Byte(Registers->counter);
    Registers->counter++;
    return 1;
}

uint8_t push_stack (struct registers *Registers, uint8_t Byte)
{
    write_Byte(0x0100 + Registers->pStack, Byte);
    Registers->pStack--;
    return 1;
}

uint8_t pull_stack (struct registers *Registers)
{
    return (read_Byte(0x0100 + Registers->pStack++));
}
//Adressing modes

uint8_t check_negative (struct registers *Registers)
{
    Registers->flags_now.negative= (Registers->accumulator >> 7) ? 1 : 0;
}

uint8_t check_zero (struct registers *Registers)
{
    Registers->flags_now.zero= (Registers->accumulator == 0) ? 1 : 0;
}

void index_indirect (struct registers *Registers)
{
    Registers->counter = 0x0000 |  (fetch_byte(Registers) + Registers->x_register); // Fetch Bal, counter = BAL + X
    uint8_t ADL = fetch_byte(Registers);
    uint8_t AHL = fetch_byte(Registers);
    Registers->counter = (AHL << 8) |  ADL;
    Registers->data = fetch_byte(Registers);

}


void BRK (struct registers *Registers)
{
   fetch_byte(Registers); // fetch opcode and discard
   push_stack (Registers, (Registers->counter) >> 8); // PCH
   push_stack (Registers, Registers->counter); // PCL
   Registers->flags_now.brk = 1;
   push_stack(Registers, recount_status(Registers));
   //fetch pcl and pch

}

void LDA (struct registers *Registers)
{
    Registers->accumulator = Registers->data;
    check_zero(Registers);
    check_negative(Registers);
}
void CLC (struct registers *Registers)
{
    Registers->flags_now.carry = 0;
}

void SED (struct registers *Registers)
{
    Registers->flags_now.decimal = 1;
}

void STA (struct registers *Registers)
{
    write_Byte(Registers->data, Registers->accumulator);
}

void CLD (struct registers *Registers)
{
    Registers->flags_now.decimal = 0;
}

void ADC (struct registers *Registers)
{
    uint16_t sum = Registers->accumulator + Registers->data + Registers->flags_now.carry;
    uint8_t sign_acc_bef= Registers->accumulator >> 7;
    uint8_t sign_data = Registers->data >> 7;
    Registers->accumulator = sum;
    uint8_t sign_acc_aft = Registers->accumulator >> 7;
    Registers->flags_now.overflow = ((~sign_acc_bef&~sign_data&sign_acc_aft)|(sign_acc_bef&sign_data&~sign_acc_aft) == 1) ? 1 : 0; //check later
    Registers->flags_now.carry = (sum > 255) ? 1 : 0;
    check_zero(Registers);
    check_negative(Registers);


}


void (*pWhatMode[256])(struct registers *) = {BRK };

int main()
{
    struct flags crr_flags;
    struct flags *p_crr_flags = &crr_flags;
    p_crr_flags->negative = 1;
    printf("%d", p_crr_flags->overflow);
    return 0;
}

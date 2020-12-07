#ifndef VARIABLES_H
#define VARIABLES_H

#include <stdint.h>
#include <stdio.h>

#define A Processor->accumulator
#define X Processor->x_register
#define Y Processor->y_register
#define S Processor->status
#define PC Processor->counter
#define SP Processor->pStack


#define OPCODE Processor->crrent_opcode
#define ADDR Processor->address

#define CF Processor->flags_now.carry
#define NF Processor->flags_now.negative
#define ZF Processor->flags_now.zero
#define DF Processor->flags_now.decimal
#define BF1 Processor->flags_now.brk1
#define BF2 Processor->flags_now.brk2
#define IF Processor->flags_now.interrupt
#define OF Processor->flags_now.overflow

#define MEMADDR Processor->memory_addr

#define BPL 0x10
#define BMI 0x30
#define BVC 0x50
#define BVS 0x70
#define BCC 0x90
#define BCS 0xB0
#define BNE 0xD0
#define BEQ 0xF0

#define INTERRUPT_PERIOD1 96233500
#define INTERRUPT_PERIOD2 96307500

#define read_Byte(ADDRESS)  (Processor->memory_addr)[ADDRESS]
#define write_Byte(ADDRESS,DATA)  (Processor->memory_addr)[ADDRESS]=DATA



extern uint8_t cycles[256];


typedef struct {



        uint8_t negative;
        uint8_t overflow;
        uint8_t brk1;
        uint8_t brk2;
        uint8_t decimal;
        uint8_t interrupt;
        uint8_t zero;
        uint8_t carry;
}flags;


typedef struct
{
        uint8_t crrent_opcode; //
        uint16_t address;       //


        uint16_t counter;
        uint8_t accumulator;
        uint8_t x_register;
        uint8_t y_register;
        uint8_t status;
        uint8_t pStack;


        uint8_t flag_acc_address;

        uint8_t add_cycles;

        flags flags_now;

        uint8_t * memory_addr;

}processor;



uint8_t recount_status (processor *Processor);
void recount_flags(processor *Processor);
uint8_t fetch_byte (processor *Processor);
uint8_t push_stack (processor *Processor, uint8_t Byte);
uint8_t pull_stack (processor *Processor);
uint8_t check_n_z (processor *Processor, uint8_t reg);
#endif // VARIABLES_H

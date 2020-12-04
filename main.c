

#include <stdio.h>
#include <stdlib.h>


#include "variables.h"
#include "opcodes.h"




uint8_t cycles [256] =
{
        7, 6, 1, 1, 1, 3, 5, 1, 3, 2, 2, 1, 1, 4, 6, 1, //0x
        2, 5, 1, 1, 1, 4, 6, 1, 2, 4, 2, 1, 4, 4, 7, 1, //1x
        6, 6, 1, 1, 3, 3, 5, 1, 4, 2, 2, 1, 4, 4, 6, 1, //2x
        2, 5, 1, 1, 1, 4, 6, 1, 2, 4, 2, 1, 4, 4, 7, 1, //3x
        6, 6, 1, 1, 1, 3, 5, 1, 3, 2, 2, 1, 3, 4, 6, 1, //4x
        2, 5, 1, 1, 1, 4, 6, 1, 2, 4, 2, 1, 4, 4, 7, 1, //5x
        6, 6, 1, 1, 1, 3, 5, 1, 4, 2, 2, 1, 5, 4, 6, 1, //6x
        2, 5, 1, 1, 1, 4, 6, 1, 2, 4, 2, 1, 4, 4, 7, 1, //7x
        2, 6, 1, 1, 3, 3, 3, 1, 2, 2, 2, 1, 4, 4, 4, 1, //8x
        2, 6, 1, 1, 4, 4, 4, 1, 2, 5, 2, 1, 5, 5, 5, 1, //9x
        2, 6, 2, 1, 3, 3, 3, 1, 2, 2, 2, 1, 4, 4, 4, 1, //Ax
        2, 5, 1, 1, 4, 4, 4, 1, 4, 2, 4, 1, 4, 4, 4, 1, //Bx
        2, 6, 1, 1, 3, 3, 5, 1, 2, 2, 2, 1, 4, 4, 6, 1, //Cx
        2, 5, 1, 1, 1, 4, 6, 1, 2, 4, 2, 1, 4, 4, 7, 1, //Dx
        2, 6, 1, 1, 3, 3, 5, 1, 2, 2, 2, 1, 4, 4, 6, 1, //Ex
        2, 5, 1, 1, 1, 4, 6, 1, 2, 4, 2, 1, 4, 4, 7, 1, //Fx

};

uint8_t stop = 1;

uint8_t recount_status (processor *Processor){

        uint8_t status = (NF << 7) | (OF << 6)  | (BF1 << 5) | (BF2 << 4)| (DF << 3)  | (IF << 2) |  (ZF << 1) | CF;
        return (status);
}

void recount_flags(processor *Processor){

        NF = S >> 7;
        OF = (S >> 6) & 0x01;
        BF1 = (S >> 5) & 0x01;
        BF2 = (S >> 4) & 0x01;
        DF = (S >> 3) & 0x01;
        IF = (S >> 2) & 0x01;
        ZF = (S >> 1) & 0x01;
        CF = S & 0x01;

}


uint8_t inv_bit (uint8_t data){

        return (0x01 & ~data);

}

void Load (uint8_t * memory){

        FILE *fp;
        fp = fopen("6502_functional_test.bin", "r");
        if (fp == NULL)
                printf("Error loading memory\n");
        for (uint32_t i =0; i <= 0xFFFF; i++)
                fscanf(fp, "%c", &memory[i]);
        fclose(fp);



}


uint8_t reset (processor *Processor){

        uint8_t PCL = read_Byte(0xFFFC);
        uint8_t PHL = read_Byte(0xFFFD);

        PC = (PHL << 8) | PCL;

        IF = 1;

        Processor->flag_acc_adress = 0;

        Processor->add_cycles = 0;

        return 0;


}

uint8_t fetch_byte (processor *Processor){

        uint8_t data = read_Byte(PC++);
        return data;
}

uint8_t push_stack (processor *Processor, uint8_t Byte){

        write_Byte(0x0100 | SP--, Byte);
        return 1;
}

uint8_t pull_stack (processor *Processor){

        return (read_Byte(0x0100 + ++SP));
}
//Adressing modes

uint8_t check_n_z (processor *Processor, uint8_t reg){

        NF = (reg >> 7) ? 1 : 0;
        ZF = (reg == 0) ? 1 : 0;
        return 0;

}


uint16_t INDX (processor *Processor){ // X-indexed, indirect

        uint16_t offset = 0x00FF & (fetch_byte(Processor) + X); // Fetch Bal, counter = BAL + X
        uint8_t ADL = read_Byte(offset);
        uint8_t AHL = read_Byte(offset+ 1);
        uint16_t adr = (AHL << 8) |  ADL;
        return adr;

}
uint16_t IMP(processor *Processor) { // implied

        return PC;

}

uint16_t ABS(processor *Processor) { //absolute

        uint8_t adl = fetch_byte(Processor);
        uint8_t ahl = fetch_byte(Processor);
        return (ahl << 8 | adl);


}
uint16_t ZP (processor *Processor){  // zero_page

        return (0x00FF & fetch_byte(Processor));


}

uint16_t ZPX (processor *Processor) {

        return( 0x00FF & (fetch_byte(Processor) + X));
}


uint16_t ZPY (processor *Processor) {

        return  (0x00FF & (fetch_byte(Processor) + Y));


}

uint16_t IMM (processor *Processor){ // immediate

        return (PC++);

}

uint16_t REL (processor *Processor){

        uint8_t adl = fetch_byte(Processor);
        uint8_t adh = fetch_byte(Processor);
        uint16_t Adr = (adh << 8 )| adl;
        adl = read_Byte(Adr);
        adh = read_Byte(Adr+1);
        return (adh << 8 | adl);

}

uint16_t ABSX (processor *Processor){ // absolute, X-indexed

        uint8_t bal = fetch_byte(Processor);
        uint8_t bah = fetch_byte(Processor);
        if (bal + X > 255)
                Processor->add_cycles+=1;
        return ((bah << 8 | bal ) + X);

}

uint16_t ABSY (processor *Processor){ // absolute, Y-indexed

        uint8_t bal = fetch_byte(Processor);
        uint8_t bah = fetch_byte(Processor);
        uint16_t adr = ((bah << 8 )| bal ) + Y;
        if (bal + Y > 255)
                Processor->add_cycles+=1;
        return adr;

}

uint16_t INDY (processor *Processor){   // indirect, Y-indexed

        uint16_t ial  = 0x00FF & fetch_byte(Processor);
        uint8_t bal = read_Byte(ial);
        uint8_t bah = read_Byte(ial+1);


        if (bal + Y > 255)
                Processor->add_cycles+=1;
        return ((bah << 8 | bal) + Y);



}


uint16_t ACC (processor *Processor)
{

        Processor->flag_acc_adress = 1;
        return 0;
}


static void (*pWhatMode[256]) (processor *) = {BRK, ORA, NTG, NTG, NTG, ORA, ASL, NTG, PHP, ORA, ASL, NTG, NTG, ORA, ASL, NTG,
                                        Bxx, ORA, NTG, NTG, NTG, ORA, ASL, NTG, CLC, ORA, NTG, NTG, NTG, ORA, ASL, NTG,
                                        JSR, AND, NTG, NTG, BIT, AND, ROL, NTG, PLP, AND, ROL, NTG, BIT, AND, ROL, NTG,
                                        Bxx, AND, NTG, NTG, NTG, AND, ROL, NTG, SEC, AND, NTG, NTG, NTG, AND, ROL, NTG,
                                        RTI, EOR, NTG, NTG, NTG, EOR, LSR, NTG, PHA, EOR, LSR, NTG, JMP, EOR, LSR, NTG,
                                        Bxx, EOR, NTG, NTG, NTG, EOR, LSR, NTG, CLI, EOR, NTG, NTG, NTG, EOR, LSR, NTG,
                                        RTS, ADC, NTG, NTG, NTG, ADC, ROR, NTG, PLA, ADC, ROR, NTG, JMP, ADC, ROR, NTG,
                                        Bxx, ADC, NTG, NTG, NTG, ADC, ROR, NTG, SEI, ADC, NTG, NTG, NTG, ADC, ROR, NTG,
                                        NTG, STA, NTG, NTG, STY, STA, STX, NTG, DEY, NTG, TXA, NTG, STY, STA, STX, NTG,
                                        Bxx, STA, NTG, NTG, STY, STA, STX, NTG, TYA, STA, TXS, NTG, NTG, STA, NTG, NTG,
                                        LDY, LDA, LDX, NTG, LDY, LDA, LDX, NTG, TAY, LDA, TAX, NTG, LDY, LDA, LDX, NTG,
                                        Bxx, LDA, NTG, NTG, LDY, LDA, LDX, NTG, CLV, LDA, TSX, NTG, LDY, LDA, LDX, NTG,
                                        CPY, CMP, NTG, NTG, CPY, CMP, DEC, NTG, INY, CMP, DEX, NTG, CPY, CMP, DEC, NTG,
                                        Bxx, CMP, NTG, NTG, NTG, CMP, DEC, NTG, CLD, CMP, NTG, NTG, NTG, CMP, DEC, NTG,
                                        CPX, SBC, NTG, NTG, CPX, SBC, INC, NTG, INX, SBC, NTG, NTG, CPX, SBC, INC, NTG,
                                        Bxx, SBC, NTG, NTG, NTG, SBC, INC, NTG, SED, SBC, NTG, NTG, NTG, SBC, INC, NTG  };

//   x0    x1   x2   x3  x4   x5  x6    x7  x8    x9  xA   xB   xC   xD  xE  xF
static uint16_t (*pWhatAddress[256]) (processor *) = { IMP, INDX, NOA, NOA, NOA, ZP, ZP,  NOA, IMP, IMM, ACC, NOA, NOA, ABS, ABS, NOA,     // 0x
                                                NOA, INDY, NOA, NOA, NOA, ZPX, ZPX, NOA, IMP, ABSY, NOA, NOA, NOA, ABSX, ABSX, NOA, // 1x
                                                ABS, INDX, NOA, NOA, ZP, ZP, ZP, NOA, IMP, IMM, ACC, NOA, ABS, ABS, ABS, NOA,       // 2x
                                                NOA, INDY, NOA, NOA, NOA, ZPX, ZPX, NOA, IMP, ABSY, NOA, NOA, NOA, ABSX, ABSX, NOA, // 3x
                                                IMP, INDX, NOA, NOA, NOA, ZP, ZP, NOA, IMP, IMM, ACC, NOA, ABS, ABS, ABS, NOA,      // 4x
                                                NOA, INDY, NOA, NOA, NOA, ZPX, ZPX, NOA, IMP, ABSY, NOA, NOA, NOA, ABSX, ABSX, NOA, // 5x
                                                IMP, INDX, NOA, NOA, NOA, ZP, ZP, NOA, IMP, IMM, ACC, NOA, REL, ABS, ABS, NOA,      // 6x
                                                NOA, INDY, NOA, NOA, NOA, ZPX, ZPX, NOA, IMP, ABSY, NOA, NOA, NOA, ABSX, ABSX, NOA, // 7x
                                                NOA, INDX, NOA, NOA, ZP, ZP, ZP, NOA, IMP, NOA, IMP, NOA, ABS, ABS, ABS, NOA,       // 8x
                                                NOA, INDY, NOA, NOA, ZPX, ZPX, ZPY, NOA, IMP, ABSY, IMP, NOA, NOA, ABSX, ABSX, NOA, // 9x
                                                IMM, INDX, IMM, NOA, ZP, ZP, ZP, NOA, IMP, IMM, IMP, NOA, ABS, ABS, ABS, NOA,       // Ax
                                                NOA, INDY, NOA, NOA, ZPX, ZPX, ZPY, NOA, IMP, ABSY, IMP, NOA, ABSX, ABSX, ABSY, NOA, // Bx
                                                IMM, INDX, NOA, NOA, ZP, ZP, ZP, NOA, IMP, IMM, IMP, NOA, ABS, ABS, ABS, NOA,       // Cx
                                                NOA, INDY, NOA, NOA, NOA, ZPX, ZPX, NOA, IMP, ABSY, NOA, NOA, NOA, ABSX, ABSX, NOA, // Dx
                                                IMM, INDX, NOA, NOA, ZP, ZP, ZP, NOA, IMP, IMM, IMP, NOA, ABS, ABS, ABS, NOA,
                                                NOA, INDY, NOA, NOA, NOA, ZPX, ZPX, NOA, IMP, ABSY, NOA, NOA, NOA, ABSX, ABSX, NOA };

int main(){

        processor myProcessor;
        processor * Processor = &myProcessor;

        uint8_t Memory [65536];
        MEMADDR = Memory;

        Load (Memory);

        reset(Processor);

        PC = 0x0400;


        uint8_t cycles_to_count = 0;
        uint32_t cycles_for_interrupt = 0;
        while (1)
        {
                if (cycles_to_count == 0) {
                        uint16_t crr_PC = PC;
                        OPCODE = fetch_byte(Processor);
                        ADDR = (*pWhatAddress[OPCODE])(Processor);
                        (*pWhatMode[OPCODE])(Processor);
                        if (stop == 0)
                                printf("%04X %04X \n", OPCODE, crr_PC);
                        cycles_to_count=cycles[OPCODE] + Processor->add_cycles;
                        Processor->add_cycles = 0;
                }
                cycles_to_count--;
                cycles_for_interrupt++;
                if ((cycles_for_interrupt > INTERRUPT_PERIOD1) & (cycles_for_interrupt < INTERRUPT_PERIOD2))  {
                    //    cycles_for_interrupt = 0;
                        stop = 0;
                }
                else
                        stop = 1;

        }

        return 0;
}

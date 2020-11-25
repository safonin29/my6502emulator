#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


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
#define BF Processor->flags_now.brk
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


#define INTERRUPT_PERIOD 120

#define read_Byte(ADDRESS)  (Processor->memory_addr)[ADDRESS]
#define write_Byte(ADDRESS,DATA)  (Processor->memory_addr)[ADDRESS]=DATA



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


typedef struct {

        uint8_t negative;
        uint8_t overflow;
        uint8_t brk;
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


        uint8_t flag_acc_adress;

        uint8_t add_cycles;

        flags flags_now;

        uint8_t * memory_addr;

}processor;



uint8_t recount_status (processor *Processor){

        uint8_t status = (NF << 7) | (OF << 6)  | (BF << 4)| (DF << 3)  | (IF << 2) |  (ZF << 1) | CF;
        return (status);
}

void recount_flags(processor *Processor){

        NF = S >> 7;
        OF = (S >> 6) & 0x01;
        BF = (S >> 4) & 0x01;
        DF = (S >> 3) & 0x01;
        IF = (S >> 2) & 0x01;
        ZF = (S >> 1) & 0x01;
        CF = S & 0x01;

}


uint8_t reset (processor *Processor){

        uint8_t PCL = read_Byte(0xFFFC);
        uint8_t PHL = read_Byte(0xFFFD);

        PC = (PHL << 8) || PCL;

        IF = 1;

        Processor->flag_acc_adress = 0;

        Processor->add_cycles = 0;

        return 0;


}

uint8_t fetch_byte (processor *Processor){

        uint8_t data = read_Byte(PC);
        PC++;
        return data;
}

uint8_t push_stack (processor *Processor, uint8_t Byte){

        write_Byte(0x0100 + SP, Byte);
        SP--;
        return 1;
}

uint8_t pull_stack (processor *Processor){

        return (read_Byte(0x0100 + SP++));
}
//Adressing modes

uint8_t check_n_z (processor *Processor, uint8_t reg){

        NF = (reg >> 7) ? 1 : 0;
        ZF = (reg == 0) ? 1 : 0;
        return 0;

}


uint16_t INDX (processor *Processor){ // X-indexed, indirect

        PC = 0x0000 |  (fetch_byte(Processor) + X); // Fetch Bal, counter = BAL + X
        uint8_t ADL = fetch_byte(Processor);
        uint8_t AHL = fetch_byte(Processor);
        PC = (AHL << 8) |  ADL;
        uint16_t adr = PC++;
        return adr;

}
uint16_t IMP(processor *Processor) { // implied

        return PC;

}

uint16_t ABS(processor *Processor) { //absolute

        uint8_t adl = fetch_byte(Processor);
        uint8_t ahl = fetch_byte(Processor);
        uint16_t Adr = (ahl << 8 )| adl;
        return Adr;


}
uint16_t ZP (processor *Processor){  // zero_page

        uint8_t adl = fetch_byte(Processor);
        uint16_t adr = 0 | adl;
        return adr;


}

uint16_t ZPX (processor *Processor) {

        uint8_t adl = fetch_byte(Processor);
        uint16_t adr = (0 | adl) + X;
        return adr;


}


uint16_t ZPY (processor *Processor) {

        uint8_t adl = fetch_byte(Processor);
        uint16_t adr = (0 | adl) + Y;
        return adr;


}

uint16_t IMM (processor *Processor){ // immediate

        uint16_t adr = PC++;
        return adr;
}

uint16_t REL (processor *Processor){

        uint8_t number = fetch_byte(Processor);
        uint16_t adr = PC + number;
        if ( (uint8_t) PC + number > 255)
                Processor->add_cycles = 2;
        else
                Processor->add_cycles = 1;
        return (adr);

}

uint16_t ABSX (processor *Processor){ // absolute, X-indexed

        uint8_t bal = fetch_byte(Processor);
        uint8_t bah = fetch_byte(Processor);
        uint16_t adr = ((bah << 8 )| bal ) + X;
        if (bal + X > 255) {
                CF = 1;
                Processor->add_cycles+=1;
        }
        return adr;

}

uint16_t ABSY (processor *Processor){ // absolute, Y-indexed

        uint8_t bal = fetch_byte(Processor);
        uint8_t bah = fetch_byte(Processor);
        uint16_t adr = ((bah << 8 )| bal ) + Y;
        if (bal + Y > 255) {
                CF = 1;
                Processor->add_cycles+=1;
        }
        return adr;

}

uint16_t INDY (processor *Processor){   // indirect, Y-indexed

        uint16_t ial  = 0 + fetch_byte(Processor);
        uint8_t bal = read_Byte(ial);
        uint8_t bah = read_Byte(ial+1);
        uint16_t adr = (bah << 8 | bal) + Y;
        if (bal + X > 255) {
                CF = 1;
                Processor->add_cycles+=1;
        }
        return (adr);



}

uint16_t ACC (processor *Processor)
{
        Processor->flag_acc_adress = 1;
        return 0;
}


void BRK (processor *Processor){

        fetch_byte(Processor); // fetch opcode and discard
        push_stack (Processor, (PC >> 8)); // PCH
        push_stack (Processor, PC); // PCL
        BF = 1;
        S = recount_status(Processor);
        push_stack(Processor, S);
        //fetch pcl and pch

}

void LDA (processor *Processor){

        A = read_Byte(ADDR);
        check_n_z(Processor, A);
}
void CLC (processor *Processor){

        CF = 0;
}

void SED (processor *Processor){

        DF = 1;
}

void STA (processor *Processor){

        Processor->add_cycles = 0; // always penalty cycle
        write_Byte(ADDR, A);
}

void CLD (processor *Processor){

        DF = 0;
}

void SEC (processor *Processor){

        CF = 1;
}

void CLV (processor *Processor){

        OF = 0;
}

void ADC (processor *Processor){

        uint8_t data = read_Byte(ADDR);
        if (DF == 0) {

                uint16_t sum = A + data + CF;
                uint8_t sign_acc_bef= A >> 7;
                uint8_t sign_data = data >> 7;
                A = sum;
                uint8_t sign_acc_aft = data >> 7;
                OF= ((~sign_acc_bef&~sign_data&sign_acc_aft)|(sign_acc_bef&sign_data&~sign_acc_aft) == 1) ? 1 : 0; //check later
                CF = (sum > 255) ? 1 : 0;
        }
        else
        {
                uint8_t l_byte = (A & 0x0F) + (data & 0x0F) + CF;
                uint8_t carry_dec;
                if (l_byte > 9) {
                        l_byte = l_byte-10;
                        carry_dec = 1;
                }
                uint8_t h_byte = (A >> 4) + (data >> 4) + carry_dec;
                if (h_byte > 9) {
                        h_byte = h_byte-10;
                        CF = 1;
                }

                A = (h_byte << 4) | l_byte;
        }
        check_n_z(Processor, A);


}

void SBC (processor *Processor){
        uint8_t data = read_Byte(ADDR);
        if (DF == 0) {
                uint16_t sum = ~data + CF + A;
                uint8_t sign_acc_bef= A >> 7;
                uint8_t sign_data = ~data >> 7;
                CF = (sum > 255) ? 1 : 0;
                A = sum;
                uint8_t sign_acc_aft = A >> 7;
                OF= ((~sign_acc_bef&~sign_data&sign_acc_aft)|(sign_acc_bef&sign_data&~sign_acc_aft) == 1) ? 1 : 0; //check later
        }
        else {
                uint16_t l_byte = (A & 0x0F) + (data & 0x0F) + CF;
                uint8_t carry_dec;
                carry_dec = (l_byte > 255) ? 1 : 0;
                l_byte = ~(l_byte - 1);
                uint16_t h_byte = (A >> 4) + (data >> 4) + carry_dec;
                CF = (h_byte > 255) ? 1 : 0;
                h_byte = ~(h_byte - 1);
                A= (h_byte << 4) | l_byte; // check later
        }

        check_n_z(Processor, A);

}

void AND (processor *Processor){

        A = A & read_Byte(ADDR);
        check_n_z(Processor, A);
        return;

}

void ORA (processor *Processor){

        A = A | read_Byte(ADDR);
        check_n_z(Processor, A);
        return;

}

void EOR (processor *Processor){

        A = A ^ read_Byte(ADDR);
        check_n_z(Processor, A);

}

void SEI (processor *Processor){

        IF = 1;

}


void CLI (processor *Processor){

        IF = 0;

}

void JMP (processor *Processor){

        PC = read_Byte(ADDR);

}

void Bxx (processor *Processor){

        uint8_t label = read_Byte(ADDR);
        switch (OPCODE)
        {
        case BPL:
                if (NF  == 0 )
                        PC += label;
                break;
        case BMI:
                if (NF  == 1 )
                        PC += label;
                break;
        case BVC:
                if (OF == 0)
                        PC += label;
                break;
        case BVS:
                if (OF == 1)
                        PC += label;
                break;
        case BCC:
                if (CF == 0)
                        PC += label;
                break;
        case BCS:
                if (CF == 1)
                        PC += label;
                break;
        case BNE:
                if (ZF == 1)
                        PC += label;
                break;
        case BEQ:
                if (ZF == 0)
                        PC += label;
                break;
        }

}

void CMP (processor *Processor)
{
        uint8_t count = read_Byte(ADDR);
        ZF = (count == A) ? 1 : 0;
        CF = (count <= A) ? 1 : 0;
        NF = ((A + ~count + 1) >> 7 == 1 ) ? 1 : 0; // check later

}

void BIT (processor *Processor){
        uint8_t data = read_Byte(ADDR);
        uint8_t sum = A & data;
        ZF = (sum == 0) ? 1 : 0;
        NF = (sum >> 7 == 1) ? 1 : 0;
        OF = ((sum &0x40) >> 6 == 1 ) ? 1 : 0;
}

void LDX (processor * Processor){

        uint8_t data = read_Byte(ADDR);
        X = data;
        check_n_z(Processor, X);
        return;

}

void LDY (processor * Processor){

        uint8_t data = read_Byte(ADDR);
        Y = data;
        check_n_z(Processor, X);
        return;
}

void STX (processor * Processor){


        write_Byte(ADDR, X);

}

void STY (processor * Processor){


        write_Byte(ADDR, Y);

}

void INX (processor * Processor){

        X++;
        check_n_z(Processor, X);
        return;

}


void INY (processor * Processor){

        Y++;
        check_n_z(Processor, Y);
        return;

}

void DEX (processor * Processor){

        X--;
        check_n_z(Processor, X);
        return;

}


void DEY (processor * Processor){

        Y--;
        check_n_z(Processor, Y);
        return;

}

void CPX (processor *Processor){

        uint8_t count = read_Byte(ADDR);
        ZF = (count == X) ? 1 : 0;
        CF = (count <= X) ? 1 : 0;
        NF = ((X + ~count + 1) >> 7 == 1 ) ? 1 : 0; // check later

}
void CPY (processor *Processor){

        uint8_t count = read_Byte(ADDR);
        ZF = (count == X) ? 1 : 0;
        CF = (count <= X) ? 1 : 0;
        NF = ((Y + ~count + 1) >> 7 == 1 ) ? 1 : 0; // check later

}

void TAX (processor *Processor){


        X  =  A;
        check_n_z(Processor, Y);
        return;
}

void TXA (processor *Processor){


        A  =  X;
        check_n_z(Processor, A);
        return;
}

void TAY (processor *Processor){


        Y  =  A;
        check_n_z(Processor, Y);
        return;
}

void TYA (processor *Processor){


        A  =  Y;
        check_n_z(Processor, A);
        return;
}

void JSR (processor *Processor){

        uint16_t save_addr = PC-1;
        uint8_t PCH = save_addr >> 8;
        push_stack(Processor, PCH);
        uint8_t PCL = (uint8_t) save_addr;
        push_stack(Processor, PCL);
        PC = ADDR;
        return;

}


void PHP (processor *Processor){


        S = recount_status(Processor);
        push_stack(Processor, S);
        return;
}

void PLP (processor *Processor){


        S = pull_stack(Processor);
        return;
}

void RTS (processor *Processor) {


        uint8_t PCL = pull_stack(Processor);
        uint8_t PCH = pull_stack(Processor);
        PC = ((PCH << 8) | PCL ) + 1;  // check later;
        return;

}

void PHA (processor *Processor){

        push_stack(Processor, A);
        return;


}

void PLA (processor *Processor){

        A = pull_stack(Processor);
        check_n_z(Processor, A);
        return;


}

void TXS (processor *Processor){


        SP = X;
        return;

}

void TSX (processor *Processor){


        X = SP;
        check_n_z(Processor, X);
        return;

}

void RTI (processor *Processor){

        S = pull_stack(Processor);
        recount_flags(Processor);
        uint8_t PCL = pull_stack(Processor);
        uint8_t PCH = pull_stack(Processor);
        PC = (PCH << 8) | PCL;

}

void LSR (processor *Processor){

        if (Processor->flag_acc_adress == 1) {

                CF = A & 0x01;
                A  = A >> 1;
                check_n_z (Processor, A);
        }
        else {

                uint8_t data = read_Byte(ADDR);

                CF = data & 0x01;
                data = data >> 1;
                check_n_z (Processor, data);
                write_Byte(ADDR, data);

        }


}


void ASL (processor *Processor){

        if (Processor->flag_acc_adress == 1) {

                CF = A >> 7;
                A  = A << 1;
                check_n_z (Processor, A);
        }
        else {

                uint8_t data = read_Byte(ADDR);

                CF = data >> 7;
                data = data << 1;
                check_n_z (Processor, data);
                write_Byte(ADDR, data);

        }

}


void ROL (processor *Processor){



        if (Processor->flag_acc_adress == 1) {

                CF = A >> 7;
                A  = (A << 1) | CF;
                check_n_z (Processor, A);
        }
        else {

                uint8_t data = read_Byte(ADDR);

                CF = data >> 7;
                data = (data << 1) | CF;
                check_n_z (Processor, data);

                write_Byte(ADDR, data);

        }

}



void ROR  (processor *Processor){



        if (Processor->flag_acc_adress == 1) {

                CF = A & 0x01;
                A  = (A >> 1) | (CF << 7);
                check_n_z (Processor, A);
        }
        else {

                uint8_t data = read_Byte(ADDR);

                CF = data & 0x01;
                data = (data >> 1) | (CF << 7);
                check_n_z (Processor, data);

                write_Byte(ADDR, data);

        }

}


void INC  (processor *Processor){

        uint8_t data = read_Byte(ADDR);
        data++;
        check_n_z(Processor, data);
        write_Byte(ADDR, data);

}

void DEC  (processor *Processor){

        uint8_t data = read_Byte(ADDR);
        data--;
        check_n_z(Processor, data);
        write_Byte(ADDR, data);

}


void NTG (processor *Processor){ // no opcode
        return;


}

uint16_t NOA (processor *Processor){ // no adressing
        return 0;


}




void (*pWhatMode[256]) (processor *) = {BRK, ORA, NTG, NTG, NTG, ORA, ASL, NTG, PHP, ORA, ASL, NTG, NTG, ORA, ASL, NTG,
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

uint16_t (*pWhatAddress[256]) (processor *) = { IMP, INDX, NOA, NOA, NOA, ZP, ZP,  NOA, IMP, IMM, ACC, NOA, NOA, ABS, ABS, NOA,
                                                REL, INDY, NOA, NOA, NOA, ZPX, ZPX, NOA, IMP, ABSY, NOA, NOA, NOA, ABSX, ABSX, NOA,
                                                ABS, INDX, NOA, NOA, ZP, ZP, ZP, NOA, IMP, IMM, ACC, NOA, ABS, ABS, ABS, NOA,
                                                REL, INDY, NOA, NOA, NOA, ZPX, ZPX, NOA, IMP, ABSY, NOA, NOA, NOA, ABSX, ABSX, NOA,
                                                IMP, INDX, NOA, NOA, NOA, ZP, ZP, NOA, IMP, IMM, ACC, NOA, ABS, ABS, ABS, NOA,
                                                REL, INDY, NOA, NOA, NOA, ZPX, ZPX, NOA, IMP, ABSY, NOA, NOA, NOA, ABSX, ABSX, NOA,
                                                IMP, INDX, NOA, NOA, NOA, ZP, ZP, NOA, IMP, IMM, ACC, NOA, NOA, ABS, ABS, NOA,
                                                REL, INDY, NOA, NOA, NOA, ZPX, ZPX, NOA, IMP, ABSY, NOA, NOA, NOA, ABSX, ABSX, NOA,
                                                NOA, INDX, NOA, NOA, ZP, ZP, ZP, NOA, IMP, NOA, IMP, NOA, ABS, ABS, ABS, NOA,
                                                REL, INDY, NOA, NOA, ZPX, ZPX, ZPY, NOA, IMP, ABSY, IMP, NOA, NOA, ABSX, ABSX, NOA,
                                                IMM, INDX, IMM, NOA, ZP, ZP, ZP, NOA, IMP, IMM, IMP, NOA, ABS, ABS, ABS, NOA,
                                                REL, INDY, NOA, NOA, ZPX, ZPX, ZPY, NOA, IMP, ABSY, IMP, NOA, ABSX, ABSX, ABSY, NOA,
                                                IMM, INDX, NOA, NOA, ZP, ZP, ZP, NOA, IMP, IMM, IMP, NOA, ABS, ABS, ABS, NOA,
                                                REL, INDY, NOA, NOA, NOA, ZPX, ZPX, NOA, IMP, ABSY, NOA, NOA, NOA, ABSX, ABSX, NOA,
                                                IMM, INDX, NOA, NOA, ZP, ZP, ZP, NOA, IMP, IMM, IMP, NOA, ABS, ABS, ABS, NOA,
                                                REL, INDY, NOA, NOA, NOA, ZPX, ZPX, NOA, IMP, ABSY, NOA, NOA, NOA, ABSX, ABSX, NOA };

int main(){

        processor myProcessor;
        processor * Processor = &myProcessor;

        uint8_t Memory [0xFFFF];
        MEMADDR = Memory;

        reset(Processor);

        uint8_t cycles_to_count = 0;
        uint8_t cycles_for_interrupt = 0;
        while (1)
        {
                if (cycles_to_count == 0) {
                        OPCODE = fetch_byte(Processor);
                        ADDR = (*pWhatAddress[OPCODE])(Processor);
                        (*pWhatMode[OPCODE])(Processor);
                        cycles_to_count=cycles[OPCODE] + Processor->add_cycles;
                        Processor->add_cycles = 0;
                }
                cycles_to_count--;
                cycles_for_interrupt++;
                if (cycles_for_interrupt == INTERRUPT_PERIOD)
                        cycles_for_interrupt = 0;
        }

        return 0;
}

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

#define BPL 0x10
#define BMI 0x30
#define BVC 0x50
#define BVS 0x70
#define BCC 0x90
#define BCS 0xB0
#define BNE 0xD0
#define BEQ 0xF0



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
        uint16_t memory;
        uint16_t address;       //


        uint16_t counter;
        uint8_t accumulator;
        uint8_t x_register;
        uint8_t y_register;
        uint8_t status;
        uint8_t pStack;

        flags flags_now;

}processor;

uint8_t recount_status (processor *Processor){

        uint8_t status = (NF << 7) | (OF << 6)  | (BF << 4)| (DF << 3)  | (IF << 2) |  (ZF << 1) | CF;
        return (status);
}
uint8_t read_Byte (uint16_t Address){

        return 1;
}


uint8_t write_Byte (uint16_t Address, uint8_t Byte){

        return 1;
}

uint8_t fetch_byte (processor *Processor){

        read_Byte(PC);
        PC++;
        return 1;
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
        return;

}


uint16_t index_indirect (processor *Processor){

        PC = 0x0000 |  (fetch_byte(Processor) + X); // Fetch Bal, counter = BAL + X
        uint8_t ADL = fetch_byte(Processor);
        uint8_t AHL = fetch_byte(Processor);
        PC = (AHL << 8) |  ADL;
        uint16_t adr = PC++;
        return adr;

}
uint16_t implied(processor *Processor) {

        return PC;

}

uint16_t absolute (processor *Processor) {

        uint8_t adl = fetch_byte(Processor);
        uint8_t ahl = fetch_byte(Processor);
        uint16_t Adr = (ahl << 8 )| adl;
        return Adr;


}
uint16_t zero_page (processor *Processor){

        uint8_t adl = fetch_byte(Processor);
        uint16_t adr = 0 | adl;
        return adr;


}

uint16_t immediate (processor *Processor){

        uint16_t adr = PC++;
        return adr;
}

uint16_t abs_index_x (processor *Processor){

        uint8_t bal = fetch_byte(Processor);
        uint8_t bah = fetch_byte(Processor);
        uint16_t adr = ((bah << 8 )| bal ) + X;
        return adr;

}

uint16_t abs_index_y (processor *Processor){

        uint8_t bal = fetch_byte(Processor);
        uint8_t bah = fetch_byte(Processor);
        uint16_t adr = ((bah << 8 )| bal ) + Y;
        return adr;

}

uint16_t indirect_index (processor *Processor){

        uint16_t ial  = 0 + fetch_byte(Processor);
        uint8_t bal = read_Byte(ial);
        uint8_t bah = read_Byte(ial+1);
        uint16_t adr = (bah << 8 | bal) + Y;
        return (adr);



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




void (*pWhatMode[256]) (processor *) = {BRK };

int main(){

        flags crr_flags;
        flags *p_crr_flags = &crr_flags;
        p_crr_flags->negative = 1;
        //      printf("%d", OF);
        return 0;
}

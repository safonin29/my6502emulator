
 #include "variables.h"


void BRK (processor *Processor){

        fetch_byte(Processor); // fetch opcode and discard
        push_stack (Processor, (PC >> 8)); // PCH
        push_stack (Processor, PC); // PCL
        BF1 = 1;
        BF2 = 1;
        S = recount_status(Processor);
        push_stack(Processor, S);
        uint8_t bal = read_Byte(0xFFFE);
        uint8_t bah = read_Byte(0xFFFF);
        PC = (bah << 8) | bal;
        IF = 1;
        BF1 = 0;
        BF2 = 0;
        return;


}

void LDA (processor *Processor){

        A = read_Byte(ADDR);
        check_n_z(Processor, A);
        return;
}
void CLC (processor *Processor){

        CF = 0;
        return;
}

void SED (processor *Processor){

        DF = 1;
        return;
}

void STA (processor *Processor){

        Processor->add_cycles = 0; // always penalty cycle
        write_Byte(ADDR, A);
        return;
}

void CLD (processor *Processor){

        DF = 0;
        return;
}

void SEC (processor *Processor){

        CF = 1;
        return;
}

void CLV (processor *Processor){

        OF = 0;
        return;
}

void ADC (processor *Processor){

        uint8_t acc = A;
        uint8_t data = read_Byte(ADDR);
        if (DF == 0) {

                uint16_t sum = acc + data + CF;
                uint8_t sign_acc_bef= acc >> 7;
                A = acc = sum;
                if ((sign_acc_bef ^  (data >> 7))  == 0) {
                        uint8_t sign_acc_aft = acc >> 7;
                        OF = (sign_acc_aft != sign_acc_bef )  ? 1 : 0;
                }

                else
                        OF = 0;
                CF = (sum >>  8) ? 1 : 0;
        }
        else
        {
                uint8_t l_byte = (acc & 0x0F) + (data & 0x0F) + CF;
                uint8_t carry_dec = 0;
                if (l_byte > 9) {
                        l_byte -= 10;
                        carry_dec = 1;
                }
                uint8_t h_byte = (acc >> 4) + (data >> 4) + carry_dec;
                if (h_byte > 9) {
                        h_byte -= 10;
                        CF = 1;
                }
                else
                        CF = 0;

                A = (h_byte << 4) | l_byte;



        }
        check_n_z(Processor, A);
        return;


}

void SBC (processor *Processor){

        uint8_t acc = A;
        uint8_t data = read_Byte(ADDR);
        if (DF == 0) {
                uint16_t sum = (uint8_t)(~data)  + CF + acc;
                uint8_t sign_acc_bef= acc >> 7;
                uint8_t sign_data = 0x01 & ((~data) >> 7);
                CF = (sum >> 8) ? 1 : 0;
                A = sum;
                if ((sign_acc_bef ^  sign_data)  == 0){
                        uint8_t sign_acc_aft = 0x01 & (sum >> 7);
                        OF = (sign_acc_aft != sign_acc_bef )  ? 1 : 0;
                }
                else
                        OF = 0;
        }
        else {
                uint8_t l_byte = (acc & 0x0F) + (~(data & 0x0F) + CF);
                uint8_t carry_dec;
                carry_dec = (l_byte >> 7) ? 0 : 1;
                if (carry_dec == 0)
                        l_byte = 0x0A + l_byte;
                uint8_t h_byte = (acc >> 4) + ~((data >> 4) & 0x0F) + carry_dec;
                CF = (h_byte >> 7) ? 0 : 1;
                if (CF == 0)
                        h_byte = 0x0A + h_byte;
                A= (h_byte << 4) | l_byte; // check later
        }

        check_n_z(Processor, A);
        return;

}

void AND (processor *Processor){

        uint8_t data = read_Byte(ADDR);
        A &= data;
        check_n_z(Processor, A);
        return;

}

void ORA (processor *Processor){

        A |= read_Byte(ADDR);
        check_n_z(Processor, A);
        return;

}

void EOR (processor *Processor){

        A ^= read_Byte(ADDR);
        check_n_z(Processor, A);
        return;

}

void SEI (processor *Processor){

        IF = 1;
        return;

}


void CLI (processor *Processor){

        IF = 0;
        return;

}

void JMP (processor *Processor){

        PC = ADDR;
        return;

}

void Bxx (processor *Processor){

        uint8_t newlabel = fetch_byte(Processor);
        uint16_t label;
        if (newlabel >> 7)
                label = 0xFF00 | newlabel;
        else
                label = newlabel;
        switch (OPCODE)
        {
        case BPL:
                if (NF  == 1 )
                        return;
                break;
        case BMI:
                if (NF  == 0 )
                        return;
                break;
        case BVC:
                if (OF == 1)
                        return;
                break;
        case BVS:
                if (OF == 0)
                        return;
                break;
        case BCC:
                if (CF == 1)
                        return;
                break;
        case BCS:
                if (CF == 0)
                        return;
                break;
        case BNE:
                if (ZF == 1)
                        return;
                break;
        case BEQ:
                if (ZF == 0)
                        return;
                break;

        }
                PC += label;
                Processor->add_cycles = ( ((uint8_t) PC + label) >> 8) ? 2 : 1;
        return;
}

void CMP (processor *Processor)
{
        uint8_t acc = A;
        uint8_t count = read_Byte(ADDR);
        ZF = (count == acc) ? 1 : 0;
        CF = (count <= acc) ? 1 : 0;
        NF = ((acc + (uint8_t)(~count) + 1) >> 7 == 1 ) ? 1 : 0;
        return;
}

void BIT (processor *Processor){
        uint8_t data = read_Byte(ADDR);
        uint8_t sum = A & data;
        ZF = (sum == 0) ? 1 : 0;
        NF = (data>> 7) ? 1 : 0;
        OF = ((data &0x40) >> 6 ) ? 1 : 0;
        return;
}

void LDX (processor * Processor){

        X = read_Byte(ADDR);
        check_n_z(Processor, X);
        return;

}

void LDY (processor * Processor){

        Y = read_Byte(ADDR);
        check_n_z(Processor, Y);
        return;
}

void STX (processor * Processor){


        write_Byte(ADDR, X);

}

void STY (processor * Processor){


        write_Byte(ADDR, Y);

}

void INX (processor * Processor){

        check_n_z(Processor, ++X);
        return;

}


void INY (processor * Processor){

        check_n_z(Processor, ++Y);
        return;

}

void DEX (processor * Processor){

        check_n_z(Processor, --X);
        return;
}


void DEY (processor * Processor){

        check_n_z(Processor, --Y);
        return;
}

void CPX (processor *Processor){

        uint8_t x_register = X;
        uint8_t count = read_Byte(ADDR);
        ZF = (count == x_register) ? 1 : 0;
        CF = (count <= x_register) ? 1 : 0;
        NF = ((x_register + (uint8_t)(~count) + 1) >> 7 == 1 ) ? 1 : 0;
        return;

}
void CPY (processor *Processor){

        uint8_t y_register = Y;
        uint8_t count = read_Byte(ADDR);
        ZF = (count == y_register) ? 1 : 0;
        CF = (count <= y_register) ? 1 : 0;
        NF = ((y_register + (uint8_t)(~count) + 1) >> 7 == 1 ) ? 1 : 0;
        return;

}

void TAX (processor *Processor){


        X  =  A;
        check_n_z(Processor, X);
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
        push_stack(Processor, save_addr >> 8);
        push_stack(Processor, (uint8_t) save_addr);
        PC = ADDR;
        return;
}


void PHP (processor *Processor){

        BF1 = 1;
        BF2 = 1;
        S = recount_status(Processor);
        push_stack(Processor, S);
        BF1 = 0;
        BF2 = 0;
        S = recount_status(Processor);
        return;
}

void PLP (processor *Processor){


        S = pull_stack(Processor);
        recount_flags(Processor);
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
        return;
}

void LSR (processor *Processor){

        if (Processor->flag_acc_address == 1) {

                uint8_t acc = A;
                CF = acc & 0x01;
                A  = 0x7F & (acc >> 1);
                check_n_z (Processor, A);
                Processor->flag_acc_address = 0;
        }
        else {

                uint8_t data = read_Byte(ADDR);
                CF = data & 0x01;
                data = data >> 1;
                check_n_z (Processor, data);
                write_Byte(ADDR, data);

        }
        return;
}


void ASL (processor *Processor){

        if (Processor->flag_acc_address == 1) {

                uint8_t acc = A;
                CF = 0x01 & (acc >> 7);
                A  = acc << 1;
                check_n_z (Processor, A);
                Processor->flag_acc_address = 0;
        }
        else {

                uint8_t data = read_Byte(ADDR);
                CF = 0x01 & (data >> 7);
                data = data << 1;
                check_n_z (Processor, data);
                write_Byte(ADDR, data);

        }
        return;
}


void ROL (processor *Processor){



        if (Processor->flag_acc_address == 1) {

                uint8_t acc = A;
                uint8_t temp_cf = 0x01 & (acc >> 7);
                A  = (acc << 1) | CF;
                CF = temp_cf;
                check_n_z (Processor, A);
                Processor->flag_acc_address = 0;
        }
        else {

                uint8_t data = read_Byte(ADDR);

                uint8_t temp_cf = 0x01 & (data >> 7);
                data = (data << 1) | CF;
                CF = temp_cf;
                check_n_z (Processor, data);
                write_Byte(ADDR, data);

        }
        return;
}



void ROR  (processor *Processor){

        if (Processor->flag_acc_address == 1) {

                uint8_t acc = A;
                uint8_t temp_cf = acc & 0x01;
                A  = (acc >> 1) | (CF << 7);
                CF = temp_cf;
                check_n_z (Processor, A);
                Processor->flag_acc_address = 0;
        }
        else {

                uint8_t data = read_Byte(ADDR);
                uint8_t temp_cf = data & 0x01;
                data  = (data >> 1) | (CF << 7);
                CF = temp_cf;
                check_n_z (Processor, data);
                write_Byte(ADDR, data);

        }
        return;
}


void INC  (processor *Processor){

        uint16_t address = ADDR;
        uint8_t data = read_Byte(address);
        data++;
        check_n_z(Processor, data);
        write_Byte(address, data);
        return;
}

void DEC  (processor *Processor){

        uint16_t address = ADDR;
        uint8_t data = read_Byte(address);
        data--;
        check_n_z(Processor, data);
        write_Byte(address, data);
        return;
}


void NTG (processor *Processor){ // no opcode
        return;
}

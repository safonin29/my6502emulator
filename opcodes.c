
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
                uint8_t sign_acc_aft = sum >> 7;
                OF= (((inv_bit(sign_acc_bef)&inv_bit(sign_data)&sign_acc_aft)|(sign_acc_bef&sign_data&inv_bit(sign_acc_aft))) == 1) ? 1 : 0; //check later
                CF = (sum > 255) ? 1 : 0;
        }
        else
        {
                uint8_t l_byte = (A & 0x0F) + (data & 0x0F) + CF;
                uint8_t carry_dec = 0;
                if (l_byte > 9) {
                        l_byte -= 10;
                        carry_dec = 1;
                }
                uint8_t h_byte = (A >> 4) + (data >> 4) + carry_dec;
                if (h_byte > 9) {
                        h_byte -= 10;
                        CF = 1;
                }
                else
                        CF = 0;

                A = (h_byte << 4) | l_byte;



        }
        check_n_z(Processor, A);


}

void SBC (processor *Processor){

        uint8_t data = read_Byte(ADDR);
        if (DF == 0) {
                uint16_t sum = (uint8_t)(~data)  + CF + A;
                uint8_t sign_acc_bef= A >> 7;
                uint8_t sign_data = 0x01 & ((uint8_t)(~data) >> 7);
                CF = (sum > 255) ? 1 : 0;
                A = sum;
                uint8_t sign_acc_aft = 0x01 & (sum >> 7);
                OF= (((inv_bit(sign_acc_bef)&inv_bit(sign_data)&sign_acc_aft)|(sign_acc_bef&sign_data&inv_bit(sign_acc_aft))) == 1) ? 1 : 0; //check later
        }
        else {
                uint8_t l_byte = (A & 0x0F) + (~(data & 0x0F) + CF);
                uint8_t carry_dec;
                carry_dec = ((l_byte >> 7) == 1) ? 0 : 1;
                if (carry_dec == 0)
                    l_byte = 0x0A + l_byte;
                uint8_t h_byte = (A >> 4) + ~((data >> 4) & 0x0F) + carry_dec;
                CF = ((h_byte >> 7) == 1) ? 0 : 1;
                if (CF == 0)
                    h_byte = 0x0A + h_byte;
                A= (h_byte << 4) | l_byte; // check later
        }

        check_n_z(Processor, A);

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

}

void SEI (processor *Processor){

        IF = 1;

}


void CLI (processor *Processor){

        IF = 0;

}

void JMP (processor *Processor){

        PC = ADDR;

}

void Bxx (processor *Processor){

        uint8_t newlabel = fetch_byte(Processor);
        uint16_t label;
        if ((newlabel >> 7) == 1)
                label = 0xFF00 | newlabel;
        else
                label = newlabel;
        uint8_t flag_branch = 0;
        switch (OPCODE)
        {
        case BPL:
                if (NF  == 0 ) {
                        PC += label;
                        flag_branch = 1;
                }
                break;
        case BMI:
                if (NF  == 1 ) {
                        PC += label;
                        flag_branch = 1;
                }
                break;
        case BVC:
                if (OF == 0) {
                        PC += label;
                        flag_branch = 1;
                }
                break;
        case BVS:
                if (OF == 1) {
                        PC += label;
                        flag_branch = 1;
                }
                break;
        case BCC:
                if (CF == 0) {
                        PC += label;
                        flag_branch = 1;
                }
                break;
        case BCS:
                if (CF == 1) {
                        PC += label;
                        flag_branch = 1;
                }
                break;
        case BNE:
                if (ZF == 0) {
                        PC += label;
                        flag_branch = 1;
                }
                break;
        case BEQ:
                if (ZF == 1) {
                        PC += label;
                        flag_branch = 1;
                }
                break;
        }

        if (flag_branch == 1) {
                if ( (uint8_t) PC + label> 255)
                        Processor->add_cycles = 2;
                else
                        Processor->add_cycles = 1;

        }

}

void CMP (processor *Processor)
{

        uint8_t count = read_Byte(ADDR);
        ZF = (count == A) ? 1 : 0;
        CF = (count <= A) ? 1 : 0;
        NF = ((A + (uint8_t)(~count) + 1) >> 7 == 1 ) ? 1 : 0;

}

void BIT (processor *Processor){
        uint8_t data = read_Byte(ADDR);
        uint8_t sum = A & data;
        ZF = (sum == 0) ? 1 : 0;
        NF = (data>> 7 == 1) ? 1 : 0;
        OF = ((data &0x40) >> 6 == 1 ) ? 1 : 0;
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

        uint8_t count = read_Byte(ADDR);
        ZF = (count == X) ? 1 : 0;
        CF = (count <= X) ? 1 : 0;
        NF = ((X + (uint8_t)(~count) + 1) >> 7 == 1 ) ? 1 : 0; // check later

}
void CPY (processor *Processor){

        uint8_t count = read_Byte(ADDR);
        ZF = (count == Y) ? 1 : 0;
        CF = (count <= Y) ? 1 : 0;
        NF = ((Y + (uint8_t)(~count) + 1) >> 7 == 1 ) ? 1 : 0; // check later

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

}

void LSR (processor *Processor){

        if (Processor->flag_acc_adress == 1) {

                CF = A & 0x01;
                A  = 0x7F & (A >> 1);
                check_n_z (Processor, A);
                Processor->flag_acc_adress = 0;
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

                CF = 0x01 & (A >> 7);
                A  = A << 1;
                check_n_z (Processor, A);
                Processor->flag_acc_adress = 0;
        }
        else {

                uint8_t data = read_Byte(ADDR);

                CF = 0x01 & (data >> 7);
                data = data << 1;
                check_n_z (Processor, data);
                write_Byte(ADDR, data);

        }

}


void ROL (processor *Processor){



        if (Processor->flag_acc_adress == 1) {

                uint8_t temp_cf = 0x01 & (A >> 7);
                A  = (A << 1) | CF;
                CF = temp_cf;
                check_n_z (Processor, A);
                Processor->flag_acc_adress = 0;
        }
        else {

                uint8_t data = read_Byte(ADDR);

                uint8_t temp_cf = 0x01 & (data >> 7);
                data = (data << 1) | CF;
                CF = temp_cf;
                check_n_z (Processor, data);

                write_Byte(ADDR, data);

        }

}



void ROR  (processor *Processor){



        if (Processor->flag_acc_adress == 1) {

                uint8_t temp_cf = A & 0x01;
                A  = (A >> 1) | (CF << 7);
                CF = temp_cf;
                check_n_z (Processor, A);
                Processor->flag_acc_adress = 0;
        }
        else {

                uint8_t data = read_Byte(ADDR);

                uint8_t temp_cf = data & 0x01;
                data  = (data >> 1) | (CF << 7);
                CF = temp_cf;
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

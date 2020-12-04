#ifndef OPCODES_H
#define OPCODES_H


void BRK (processor *Processor);
void LDA (processor *Processor);
void CLC (processor *Processor);
void SED (processor *Processor);
void STA (processor *Processor);
void CLD (processor *Processor);
void SEC (processor *Processor);
void CLV (processor *Processor);
void ADC (processor *Processor);
void SBC (processor *Processor);
void AND (processor *Processor);
void ORA (processor *Processor);
void EOR (processor *Processor);
void SEI (processor *Processor);
void CLI (processor *Processor);
void JMP (processor *Processor);
void Bxx (processor *Processor);
void CMP (processor *Processor);
void BIT (processor *Processor);
void LDX (processor *Processor);
void LDY (processor *Processor);
void STX (processor *Processor);
void STY (processor *Processor);
void INX (processor *Processor);
void INY (processor *Processor);
void DEX (processor *Processor);
void DEY (processor *Processor);
void CPX (processor *Processor);
void CPY (processor *Processor);
void TAX (processor *Processor);
void TXA (processor *Processor);
void TAY (processor *Processor);
void TYA (processor *Processor);
void JSR (processor *Processor);
void PHP (processor *Processor);
void PLP (processor *Processor);
void RTS (processor *Processor);
void PHA (processor *Processor);
void PLA (processor *Processor);
void TXS (processor *Processor);
void TSX (processor *Processor);
void RTI (processor *Processor);
void LSR (processor *Processor);
void ASL (processor *Processor);
void ROL (processor *Processor);
void ROR (processor *Processor);
void INC (processor *Processor);
void DEC (processor *Processor);
void NTG (processor *Processor);
uint16_t NOA (processor *Processor);
#endif




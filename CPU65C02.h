#ifndef CPU65C02_H
#define CPU65C02_H

#include <cstdint>
#include <iostream>

class CPU65C02 {
private:
    uint8_t A, X, Y, S, P; // 8-bit registers // S is the stack pointer register
    uint16_t PC; // 16-bit address counter
    uint8_t status; // 8-bit status register
    uint8_t RAM[65536]; // 64KB of RAM, 16 bits address
    uint32_t cycles; // Cycle counter
    bool debug; // Debug flag
    typedef void (CPU65C02::*OpCodeFn)();
    OpCodeFn opcode_table[256];

    uint8_t fetch_byte();
    uint8_t fetch_byte(uint16_t addr);
    uint16_t fetch_word();
    void debug_print(const char* message);
    void update_flags(uint8_t value);
    void update_NZ_flags(uint8_t value);
    void input();
    void print_registers();
    void push(uint8_t value);
    
    // Getters
    uint8_t pull();
    uint8_t get_P() { return P; }
    uint8_t get_X() { return X; }
    uint8_t get_SP() { return S; }
    uint8_t get_A() { return A; }
    uint8_t get_Y() { return Y; }
    uint16_t get_PC() { return PC; }
    uint8_t get_status() { return status; }
    uint8_t get_RAM(uint16_t addr) { return RAM[addr]; }
    uint32_t get_cycles() { return cycles; }

public:
    CPU65C02();
    void reset();
    void load_program(uint8_t* program, size_t size);
    void execute();

    // LDA instructions
    void LDA_ZP();
    void LDA_ZP_X();
    void LDA_IMM();
    void LDA_ABS();
    void LDA_ABS_Y();
    void LDA_ABS_X();
    void LDA_PRE_IND_X();
    void LDA_POST_IND_Y();
    void LDA_IND();

    // LDX instructions
    void LDX_IMM();
    void LDX_ZP();
    void LDX_ZP_Y();
    void LDX_ABS();
    void LDX_ABS_Y();

    // LDY instructions
    void LDY_IMM();
    void LDY_ZP();
    void LDY_ZP_X();
    void LDY_ABS();
    void LDY_ABS_X();

    // STA instructions
    void STA_ZP();
    void STA_ZP_X();
    void STA_ABS();
    void STA_ABS_X();
    void STA_ABS_Y();
    void STA_PRE_IND_X();
    void STA_POST_IND_Y();
    void STA_IND();

    // STX instructions
    void STX_ZP();
    void STX_ZP_Y();
    void STX_ABS();

    // STY instructions
    void STY_ZP();
    void STY_ZP_X();
    void STY_ABS();

    // Arithmetic Operations
    void ADC_IMM();
    void ADC_ZP();
    void ADC_ZP_X();
    void ADC_ABS();
    void ADC_ABS_X();
    void ADC_ABS_Y();
    void ADC_PRE_IND_X();
    void ADC_POST_IND_Y();
    void ADC_IND();

    void SBC_IMM();
    void SBC_ZP();
    void SBC_ZP_X();
    void SBC_ABS();
    void SBC_ABS_X();
    void SBC_ABS_Y();
    void SBC_PRE_IND_X();
    void SBC_POST_IND_Y();
    void SBC_IND();

    void INC_ZP();
    void INC_ZP_X();
    void INC_ABS();
    void INC_ABS_X();

    void INX();
    void INY();

    void DEC_ZP();
    void DEC_ZP_X();
    void DEC_ABS();
    void DEC_ABS_X();

    void DEX();
    void DEY();

    // Logical Operations
    void AND_IMM();
    void AND_ZP();
    void AND_ZP_X();
    void AND_ABS();
    void AND_ABS_X();
    void AND_ABS_Y();
    void AND_PRE_IND_X();
    void AND_POST_IND_Y();
    void AND_IND();

    void ORA_IMM();
    void ORA_ZP();
    void ORA_ZP_X();
    void ORA_ABS();
    void ORA_ABS_X();
    void ORA_ABS_Y();
    void ORA_PRE_IND_X();
    void ORA_POST_IND_Y();
    void ORA_IND();

    void EOR_IMM();
    void EOR_ZP();
    void EOR_ZP_X();
    void EOR_ABS();
    void EOR_ABS_X();
    void EOR_ABS_Y();
    void EOR_PRE_IND_X();
    void EOR_POST_IND_Y();
    void EOR_IND();

    void ASL_ACC();
    void ASL_ZP();
    void ASL_ZP_X();
    void ASL_ABS();
    void ASL_ABS_X();

    void LSR_ACC();
    void LSR_ZP();
    void LSR_ZP_X();
    void LSR_ABS();
    void LSR_ABS_X();

    void ROL_ACC();
    void ROL_ZP();
    void ROL_ZP_X();
    void ROL_ABS();
    void ROL_ABS_X();

    void ROR_ACC();
    void ROR_ZP();
    void ROR_ZP_X();
    void ROR_ABS();
    void ROR_ABS_X();

    // Other instructions
    void JMP();
    void BRK();
    void NOP();
    void RTI();

    // Stack Operations
    void PHA();  // Push Accumulator
    void PHP();  // Push Processor Status
    void PLA();  // Pull Accumulator
    void PLP();  // Pull Processor Status
    void TSX();  // Transfer Stack Pointer to X
    void TXS();  // Transfer X to Stack Pointer

    // Branch Instructions
    void BCC();  // Branch if Carry Clear
    void BCS();  // Branch if Carry Set
    void BEQ();  // Branch if Equal
    void BNE();  // Branch if Not Equal
    void BMI();  // Branch if Minus
    void BPL();  // Branch if Plus
    void BVC();  // Branch if Overflow Clear
    void BVS();  // Branch if Overflow Set

    // Status Flag Operations
    void CLC();  // Clear Carry Flag
    void SEC();  // Set Carry Flag
    void CLD();  // Clear Decimal Mode
    void SED();  // Set Decimal Mode
    void CLI();  // Clear Interrupt Disable
    void SEI();  // Set Interrupt Disable
    void CLV();  // Clear Overflow Flag

    // Comparison Operations
    void CMP_IMM();  // Compare with Accumulator (Immediate)
    void CMP_ZP();   // Compare with Accumulator (Zero Page)
    void CMP_ZP_X(); // Compare with Accumulator (Zero Page, X)
    void CMP_ABS();  // Compare with Accumulator (Absolute)
    void CMP_ABS_X(); // Compare with Accumulator (Absolute, X)
    void CMP_ABS_Y(); // Compare with Accumulator (Absolute, Y)
    void CMP_PRE_IND_X(); // Compare with Accumulator (Indirect, X)
    void CMP_POST_IND_Y(); // Compare with Accumulator (Indirect, Y)
    void CMP_IND();  // Compare with Accumulator (Indirect)

    void CPX_IMM();  // Compare with X Register (Immediate)
    void CPX_ZP();   // Compare with X Register (Zero Page)
    void CPX_ABS();  // Compare with X Register (Absolute)

    void CPY_IMM();  // Compare with Y Register (Immediate)
    void CPY_ZP();   // Compare with Y Register (Zero Page)
    void CPY_ABS();  // Compare with Y Register (Absolute)

    // Additional 65C02-specific instructions
    void BRA();      // Branch Always
    void PHX();      // Push X Register
    void PHY();      // Push Y Register
    void PLX();      // Pull X Register
    void PLY();      // Pull Y Register
    void STZ_ZP();   // Store Zero (Zero Page)
    void STZ_ZP_X(); // Store Zero (Zero Page, X)
    void STZ_ABS();  // Store Zero (Absolute)
    void STZ_ABS_X(); // Store Zero (Absolute, X)
    void TRB_ZP();   // Test and Reset Bits (Zero Page)
    void TRB_ABS();  // Test and Reset Bits (Absolute)
    void TSB_ZP();   // Test and Set Bits (Zero Page)
    void TSB_ABS();  // Test and Set Bits (Absolute)
};

#endif // CPU65C02_H 
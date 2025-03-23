#include "CPU65C02.h"
#include <algorithm>
#include <cstdio>
#include <iomanip>
#include <limits>

using namespace std;


uint8_t CPU65C02::fetch_byte() {
    return RAM[PC++];
}

uint8_t CPU65C02::fetch_byte(uint16_t addr) {
    return RAM[addr];
}


void CPU65C02::debug_print(const char* message) {
    #ifdef DEBUG
    cout << "[DEBUG] " << message << endl;
    #endif
}


uint16_t CPU65C02::fetch_word() {
    uint16_t low_byte = fetch_byte();
    uint16_t high_byte = fetch_byte();
    return (high_byte << 8) | low_byte;  // 6502 is little-endian
}

void CPU65C02::update_flags(uint8_t value) {
    // Update Zero flag
    if (value == 0) {
        status |= 0x02;  // Set Zero flag
    } else {
        status &= ~0x02; // Clear Zero flag
    }
    
    // Update Negative flag
    if (value & 0x80) {
        status |= 0x80;  // Set Negative flag
    } else {
        status &= ~0x80; // Clear Negative flag
    }
}

CPU65C02::CPU65C02() {
    reset();
    // Simple instruction set with real opcodes
    // LDA series
    opcode_table[0xA9] = &CPU65C02::LDA_IMM;   // LDA Immediate
    opcode_table[0xA5] = &CPU65C02::LDA_ZP; // LDA Zero Page
    opcode_table[0xB5] = &CPU65C02::LDA_ZP_X; // LDA Zero Page, X
    opcode_table[0xAD] = &CPU65C02::LDA_ABS; // LDA Absolute
    opcode_table[0xBD] = &CPU65C02::LDA_ABS_X; // LDA Absolute, X
    opcode_table[0xB9] = &CPU65C02::LDA_ABS_Y; // LDA Absolute, Y
    opcode_table[0xA1] = &CPU65C02::LDA_PRE_IND_X; // LDA Indirect, X
    opcode_table[0xB1] = &CPU65C02::LDA_POST_IND_Y; // LDA Indirect, Y
    opcode_table[0xB2] = &CPU65C02::LDA_IND; // LDA Indirect

    // LDX series
    opcode_table[0xA2] = &CPU65C02::LDX_IMM;  // LDX Immediate
    opcode_table[0xA6] = &CPU65C02::LDX_ZP;  // LDX Zero Page
    opcode_table[0xB6] = &CPU65C02::LDX_ZP_Y;  // LDX Zero Page, Y
    opcode_table[0xAE] = &CPU65C02::LDX_ABS;  // LDX Absolute
    opcode_table[0xBE] = &CPU65C02::LDX_ABS_Y;  // LDX Absolute, Y

    // LDY series
    opcode_table[0xA0] = &CPU65C02::LDY_IMM;  // LDY Immediate
    opcode_table[0xA4] = &CPU65C02::LDY_ZP;  // LDY Zero Page
    opcode_table[0xB4] = &CPU65C02::LDY_ZP_X;  // LDY Zero Page, X
    opcode_table[0xAC] = &CPU65C02::LDY_ABS;  // LDY Absolute
    opcode_table[0xBC] = &CPU65C02::LDY_ABS_X;  // LDY Absolute, X

    // STA series
    opcode_table[0x85] = &CPU65C02::STA_ZP;  // STA Zero Page
    opcode_table[0x95] = &CPU65C02::STA_ZP_X;  // STA Zero Page, X
    opcode_table[0x8D] = &CPU65C02::STA_ABS;  // STA Absolute
    opcode_table[0x9D] = &CPU65C02::STA_ABS_X;  // STA Absolute, X
    opcode_table[0x99] = &CPU65C02::STA_ABS_Y;  // STA Absolute, Y
    opcode_table[0x81] = &CPU65C02::STA_PRE_IND_X;  // STA Indirect, X
    opcode_table[0x91] = &CPU65C02::STA_POST_IND_Y;  // STA Indirect, Y
    opcode_table[0x92] = &CPU65C02::STA_IND;  // STA Indirect

    // STX series
    opcode_table[0x86] = &CPU65C02::STX_ZP;  // STX Zero Page
    opcode_table[0x96] = &CPU65C02::STX_ZP_Y;  // STX Zero Page, Y
    opcode_table[0x8E] = &CPU65C02::STX_ABS;  // STX Absolute

    // STY series
    opcode_table[0x84] = &CPU65C02::STY_ZP;  // STY Zero Page
    opcode_table[0x94] = &CPU65C02::STY_ZP_X;  // STY Zero Page, X
    opcode_table[0x8C] = &CPU65C02::STY_ABS;  // STY Absolute
    opcode_table[0x4C] = &CPU65C02::JMP;  // JMP Absolute
    opcode_table[0x00] = &CPU65C02::BRK;  // BRK
    opcode_table[0xEA] = &CPU65C02::NOP;  // NOP
    opcode_table[0x40] = &CPU65C02::RTI;  // RTI

    // ADC series
    opcode_table[0x69] = &CPU65C02::ADC_IMM;   // ADC Immediate
    opcode_table[0x65] = &CPU65C02::ADC_ZP;    // ADC Zero Page
    opcode_table[0x75] = &CPU65C02::ADC_ZP_X;  // ADC Zero Page, X
    opcode_table[0x6D] = &CPU65C02::ADC_ABS;   // ADC Absolute
    opcode_table[0x7D] = &CPU65C02::ADC_ABS_X; // ADC Absolute, X
    opcode_table[0x79] = &CPU65C02::ADC_ABS_Y; // ADC Absolute, Y
    opcode_table[0x61] = &CPU65C02::ADC_PRE_IND_X;  // ADC Indirect, X
    opcode_table[0x71] = &CPU65C02::ADC_POST_IND_Y; // ADC Indirect, Y
    opcode_table[0x72] = &CPU65C02::ADC_IND;   // ADC Indirect

    // SBC series
    opcode_table[0xE9] = &CPU65C02::SBC_IMM;   // SBC Immediate
    opcode_table[0xE5] = &CPU65C02::SBC_ZP;    // SBC Zero Page
    opcode_table[0xF5] = &CPU65C02::SBC_ZP_X;  // SBC Zero Page, X
    opcode_table[0xED] = &CPU65C02::SBC_ABS;   // SBC Absolute
    opcode_table[0xFD] = &CPU65C02::SBC_ABS_X; // SBC Absolute, X
    opcode_table[0xF9] = &CPU65C02::SBC_ABS_Y; // SBC Absolute, Y
    opcode_table[0xE1] = &CPU65C02::SBC_PRE_IND_X;  // SBC Indirect, X
    opcode_table[0xF1] = &CPU65C02::SBC_POST_IND_Y; // SBC Indirect, Y
    opcode_table[0xF2] = &CPU65C02::SBC_IND;   // SBC Indirect

    // INC series
    opcode_table[0xE6] = &CPU65C02::INC_ZP;    // INC Zero Page
    opcode_table[0xF6] = &CPU65C02::INC_ZP_X;  // INC Zero Page, X
    opcode_table[0xEE] = &CPU65C02::INC_ABS;   // INC Absolute
    opcode_table[0xFE] = &CPU65C02::INC_ABS_X; // INC Absolute, X

    // INX/INY
    opcode_table[0xE8] = &CPU65C02::INX;       // INX
    opcode_table[0xC8] = &CPU65C02::INY;       // INY

    // DEC series
    opcode_table[0xC6] = &CPU65C02::DEC_ZP;    // DEC Zero Page
    opcode_table[0xD6] = &CPU65C02::DEC_ZP_X;  // DEC Zero Page, X
    opcode_table[0xCE] = &CPU65C02::DEC_ABS;   // DEC Absolute
    opcode_table[0xDE] = &CPU65C02::DEC_ABS_X; // DEC Absolute, X

    // DEX/DEY
    opcode_table[0xCA] = &CPU65C02::DEX;       // DEX
    opcode_table[0x88] = &CPU65C02::DEY;       // DEY

    // AND series
    opcode_table[0x29] = &CPU65C02::AND_IMM;   // AND Immediate
    opcode_table[0x25] = &CPU65C02::AND_ZP;    // AND Zero Page
    opcode_table[0x35] = &CPU65C02::AND_ZP_X;  // AND Zero Page, X
    opcode_table[0x2D] = &CPU65C02::AND_ABS;   // AND Absolute
    opcode_table[0x3D] = &CPU65C02::AND_ABS_X; // AND Absolute, X
    opcode_table[0x39] = &CPU65C02::AND_ABS_Y; // AND Absolute, Y
    opcode_table[0x21] = &CPU65C02::AND_PRE_IND_X;  // AND Indirect, X
    opcode_table[0x31] = &CPU65C02::AND_POST_IND_Y; // AND Indirect, Y
    opcode_table[0x32] = &CPU65C02::AND_IND;   // AND Indirect

    // ORA series
    opcode_table[0x09] = &CPU65C02::ORA_IMM;   // ORA Immediate
    opcode_table[0x05] = &CPU65C02::ORA_ZP;    // ORA Zero Page
    opcode_table[0x15] = &CPU65C02::ORA_ZP_X;  // ORA Zero Page, X
    opcode_table[0x0D] = &CPU65C02::ORA_ABS;   // ORA Absolute
    opcode_table[0x1D] = &CPU65C02::ORA_ABS_X; // ORA Absolute, X
    opcode_table[0x19] = &CPU65C02::ORA_ABS_Y; // ORA Absolute, Y
    opcode_table[0x01] = &CPU65C02::ORA_PRE_IND_X;  // ORA Indirect, X
    opcode_table[0x11] = &CPU65C02::ORA_POST_IND_Y; // ORA Indirect, Y
    opcode_table[0x12] = &CPU65C02::ORA_IND;   // ORA Indirect

    // EOR series
    opcode_table[0x49] = &CPU65C02::EOR_IMM;   // EOR Immediate
    opcode_table[0x45] = &CPU65C02::EOR_ZP;    // EOR Zero Page
    opcode_table[0x55] = &CPU65C02::EOR_ZP_X;  // EOR Zero Page, X
    opcode_table[0x4D] = &CPU65C02::EOR_ABS;   // EOR Absolute
    opcode_table[0x5D] = &CPU65C02::EOR_ABS_X; // EOR Absolute, X
    opcode_table[0x59] = &CPU65C02::EOR_ABS_Y; // EOR Absolute, Y
    opcode_table[0x41] = &CPU65C02::EOR_PRE_IND_X;  // EOR Indirect, X
    opcode_table[0x51] = &CPU65C02::EOR_POST_IND_Y; // EOR Indirect, Y
    opcode_table[0x52] = &CPU65C02::EOR_IND;   // EOR Indirect

    // ASL series
    opcode_table[0x0A] = &CPU65C02::ASL_ACC;   // ASL Accumulator
    opcode_table[0x06] = &CPU65C02::ASL_ZP;    // ASL Zero Page
    opcode_table[0x16] = &CPU65C02::ASL_ZP_X;  // ASL Zero Page, X
    opcode_table[0x0E] = &CPU65C02::ASL_ABS;   // ASL Absolute
    opcode_table[0x1E] = &CPU65C02::ASL_ABS_X; // ASL Absolute, X

    // LSR series
    opcode_table[0x4A] = &CPU65C02::LSR_ACC;   // LSR Accumulator
    opcode_table[0x46] = &CPU65C02::LSR_ZP;    // LSR Zero Page
    opcode_table[0x56] = &CPU65C02::LSR_ZP_X;  // LSR Zero Page, X
    opcode_table[0x4E] = &CPU65C02::LSR_ABS;   // LSR Absolute
    opcode_table[0x5E] = &CPU65C02::LSR_ABS_X; // LSR Absolute, X

    // ROL series
    opcode_table[0x2A] = &CPU65C02::ROL_ACC;   // ROL Accumulator
    opcode_table[0x26] = &CPU65C02::ROL_ZP;    // ROL Zero Page
    opcode_table[0x36] = &CPU65C02::ROL_ZP_X;  // ROL Zero Page, X
    opcode_table[0x2E] = &CPU65C02::ROL_ABS;   // ROL Absolute
    opcode_table[0x3E] = &CPU65C02::ROL_ABS_X; // ROL Absolute, X

    // ROR series
    opcode_table[0x6A] = &CPU65C02::ROR_ACC;   // ROR Accumulator
    opcode_table[0x66] = &CPU65C02::ROR_ZP;    // ROR Zero Page
    opcode_table[0x76] = &CPU65C02::ROR_ZP_X;  // ROR Zero Page, X
    opcode_table[0x6E] = &CPU65C02::ROR_ABS;   // ROR Absolute
    opcode_table[0x7E] = &CPU65C02::ROR_ABS_X; // ROR Absolute, X

    // Stack Operations
    opcode_table[0x48] = &CPU65C02::PHA;  // PHA
    opcode_table[0x08] = &CPU65C02::PHP;  // PHP
    opcode_table[0x68] = &CPU65C02::PLA;  // PLA
    opcode_table[0x28] = &CPU65C02::PLP;  // PLP
    opcode_table[0xBA] = &CPU65C02::TSX;  // TSX
    opcode_table[0x9A] = &CPU65C02::TXS;  // TXS

    // Branch Instructions
    opcode_table[0x90] = &CPU65C02::BCC;  // BCC
    opcode_table[0xB0] = &CPU65C02::BCS;  // BCS
    opcode_table[0xF0] = &CPU65C02::BEQ;  // BEQ
    opcode_table[0xD0] = &CPU65C02::BNE;  // BNE
    opcode_table[0x30] = &CPU65C02::BMI;  // BMI
    opcode_table[0x10] = &CPU65C02::BPL;  // BPL
    opcode_table[0x50] = &CPU65C02::BVC;  // BVC
    opcode_table[0x70] = &CPU65C02::BVS;  // BVS

    // Status Flag Operations
    opcode_table[0x18] = &CPU65C02::CLC;  // CLC
    opcode_table[0x38] = &CPU65C02::SEC;  // SEC
    opcode_table[0xD8] = &CPU65C02::CLD;  // CLD
    opcode_table[0xF8] = &CPU65C02::SED;  // SED
    opcode_table[0x58] = &CPU65C02::CLI;  // CLI
    opcode_table[0x78] = &CPU65C02::SEI;  // SEI
    opcode_table[0xB8] = &CPU65C02::CLV;  // CLV

    // Comparison Operations
    opcode_table[0xC9] = &CPU65C02::CMP_IMM;   // CMP Immediate
    opcode_table[0xC5] = &CPU65C02::CMP_ZP;    // CMP Zero Page
    opcode_table[0xD5] = &CPU65C02::CMP_ZP_X;  // CMP Zero Page, X
    opcode_table[0xCD] = &CPU65C02::CMP_ABS;   // CMP Absolute
    opcode_table[0xDD] = &CPU65C02::CMP_ABS_X; // CMP Absolute, X
    opcode_table[0xD9] = &CPU65C02::CMP_ABS_Y; // CMP Absolute, Y
    opcode_table[0xC1] = &CPU65C02::CMP_PRE_IND_X;  // CMP Indirect, X
    opcode_table[0xD1] = &CPU65C02::CMP_POST_IND_Y; // CMP Indirect, Y
    opcode_table[0xD2] = &CPU65C02::CMP_IND;   // CMP Indirect

    opcode_table[0xE0] = &CPU65C02::CPX_IMM;   // CPX Immediate
    opcode_table[0xE4] = &CPU65C02::CPX_ZP;    // CPX Zero Page
    opcode_table[0xEC] = &CPU65C02::CPX_ABS;   // CPX Absolute

    opcode_table[0xC0] = &CPU65C02::CPY_IMM;   // CPY Immediate
    opcode_table[0xC4] = &CPU65C02::CPY_ZP;    // CPY Zero Page
    opcode_table[0xCC] = &CPU65C02::CPY_ABS;   // CPY Absolute

    // Additional 65C02-specific instructions
    opcode_table[0x80] = &CPU65C02::BRA;       // BRA
    opcode_table[0xDA] = &CPU65C02::PHX;       // PHX
    opcode_table[0x5A] = &CPU65C02::PHY;       // PHY
    opcode_table[0xFA] = &CPU65C02::PLX;       // PLX
    opcode_table[0x7A] = &CPU65C02::PLY;       // PLY
    opcode_table[0x64] = &CPU65C02::STZ_ZP;    // STZ Zero Page
    opcode_table[0x74] = &CPU65C02::STZ_ZP_X;  // STZ Zero Page, X
    opcode_table[0x9C] = &CPU65C02::STZ_ABS;   // STZ Absolute
    opcode_table[0x9E] = &CPU65C02::STZ_ABS_X; // STZ Absolute, X
    opcode_table[0x14] = &CPU65C02::TRB_ZP;    // TRB Zero Page
    opcode_table[0x1C] = &CPU65C02::TRB_ABS;   // TRB Absolute
    opcode_table[0x04] = &CPU65C02::TSB_ZP;    // TSB Zero Page
    opcode_table[0x0C] = &CPU65C02::TSB_ABS;   // TSB Absolute
}

void CPU65C02::reset() {
    A = 0;
    X = 0;
    Y = 0;
    P = 0;
    S = 0;
    PC = 0;
    status = 0;
}

void CPU65C02::load_program(uint8_t* program, size_t size) {
    memcpy(RAM, program, size);
}


void CPU65C02::execute() {
    debug_print("Starting program execution");
    bool running = true;
    while (running && PC < 65535) {
        uint8_t opcode = RAM[PC];
        if (opcode == 0x00) {  // BRK instruction
            running = false;
            cout << "BRK - Program terminated" << endl;
            break;
        }
        #ifdef DEBUG
            cout << "PC: " << hex << (int)PC << endl;
        #endif
        #ifdef SINGLE_STEP
            input();
        #endif  
        debug_print("Fetching next instruction");
        (this->*opcode_table[RAM[PC++]])();
    }
    debug_print("Program execution completed");
}

void CPU65C02::input() {
    cout << "\nPress Enter to continue...";
    cin.get();
}

void CPU65C02::print_registers() {
    cout << "A: $" << hex << (int)A << ", X: $" << (int)X << ", Y: $" << (int)Y << ", P: $" << (int)P << ", S: $" << (int)S << ", PC: $" << PC << endl;
}

// LDA instructions implementation
void CPU65C02::LDA_ZP() {
    uint8_t addr = fetch_byte();
    A = RAM[addr];
    update_flags(A);
    cout << "LDA $" << hex << (int)addr << endl;
    print_registers();
}

void CPU65C02::LDA_ZP_X() {
    uint8_t addr = fetch_byte() + X;
    A = RAM[addr];
    update_flags(A);
    cout << "LDA $" << hex << (int)addr << ",X" << endl;
    print_registers();
}

void CPU65C02::LDA_IMM() {
    debug_print("Executing LDA_IMM");
    A = fetch_byte();
    update_flags(A);
    cout << "LDA #$" << hex << (int)A << endl;
    print_registers();
}

void CPU65C02::LDA_ABS() {
    uint16_t addr = fetch_word();
    A = RAM[addr];
    update_flags(A);
    cout << "LDA $" << hex << setw(4) << setfill('0') << addr << endl;
    print_registers();
}

void CPU65C02::LDA_ABS_Y() {
    uint16_t addr = fetch_word();
    A = fetch_byte(addr + Y);
    update_flags(A);
    cout << "LDA $" << hex << setw(4) << setfill('0') << addr << ",Y" << endl;
    print_registers();
}

void CPU65C02::LDA_ABS_X() {
    uint16_t addr = fetch_word();
    A = fetch_byte(addr + X);
    update_flags(A);
    cout << "LDA $" << hex << setw(4) << setfill('0') << addr << ",X" << endl;
    print_registers();
}

void CPU65C02::LDA_PRE_IND_X() {
    uint8_t addr = fetch_byte() + X;
    A = fetch_byte(addr) + (fetch_byte(addr + 1) << 8);
    update_flags(A);
    cout << "LDA ($" << hex << (int)addr << ",X)" << endl;
    print_registers();
}

void CPU65C02::LDA_POST_IND_Y() {
    uint8_t pre_zp_addr = fetch_byte();
    uint16_t base = fetch_byte(pre_zp_addr) + (fetch_byte(pre_zp_addr + 1) << 8);
    A = fetch_byte(base + Y);
    update_flags(A);
    cout << "LDA ($" << hex << (int)pre_zp_addr << "),Y" << endl;
    print_registers();
}

void CPU65C02::LDA_IND() {
    uint8_t addr = fetch_byte();
    A = fetch_byte(addr) + (fetch_byte(addr + 1) << 8);
    update_flags(A);
    cout << "LDA ($" << hex << (int)addr << ")" << endl;
    print_registers();
}

// LDX instructions implementation
void CPU65C02::LDX_IMM() {
    X = fetch_byte();
    update_flags(X);
    cout << "LDX #$" << hex << (int)X << endl;
    print_registers();
}

void CPU65C02::LDX_ZP() {
    uint8_t addr = fetch_byte();
    X = RAM[addr];
    update_flags(X);
    cout << "LDX $" << hex << (int)addr << endl;
    print_registers();
}

void CPU65C02::LDX_ZP_Y() {
    uint8_t zp_addr = fetch_byte() + Y;
    X = fetch_byte(zp_addr);
    update_flags(X);
    cout << "LDX $" << hex << (int)zp_addr << ",Y" << endl;
    print_registers();
}

void CPU65C02::LDX_ABS() {
    uint16_t addr = fetch_word();
    X = fetch_byte(addr);
    update_flags(X);
    cout << "LDX $" << hex << setw(4) << setfill('0') << addr << endl;
    print_registers();
}

void CPU65C02::LDX_ABS_Y() {
    uint16_t addr = fetch_word();
    X = fetch_byte(addr + Y);
    update_flags(X);
    cout << "LDX $" << hex << setw(4) << setfill('0') << addr << ",Y" << endl;
    print_registers();
}

// LDY instructions implementation
void CPU65C02::LDY_IMM() {
    Y = fetch_byte();
    update_flags(Y);
    cout << "LDY #$" << hex << (int)Y << endl;
    print_registers();
}

void CPU65C02::LDY_ZP() {
    uint8_t addr = fetch_byte();
    Y = RAM[addr];
    update_flags(Y);
    cout << "LDY $" << hex << (int)addr << endl;
    print_registers();
}

void CPU65C02::LDY_ZP_X() {
    uint8_t zp_addr = fetch_byte() + X;
    Y = fetch_byte(zp_addr);
    update_flags(Y);
    cout << "LDY $" << hex << (int)zp_addr << ",X" << endl;
    print_registers();
}

void CPU65C02::LDY_ABS() {
    uint16_t addr = fetch_word();
    Y = fetch_byte(addr);
    update_flags(Y);
    cout << "LDY $" << hex << setw(4) << setfill('0') << addr << endl;
    print_registers();
}

void CPU65C02::LDY_ABS_X() {
    uint16_t addr = fetch_word();
    Y = fetch_byte(addr + X);
    update_flags(Y);
    cout << "LDY $" << hex << setw(4) << setfill('0') << addr << ",X" << endl;
    print_registers();
}

// STA instructions implementation
void CPU65C02::STA_ZP() {
    debug_print("Executing STA_ZP");
    uint8_t addr = fetch_byte();
    RAM[addr] = A;
    cout << "STA $" << hex << (int)addr << endl;
    debug_print("Stored value in memory");
}

void CPU65C02::STA_ZP_X() {
    uint16_t addr = (fetch_byte() + X) & 0xFF;
    RAM[addr] = A;
    cout << "STA $" << hex << (int)addr << ",X" << endl;
}

void CPU65C02::STA_ABS() {
    uint16_t addr = fetch_word();
    RAM[addr] = A;
    cout << "STA $" << hex << setw(4) << setfill('0') << addr << endl;
}

void CPU65C02::STA_ABS_X() {
    uint16_t addr = fetch_word();
    RAM[addr + X] = A;
    cout << "STA $" << hex << setw(4) << setfill('0') << addr << ",X" << endl;
}

void CPU65C02::STA_ABS_Y() {
    uint16_t addr = fetch_word();
    RAM[addr + Y] = A;
    cout << "STA $" << hex << setw(4) << setfill('0') << addr << ",Y" << endl;
}

void CPU65C02::STA_PRE_IND_X() {
    uint8_t zp_addr = fetch_byte() + X;
    uint16_t base = fetch_byte(zp_addr) + (fetch_byte(zp_addr + 1) << 8);
    RAM[base] = A;
    cout << "STA ($" << hex << (int)zp_addr << ",X)" << endl;
}

void CPU65C02::STA_POST_IND_Y() {
    uint8_t zp_addr = fetch_byte();
    uint16_t base = fetch_byte(zp_addr) + (fetch_byte(zp_addr + 1) << 8);
    RAM[base + Y] = A;
    cout << "STA ($" << hex << (int)zp_addr << "),Y" << endl;
}

void CPU65C02::STA_IND() {
    uint8_t zp_addr = fetch_byte();
    uint16_t base = fetch_byte(zp_addr) + (fetch_byte(zp_addr + 1) << 8);
    RAM[base] = A;
    cout << "STA ($" << hex << (int)zp_addr << ")" << endl;
}

// STX instructions implementation
void CPU65C02::STX_ZP() {
    uint8_t addr = fetch_byte();
    RAM[addr] = X;
    cout << "STX $" << hex << (int)addr << endl;
}

void CPU65C02::STX_ZP_Y() {
    uint8_t zp_addr = fetch_byte() + Y;
    RAM[zp_addr] = X;
    cout << "STX $" << hex << (int)zp_addr << ",Y" << endl;
}

void CPU65C02::STX_ABS() {
    uint16_t addr = fetch_word();
    RAM[addr] = X;
    cout << "STX $" << hex << setw(4) << setfill('0') << addr << endl;
}

// STY instructions implementation
void CPU65C02::STY_ZP() {
    uint8_t addr = fetch_byte();
    RAM[addr] = Y;
    cout << "STY $" << hex << (int)addr << endl;
}

void CPU65C02::STY_ZP_X() {
    uint8_t zp_addr = fetch_byte() + X;
    RAM[zp_addr] = Y;
    cout << "STY $" << hex << (int)zp_addr << ",X" << endl;
}

void CPU65C02::STY_ABS() {
    uint16_t addr = fetch_word();
    RAM[addr] = Y;
    cout << "STY $" << hex << setw(4) << setfill('0') << addr << endl;
}

// Other instructions implementation
void CPU65C02::JMP() {
    PC = fetch_byte();
    cout << "JMP $" << hex << (int)PC << endl;
}

void CPU65C02::BRK() {
    cout << "BRK" << endl;
}

void CPU65C02::NOP() {
    cout << "NOP" << endl;
}

void CPU65C02::RTI() {
    cout << "RTI" << endl;
    PC = 0;
}

// ADC implementations
void CPU65C02::ADC_IMM() {
    debug_print("Executing ADC_IMM");
    uint8_t operand = fetch_byte();
    uint16_t result = A + operand + (status & 0x01); // Add with carry
    status = (status & ~0x01) | (result > 0xFF); // Set carry if result > 255
    A = result & 0xFF;
    update_flags(A);
    cout << "ADC #$" << hex << (int)operand << endl;
    print_registers();
}

void CPU65C02::ADC_ZP() {
    uint8_t addr = fetch_byte();
    uint8_t operand = RAM[addr];
    uint16_t result = A + operand + (status & 0x01);
    status = (status & ~0x01) | (result > 0xFF);
    A = result & 0xFF;
    update_flags(A);
    cout << "ADC $" << hex << (int)addr << endl;
    print_registers();
}

void CPU65C02::ADC_ZP_X() {
    uint8_t addr = fetch_byte() + X;
    uint8_t operand = RAM[addr];
    uint16_t result = A + operand + (status & 0x01);
    status = (status & ~0x01) | (result > 0xFF);
    A = result & 0xFF;
    update_flags(A);
    cout << "ADC $" << hex << (int)addr << ",X" << endl;
    print_registers();
}

void CPU65C02::ADC_ABS() {
    uint16_t addr = fetch_word();
    uint8_t operand = RAM[addr];
    uint16_t result = A + operand + (status & 0x01);
    status = (status & ~0x01) | (result > 0xFF);
    A = result & 0xFF;
    update_flags(A);
    cout << "ADC $" << hex << setw(4) << setfill('0') << addr << endl;
    print_registers();
}

void CPU65C02::ADC_ABS_X() {
    uint16_t addr = fetch_word() + X;
    uint8_t operand = RAM[addr];
    uint16_t result = A + operand + (status & 0x01);
    status = (status & ~0x01) | (result > 0xFF);
    A = result & 0xFF;
    update_flags(A);
    cout << "ADC $" << hex << setw(4) << setfill('0') << addr << ",X" << endl;
    print_registers();
}

void CPU65C02::ADC_ABS_Y() {
    uint16_t addr = fetch_word() + Y;
    uint8_t operand = RAM[addr];
    uint16_t result = A + operand + (status & 0x01);
    status = (status & ~0x01) | (result > 0xFF);
    A = result & 0xFF;
    update_flags(A);
    cout << "ADC $" << hex << setw(4) << setfill('0') << addr << ",Y" << endl;
    print_registers();
}

void CPU65C02::ADC_PRE_IND_X() {
    uint8_t zp_addr = fetch_byte() + X;
    uint16_t addr = fetch_byte(zp_addr) + (fetch_byte(zp_addr + 1) << 8);
    uint8_t operand = RAM[addr];
    uint16_t result = A + operand + (status & 0x01);
    status = (status & ~0x01) | (result > 0xFF);
    A = result & 0xFF;
    update_flags(A);
    cout << "ADC ($" << hex << (int)zp_addr << ",X)" << endl;
    print_registers();
}

void CPU65C02::ADC_POST_IND_Y() {
    uint8_t zp_addr = fetch_byte();
    uint16_t base = fetch_byte(zp_addr) + (fetch_byte(zp_addr + 1) << 8);
    uint8_t operand = RAM[base + Y];
    uint16_t result = A + operand + (status & 0x01);
    status = (status & ~0x01) | (result > 0xFF);
    A = result & 0xFF;
    update_flags(A);
    cout << "ADC ($" << hex << (int)zp_addr << "),Y" << endl;
    print_registers();
}

void CPU65C02::ADC_IND() {
    uint8_t zp_addr = fetch_byte();
    uint16_t addr = fetch_byte(zp_addr) + (fetch_byte(zp_addr + 1) << 8);
    uint8_t operand = RAM[addr];
    uint16_t result = A + operand + (status & 0x01);
    status = (status & ~0x01) | (result > 0xFF);
    A = result & 0xFF;
    update_flags(A);
    cout << "ADC ($" << hex << (int)zp_addr << ")" << endl;
    print_registers();
}

// SBC implementations
void CPU65C02::SBC_IMM() {
    debug_print("Executing SBC_IMM");
    uint8_t operand = fetch_byte();
    uint16_t result = A - operand - !(status & 0x01); // Subtract with borrow
    status = (status & ~0x01) | (result <= 0xFF); // Set carry if result >= 0
    A = result & 0xFF;
    update_flags(A);
    cout << "SBC #$" << hex << (int)operand << endl;
    print_registers();
}

void CPU65C02::SBC_ZP() {
    uint8_t addr = fetch_byte();
    uint8_t operand = RAM[addr];
    uint16_t result = A - operand - !(status & 0x01);
    status = (status & ~0x01) | (result <= 0xFF);
    A = result & 0xFF;
    update_flags(A);
    cout << "SBC $" << hex << (int)addr << endl;
    print_registers();
}

void CPU65C02::SBC_ZP_X() {
    uint8_t addr = fetch_byte() + X;
    uint8_t operand = RAM[addr];
    uint16_t result = A - operand - !(status & 0x01);
    status = (status & ~0x01) | (result <= 0xFF);
    A = result & 0xFF;
    update_flags(A);
    cout << "SBC $" << hex << (int)addr << ",X" << endl;
    print_registers();
}

void CPU65C02::SBC_ABS() {
    uint16_t addr = fetch_word();
    uint8_t operand = RAM[addr];
    uint16_t result = A - operand - !(status & 0x01);
    status = (status & ~0x01) | (result <= 0xFF);
    A = result & 0xFF;
    update_flags(A);
    cout << "SBC $" << hex << setw(4) << setfill('0') << addr << endl;
    print_registers();
}

void CPU65C02::SBC_ABS_X() {
    uint16_t addr = fetch_word() + X;
    uint8_t operand = RAM[addr];
    uint16_t result = A - operand - !(status & 0x01);
    status = (status & ~0x01) | (result <= 0xFF);
    A = result & 0xFF;
    update_flags(A);
    cout << "SBC $" << hex << setw(4) << setfill('0') << addr << ",X" << endl;
    print_registers();
}

void CPU65C02::SBC_ABS_Y() {
    uint16_t addr = fetch_word() + Y;
    uint8_t operand = RAM[addr];
    uint16_t result = A - operand - !(status & 0x01);
    status = (status & ~0x01) | (result <= 0xFF);
    A = result & 0xFF;
    update_flags(A);
    cout << "SBC $" << hex << setw(4) << setfill('0') << addr << ",Y" << endl;
    print_registers();
}

void CPU65C02::SBC_PRE_IND_X() {
    uint8_t zp_addr = fetch_byte() + X;
    uint16_t addr = fetch_byte(zp_addr) + (fetch_byte(zp_addr + 1) << 8);
    uint8_t operand = RAM[addr];
    uint16_t result = A - operand - !(status & 0x01);
    status = (status & ~0x01) | (result <= 0xFF);
    A = result & 0xFF;
    update_flags(A);
    cout << "SBC ($" << hex << (int)zp_addr << ",X)" << endl;
    print_registers();
}

void CPU65C02::SBC_POST_IND_Y() {
    uint8_t zp_addr = fetch_byte();
    uint16_t base = fetch_byte(zp_addr) + (fetch_byte(zp_addr + 1) << 8);
    uint8_t operand = RAM[base + Y];
    uint16_t result = A - operand - !(status & 0x01);
    status = (status & ~0x01) | (result <= 0xFF);
    A = result & 0xFF;
    update_flags(A);
    cout << "SBC ($" << hex << (int)zp_addr << "),Y" << endl;
    print_registers();
}

void CPU65C02::SBC_IND() {
    uint8_t zp_addr = fetch_byte();
    uint16_t addr = fetch_byte(zp_addr) + (fetch_byte(zp_addr + 1) << 8);
    uint8_t operand = RAM[addr];
    uint16_t result = A - operand - !(status & 0x01);
    status = (status & ~0x01) | (result <= 0xFF);
    A = result & 0xFF;
    update_flags(A);
    cout << "SBC ($" << hex << (int)zp_addr << ")" << endl;
    print_registers();
}

// INC implementations
void CPU65C02::INC_ZP() {
    uint8_t addr = fetch_byte();
    RAM[addr]++;
    update_flags(RAM[addr]);
    cout << "INC $" << hex << (int)addr << endl;
    print_registers();
}

void CPU65C02::INC_ZP_X() {
    uint8_t addr = fetch_byte() + X;
    RAM[addr]++;
    update_flags(RAM[addr]);
    cout << "INC $" << hex << (int)addr << ",X" << endl;
    print_registers();
}

void CPU65C02::INC_ABS() {
    uint16_t addr = fetch_word();
    RAM[addr]++;
    update_flags(RAM[addr]);
    cout << "INC $" << hex << setw(4) << setfill('0') << addr << endl;
    print_registers();
}

void CPU65C02::INC_ABS_X() {
    uint16_t addr = fetch_word() + X;
    RAM[addr]++;
    update_flags(RAM[addr]);
    cout << "INC $" << hex << setw(4) << setfill('0') << addr << ",X" << endl;
    print_registers();
}

void CPU65C02::INX() {
    X++;
    update_flags(X);
    cout << "INX" << endl;
    print_registers();
}

void CPU65C02::INY() {
    Y++;
    update_flags(Y);
    cout << "INY" << endl;
    print_registers();
}

// DEC implementations
void CPU65C02::DEC_ZP() {
    uint8_t addr = fetch_byte();
    RAM[addr]--;
    update_flags(RAM[addr]);
    cout << "DEC $" << hex << (int)addr << endl;
    print_registers();
}

void CPU65C02::DEC_ZP_X() {
    uint8_t addr = fetch_byte() + X;
    RAM[addr]--;
    update_flags(RAM[addr]);
    cout << "DEC $" << hex << (int)addr << ",X" << endl;
    print_registers();
}

void CPU65C02::DEC_ABS() {
    uint16_t addr = fetch_word();
    RAM[addr]--;
    update_flags(RAM[addr]);
    cout << "DEC $" << hex << setw(4) << setfill('0') << addr << endl;
    print_registers();
}

void CPU65C02::DEC_ABS_X() {
    uint16_t addr = fetch_word() + X;
    RAM[addr]--;
    update_flags(RAM[addr]);
    cout << "DEC $" << hex << setw(4) << setfill('0') << addr << ",X" << endl;
    print_registers();
}

void CPU65C02::DEX() {
    X--;
    update_flags(X);
    cout << "DEX" << endl;
    print_registers();
}

void CPU65C02::DEY() {
    Y--;
    update_flags(Y);
    cout << "DEY" << endl;
    print_registers();
}

// AND implementations
void CPU65C02::AND_IMM() {
    uint8_t operand = fetch_byte();
    A &= operand;
    update_flags(A);
    cout << "AND #$" << hex << (int)operand << endl;
    print_registers();
}

void CPU65C02::AND_ZP() {
    uint8_t addr = fetch_byte();
    A &= RAM[addr];
    update_flags(A);
    cout << "AND $" << hex << (int)addr << endl;
    print_registers();
}

void CPU65C02::AND_ZP_X() {
    uint8_t addr = fetch_byte() + X;
    A &= RAM[addr];
    update_flags(A);
    cout << "AND $" << hex << (int)addr << ",X" << endl;
    print_registers();
}

void CPU65C02::AND_ABS() {
    uint16_t addr = fetch_word();
    A &= RAM[addr];
    update_flags(A);
    cout << "AND $" << hex << setw(4) << setfill('0') << addr << endl;
    print_registers();
}

void CPU65C02::AND_ABS_X() {
    uint16_t addr = fetch_word() + X;
    A &= RAM[addr];
    update_flags(A);
    cout << "AND $" << hex << setw(4) << setfill('0') << addr << ",X" << endl;
    print_registers();
}

void CPU65C02::AND_ABS_Y() {
    uint16_t addr = fetch_word() + Y;
    A &= RAM[addr];
    update_flags(A);
    cout << "AND $" << hex << setw(4) << setfill('0') << addr << ",Y" << endl;
    print_registers();
}

void CPU65C02::AND_PRE_IND_X() {
    uint8_t zp_addr = fetch_byte() + X;
    uint16_t addr = fetch_byte(zp_addr) + (fetch_byte(zp_addr + 1) << 8);
    A &= RAM[addr];
    update_flags(A);
    cout << "AND ($" << hex << (int)zp_addr << ",X)" << endl;
    print_registers();
}

void CPU65C02::AND_POST_IND_Y() {
    uint8_t zp_addr = fetch_byte();
    uint16_t base = fetch_byte(zp_addr) + (fetch_byte(zp_addr + 1) << 8);
    A &= RAM[base + Y];
    update_flags(A);
    cout << "AND ($" << hex << (int)zp_addr << "),Y" << endl;
    print_registers();
}

void CPU65C02::AND_IND() {
    uint8_t zp_addr = fetch_byte();
    uint16_t addr = fetch_byte(zp_addr) + (fetch_byte(zp_addr + 1) << 8);
    A &= RAM[addr];
    update_flags(A);
    cout << "AND ($" << hex << (int)zp_addr << ")" << endl;
    print_registers();
}

// ORA implementations
void CPU65C02::ORA_IMM() {
    uint8_t operand = fetch_byte();
    A |= operand;
    update_flags(A);
    cout << "ORA #$" << hex << (int)operand << endl;
    print_registers();
}

void CPU65C02::ORA_ZP() {
    uint8_t addr = fetch_byte();
    A |= RAM[addr];
    update_flags(A);
    cout << "ORA $" << hex << (int)addr << endl;
    print_registers();
}

void CPU65C02::ORA_ZP_X() {
    uint8_t addr = fetch_byte() + X;
    A |= RAM[addr];
    update_flags(A);
    cout << "ORA $" << hex << (int)addr << ",X" << endl;
    print_registers();
}

void CPU65C02::ORA_ABS() {
    uint16_t addr = fetch_word();
    A |= RAM[addr];
    update_flags(A);
    cout << "ORA $" << hex << setw(4) << setfill('0') << addr << endl;
    print_registers();
}

void CPU65C02::ORA_ABS_X() {
    uint16_t addr = fetch_word() + X;
    A |= RAM[addr];
    update_flags(A);
    cout << "ORA $" << hex << setw(4) << setfill('0') << addr << ",X" << endl;
    print_registers();
}

void CPU65C02::ORA_ABS_Y() {
    uint16_t addr = fetch_word() + Y;
    A |= RAM[addr];
    update_flags(A);
    cout << "ORA $" << hex << setw(4) << setfill('0') << addr << ",Y" << endl;
    print_registers();
}

void CPU65C02::ORA_PRE_IND_X() {
    uint8_t zp_addr = fetch_byte() + X;
    uint16_t addr = fetch_byte(zp_addr) + (fetch_byte(zp_addr + 1) << 8);
    A |= RAM[addr];
    update_flags(A);
    cout << "ORA ($" << hex << (int)zp_addr << ",X)" << endl;
    print_registers();
}

void CPU65C02::ORA_POST_IND_Y() {
    uint8_t zp_addr = fetch_byte();
    uint16_t base = fetch_byte(zp_addr) + (fetch_byte(zp_addr + 1) << 8);
    A |= RAM[base + Y];
    update_flags(A);
    cout << "ORA ($" << hex << (int)zp_addr << "),Y" << endl;
    print_registers();
}

void CPU65C02::ORA_IND() {
    uint8_t zp_addr = fetch_byte();
    uint16_t addr = fetch_byte(zp_addr) + (fetch_byte(zp_addr + 1) << 8);
    A |= RAM[addr];
    update_flags(A);
    cout << "ORA ($" << hex << (int)zp_addr << ")" << endl;
    print_registers();
}

// EOR implementations
void CPU65C02::EOR_IMM() {
    uint8_t operand = fetch_byte();
    A ^= operand;
    update_flags(A);
    cout << "EOR #$" << hex << (int)operand << endl;
    print_registers();
}

void CPU65C02::EOR_ZP() {
    uint8_t addr = fetch_byte();
    A ^= RAM[addr];
    update_flags(A);
    cout << "EOR $" << hex << (int)addr << endl;
    print_registers();
}

void CPU65C02::EOR_ZP_X() {
    uint8_t addr = fetch_byte() + X;
    A ^= RAM[addr];
    update_flags(A);
    cout << "EOR $" << hex << (int)addr << ",X" << endl;
    print_registers();
}

void CPU65C02::EOR_ABS() {
    uint16_t addr = fetch_word();
    A ^= RAM[addr];
    update_flags(A);
    cout << "EOR $" << hex << setw(4) << setfill('0') << addr << endl;
    print_registers();
}

void CPU65C02::EOR_ABS_X() {
    uint16_t addr = fetch_word() + X;
    A ^= RAM[addr];
    update_flags(A);
    cout << "EOR $" << hex << setw(4) << setfill('0') << addr << ",X" << endl;
    print_registers();
}

void CPU65C02::EOR_ABS_Y() {
    uint16_t addr = fetch_word() + Y;
    A ^= RAM[addr];
    update_flags(A);
    cout << "EOR $" << hex << setw(4) << setfill('0') << addr << ",Y" << endl;
    print_registers();
}

void CPU65C02::EOR_PRE_IND_X() {
    uint8_t zp_addr = fetch_byte() + X;
    uint16_t addr = fetch_byte(zp_addr) + (fetch_byte(zp_addr + 1) << 8);
    A ^= RAM[addr];
    update_flags(A);
    cout << "EOR ($" << hex << (int)zp_addr << ",X)" << endl;
    print_registers();
}

void CPU65C02::EOR_POST_IND_Y() {
    uint8_t zp_addr = fetch_byte();
    uint16_t base = fetch_byte(zp_addr) + (fetch_byte(zp_addr + 1) << 8);
    A ^= RAM[base + Y];
    update_flags(A);
    cout << "EOR ($" << hex << (int)zp_addr << "),Y" << endl;
    print_registers();
}

void CPU65C02::EOR_IND() {
    uint8_t zp_addr = fetch_byte();
    uint16_t addr = fetch_byte(zp_addr) + (fetch_byte(zp_addr + 1) << 8);
    A ^= RAM[addr];
    update_flags(A);
    cout << "EOR ($" << hex << (int)zp_addr << ")" << endl;
    print_registers();
}

// ASL implementations
void CPU65C02::ASL_ACC() {
    uint8_t old_carry = status & 0x01;
    status = (status & ~0x01) | (A & 0x80) >> 7;
    A = (A << 1) | old_carry;
    update_flags(A);
    cout << "ASL A" << endl;
    print_registers();
}

void CPU65C02::ASL_ZP() {
    uint8_t addr = fetch_byte();
    uint8_t old_carry = status & 0x01;
    status = (status & ~0x01) | (RAM[addr] & 0x80) >> 7;
    RAM[addr] = (RAM[addr] << 1) | old_carry;
    update_flags(RAM[addr]);
    cout << "ASL $" << hex << (int)addr << endl;
    print_registers();
}

void CPU65C02::ASL_ZP_X() {
    uint8_t addr = fetch_byte() + X;
    uint8_t old_carry = status & 0x01;
    status = (status & ~0x01) | (RAM[addr] & 0x80) >> 7;
    RAM[addr] = (RAM[addr] << 1) | old_carry;
    update_flags(RAM[addr]);
    cout << "ASL $" << hex << (int)addr << ",X" << endl;
    print_registers();
}

void CPU65C02::ASL_ABS() {
    uint16_t addr = fetch_word();
    uint8_t old_carry = status & 0x01;
    status = (status & ~0x01) | (RAM[addr] & 0x80) >> 7;
    RAM[addr] = (RAM[addr] << 1) | old_carry;
    update_flags(RAM[addr]);
    cout << "ASL $" << hex << setw(4) << setfill('0') << addr << endl;
    print_registers();
}

void CPU65C02::ASL_ABS_X() {
    uint16_t addr = fetch_word() + X;
    uint8_t old_carry = status & 0x01;
    status = (status & ~0x01) | (RAM[addr] & 0x80) >> 7;
    RAM[addr] = (RAM[addr] << 1) | old_carry;
    update_flags(RAM[addr]);
    cout << "ASL $" << hex << setw(4) << setfill('0') << addr << ",X" << endl;
    print_registers();
}

// LSR implementations
void CPU65C02::LSR_ACC() {
    uint8_t old_carry = status & 0x01;
    status = (status & ~0x01) | (A & 0x01);
    A = (A >> 1) | (old_carry << 7);
    update_flags(A);
    cout << "LSR A" << endl;
    print_registers();
}

void CPU65C02::LSR_ZP() {
    uint8_t addr = fetch_byte();
    uint8_t old_carry = status & 0x01;
    status = (status & ~0x01) | (RAM[addr] & 0x01);
    RAM[addr] = (RAM[addr] >> 1) | (old_carry << 7);
    update_flags(RAM[addr]);
    cout << "LSR $" << hex << (int)addr << endl;
    print_registers();
}

void CPU65C02::LSR_ZP_X() {
    uint8_t addr = fetch_byte() + X;
    uint8_t old_carry = status & 0x01;
    status = (status & ~0x01) | (RAM[addr] & 0x01);
    RAM[addr] = (RAM[addr] >> 1) | (old_carry << 7);
    update_flags(RAM[addr]);
    cout << "LSR $" << hex << (int)addr << ",X" << endl;
    print_registers();
}

void CPU65C02::LSR_ABS() {
    uint16_t addr = fetch_word();
    uint8_t old_carry = status & 0x01;
    status = (status & ~0x01) | (RAM[addr] & 0x01);
    RAM[addr] = (RAM[addr] >> 1) | (old_carry << 7);
    update_flags(RAM[addr]);
    cout << "LSR $" << hex << setw(4) << setfill('0') << addr << endl;
    print_registers();
}

void CPU65C02::LSR_ABS_X() {
    uint16_t addr = fetch_word() + X;
    uint8_t old_carry = status & 0x01;
    status = (status & ~0x01) | (RAM[addr] & 0x01);
    RAM[addr] = (RAM[addr] >> 1) | (old_carry << 7);
    update_flags(RAM[addr]);
    cout << "LSR $" << hex << setw(4) << setfill('0') << addr << ",X" << endl;
    print_registers();
}

// ROL implementations
void CPU65C02::ROL_ACC() {
    uint8_t old_carry = status & 0x01;
    status = (status & ~0x01) | (A & 0x80) >> 7;
    A = (A << 1) | old_carry;
    update_flags(A);
    cout << "ROL A" << endl;
    print_registers();
}

void CPU65C02::ROL_ZP() {
    uint8_t addr = fetch_byte();
    uint8_t old_carry = status & 0x01;
    status = (status & ~0x01) | (RAM[addr] & 0x80) >> 7;
    RAM[addr] = (RAM[addr] << 1) | old_carry;
    update_flags(RAM[addr]);
    cout << "ROL $" << hex << (int)addr << endl;
    print_registers();
}

void CPU65C02::ROL_ZP_X() {
    uint8_t addr = fetch_byte() + X;
    uint8_t old_carry = status & 0x01;
    status = (status & ~0x01) | (RAM[addr] & 0x80) >> 7;
    RAM[addr] = (RAM[addr] << 1) | old_carry;
    update_flags(RAM[addr]);
    cout << "ROL $" << hex << (int)addr << ",X" << endl;
    print_registers();
}

void CPU65C02::ROL_ABS() {
    uint16_t addr = fetch_word();
    uint8_t old_carry = status & 0x01;
    status = (status & ~0x01) | (RAM[addr] & 0x80) >> 7;
    RAM[addr] = (RAM[addr] << 1) | old_carry;
    update_flags(RAM[addr]);
    cout << "ROL $" << hex << setw(4) << setfill('0') << addr << endl;
    print_registers();
}

void CPU65C02::ROL_ABS_X() {
    uint16_t addr = fetch_word() + X;
    uint8_t old_carry = status & 0x01;
    status = (status & ~0x01) | (RAM[addr] & 0x80) >> 7;
    RAM[addr] = (RAM[addr] << 1) | old_carry;
    update_flags(RAM[addr]);
    cout << "ROL $" << hex << setw(4) << setfill('0') << addr << ",X" << endl;
    print_registers();
}

// ROR implementations
void CPU65C02::ROR_ACC() {
    uint8_t old_carry = status & 0x01;
    status = (status & ~0x01) | (A & 0x01);
    A = (A >> 1) | (old_carry << 7);
    update_flags(A);
    cout << "ROR A" << endl;
    print_registers();
}

void CPU65C02::ROR_ZP() {
    uint8_t addr = fetch_byte();
    uint8_t old_carry = status & 0x01;
    status = (status & ~0x01) | (RAM[addr] & 0x01);
    RAM[addr] = (RAM[addr] >> 1) | (old_carry << 7);
    update_flags(RAM[addr]);
    cout << "ROR $" << hex << (int)addr << endl;
    print_registers();
}

void CPU65C02::ROR_ZP_X() {
    uint8_t addr = fetch_byte() + X;
    uint8_t old_carry = status & 0x01;
    status = (status & ~0x01) | (RAM[addr] & 0x01);
    RAM[addr] = (RAM[addr] >> 1) | (old_carry << 7);
    update_flags(RAM[addr]);
    cout << "ROR $" << hex << (int)addr << ",X" << endl;
    print_registers();
}

void CPU65C02::ROR_ABS() {
    uint16_t addr = fetch_word();
    uint8_t old_carry = status & 0x01;
    status = (status & ~0x01) | (RAM[addr] & 0x01);
    RAM[addr] = (RAM[addr] >> 1) | (old_carry << 7);
    update_flags(RAM[addr]);
    cout << "ROR $" << hex << setw(4) << setfill('0') << addr << endl;
    print_registers();
}

void CPU65C02::ROR_ABS_X() {
    uint16_t addr = fetch_word() + X;
    uint8_t old_carry = status & 0x01;
    status = (status & ~0x01) | (RAM[addr] & 0x01);
    RAM[addr] = (RAM[addr] >> 1) | (old_carry << 7);
    update_flags(RAM[addr]);
    cout << "ROR $" << hex << setw(4) << setfill('0') << addr << ",X" << endl;
    print_registers();
}

// Stack helper functions
void CPU65C02::push(uint8_t value) {
    RAM[0x100 + S] = value;
    S--;
}

uint8_t CPU65C02::pull() {
    S++;
    return RAM[0x100 + S];
}

void CPU65C02::update_NZ_flags(uint8_t value) {
    // Update Zero flag
    if (value == 0) {
        status |= 0x02;  // Set Zero flag
    } else {
        status &= ~0x02; // Clear Zero flag
    }
    
    // Update Negative flag
    if (value & 0x80) {
        status |= 0x80;  // Set Negative flag
    } else {
        status &= ~0x80; // Clear Negative flag
    }
}

// Stack Operations
void CPU65C02::PHA() {
    push(A);
    cycles += 3;
    if (debug) cout << "PHA: Pushed A ($" << hex << (int)A << ") to stack" << endl;
}

void CPU65C02::PHP() {
    // Set B and U flags before pushing
    uint8_t status_to_push = P;
    status_to_push |= 0x30;  // Set B and U flags
    push(status_to_push);
    cycles += 3;
    if (debug) cout << "PHP: Pushed P ($" << hex << (int)status_to_push << ") to stack" << endl;
}

void CPU65C02::PLA() {
    A = pull();
    update_NZ_flags(A);
    cycles += 4;
    if (debug) cout << "PLA: Pulled $" << hex << (int)A << " from stack to A" << endl;
}

void CPU65C02::PLP() {
    P = pull();
    cycles += 4;
    if (debug) cout << "PLP: Pulled $" << hex << (int)P << " from stack to P" << endl;
}

void CPU65C02::TSX() {
    X = S;
    update_NZ_flags(X);
    cycles += 2;
    if (debug) cout << "TSX: Transferred SP ($" << hex << (int)S << ") to X" << endl;
}

void CPU65C02::TXS() {
    S = X;
    cycles += 2;
    if (debug) cout << "TXS: Transferred X ($" << hex << (int)X << ") to SP" << endl;
}

// Branch Instructions Implementation
void CPU65C02::BCC() {
    int8_t offset = fetch_byte();
    if (!(status & 0x01)) {  // Carry Clear
        PC += offset;
        cycles += 3;
        if (debug) cout << "BCC: Branch taken, new PC = $" << hex << (int)PC << endl;
    } else {
        cycles += 2;
        if (debug) cout << "BCC: Branch not taken" << endl;
    }
}

void CPU65C02::BCS() {
    int8_t offset = fetch_byte();
    if (status & 0x01) {  // Carry Set
        PC += offset;
        cycles += 3;
        if (debug) cout << "BCS: Branch taken, new PC = $" << hex << (int)PC << endl;
    } else {
        cycles += 2;
        if (debug) cout << "BCS: Branch not taken" << endl;
    }
}

void CPU65C02::BEQ() {
    int8_t offset = fetch_byte();
    if (status & 0x02) {  // Zero Set
        PC += offset;
        cycles += 3;
        if (debug) cout << "BEQ: Branch taken, new PC = $" << hex << (int)PC << endl;
    } else {
        cycles += 2;
        if (debug) cout << "BEQ: Branch not taken" << endl;
    }
}

void CPU65C02::BNE() {
    int8_t offset = fetch_byte();
    if (!(status & 0x02)) {  // Zero Clear
        PC += offset;
        cycles += 3;
        if (debug) cout << "BNE: Branch taken, new PC = $" << hex << (int)PC << endl;
    } else {
        cycles += 2;
        if (debug) cout << "BNE: Branch not taken" << endl;
    }
}

void CPU65C02::BMI() {
    int8_t offset = fetch_byte();
    if (status & 0x80) {  // Negative Set
        PC += offset;
        cycles += 3;
        if (debug) cout << "BMI: Branch taken, new PC = $" << hex << (int)PC << endl;
    } else {
        cycles += 2;
        if (debug) cout << "BMI: Branch not taken" << endl;
    }
}

void CPU65C02::BPL() {
    int8_t offset = fetch_byte();
    if (!(status & 0x80)) {  // Negative Clear
        PC += offset;
        cycles += 3;
        if (debug) cout << "BPL: Branch taken, new PC = $" << hex << (int)PC << endl;
    } else {
        cycles += 2;
        if (debug) cout << "BPL: Branch not taken" << endl;
    }
}

void CPU65C02::BVC() {
    int8_t offset = fetch_byte();
    if (!(status & 0x40)) {  // Overflow Clear
        PC += offset;
        cycles += 3;
        if (debug) cout << "BVC: Branch taken, new PC = $" << hex << (int)PC << endl;
    } else {
        cycles += 2;
        if (debug) cout << "BVC: Branch not taken" << endl;
    }
}

void CPU65C02::BVS() {
    int8_t offset = fetch_byte();
    if (status & 0x40) {  // Overflow Set
        PC += offset;
        cycles += 3;
        if (debug) cout << "BVS: Branch taken, new PC = $" << hex << (int)PC << endl;
    } else {
        cycles += 2;
        if (debug) cout << "BVS: Branch not taken" << endl;
    }
}

// Status Flag Operations Implementation
void CPU65C02::CLC() {
    status &= ~0x01;  // Clear Carry flag
    cycles += 2;
    if (debug) cout << "CLC: Cleared Carry flag" << endl;
}

void CPU65C02::SEC() {
    status |= 0x01;   // Set Carry flag
    cycles += 2;
    if (debug) cout << "SEC: Set Carry flag" << endl;
}

void CPU65C02::CLD() {
    status &= ~0x08;  // Clear Decimal mode flag
    cycles += 2;
    if (debug) cout << "CLD: Cleared Decimal mode flag" << endl;
}

void CPU65C02::SED() {
    status |= 0x08;   // Set Decimal mode flag
    cycles += 2;
    if (debug) cout << "SED: Set Decimal mode flag" << endl;
}

void CPU65C02::CLI() {
    status &= ~0x04;  // Clear Interrupt Disable flag
    cycles += 2;
    if (debug) cout << "CLI: Cleared Interrupt Disable flag" << endl;
}

void CPU65C02::SEI() {
    status |= 0x04;   // Set Interrupt Disable flag
    cycles += 2;
    if (debug) cout << "SEI: Set Interrupt Disable flag" << endl;
}

void CPU65C02::CLV() {
    status &= ~0x40;  // Clear Overflow flag
    cycles += 2;
    if (debug) cout << "CLV: Cleared Overflow flag" << endl;
}

// Comparison Operations Implementation
void CPU65C02::CMP_IMM() {
    uint8_t operand = fetch_byte();
    uint8_t result = A - operand;
    update_flags(result);
    status = (status & ~0x01) | (A >= operand); // Set carry if A >= operand
    cycles += 2;
    if (debug) cout << "CMP #$" << hex << (int)operand << endl;
}

void CPU65C02::CMP_ZP() {
    uint8_t addr = fetch_byte();
    uint8_t operand = RAM[addr];
    uint8_t result = A - operand;
    update_flags(result);
    status = (status & ~0x01) | (A >= operand);
    cycles += 3;
    if (debug) cout << "CMP $" << hex << (int)addr << endl;
}

void CPU65C02::CMP_ZP_X() {
    uint8_t addr = fetch_byte() + X;
    uint8_t operand = RAM[addr];
    uint8_t result = A - operand;
    update_flags(result);
    status = (status & ~0x01) | (A >= operand);
    cycles += 4;
    if (debug) cout << "CMP $" << hex << (int)addr << ",X" << endl;
}

void CPU65C02::CMP_ABS() {
    uint16_t addr = fetch_word();
    uint8_t operand = RAM[addr];
    uint8_t result = A - operand;
    update_flags(result);
    status = (status & ~0x01) | (A >= operand);
    cycles += 4;
    if (debug) cout << "CMP $" << hex << setw(4) << setfill('0') << addr << endl;
}

void CPU65C02::CMP_ABS_X() {
    uint16_t addr = fetch_word() + X;
    uint8_t operand = RAM[addr];
    uint8_t result = A - operand;
    update_flags(result);
    status = (status & ~0x01) | (A >= operand);
    cycles += 4;
    if (debug) cout << "CMP $" << hex << setw(4) << setfill('0') << addr << ",X" << endl;
}

void CPU65C02::CMP_ABS_Y() {
    uint16_t addr = fetch_word() + Y;
    uint8_t operand = RAM[addr];
    uint8_t result = A - operand;
    update_flags(result);
    status = (status & ~0x01) | (A >= operand);
    cycles += 4;
    if (debug) cout << "CMP $" << hex << setw(4) << setfill('0') << addr << ",Y" << endl;
}

void CPU65C02::CMP_PRE_IND_X() {
    uint8_t zp_addr = fetch_byte() + X;
    uint16_t addr = fetch_byte(zp_addr) + (fetch_byte(zp_addr + 1) << 8);
    uint8_t operand = RAM[addr];
    uint8_t result = A - operand;
    update_flags(result);
    status = (status & ~0x01) | (A >= operand);
    cycles += 6;
    if (debug) cout << "CMP ($" << hex << (int)zp_addr << ",X)" << endl;
}

void CPU65C02::CMP_POST_IND_Y() {
    uint8_t zp_addr = fetch_byte();
    uint16_t base = fetch_byte(zp_addr) + (fetch_byte(zp_addr + 1) << 8);
    uint8_t operand = RAM[base + Y];
    uint8_t result = A - operand;
    update_flags(result);
    status = (status & ~0x01) | (A >= operand);
    cycles += 5;
    if (debug) cout << "CMP ($" << hex << (int)zp_addr << "),Y" << endl;
}

void CPU65C02::CMP_IND() {
    uint8_t zp_addr = fetch_byte();
    uint16_t addr = fetch_byte(zp_addr) + (fetch_byte(zp_addr + 1) << 8);
    uint8_t operand = RAM[addr];
    uint8_t result = A - operand;
    update_flags(result);
    status = (status & ~0x01) | (A >= operand);
    cycles += 5;
    if (debug) cout << "CMP ($" << hex << (int)zp_addr << ")" << endl;
}

void CPU65C02::CPX_IMM() {
    uint8_t operand = fetch_byte();
    uint8_t result = X - operand;
    update_flags(result);
    status = (status & ~0x01) | (X >= operand);
    cycles += 2;
    if (debug) cout << "CPX #$" << hex << (int)operand << endl;
}

void CPU65C02::CPX_ZP() {
    uint8_t addr = fetch_byte();
    uint8_t operand = RAM[addr];
    uint8_t result = X - operand;
    update_flags(result);
    status = (status & ~0x01) | (X >= operand);
    cycles += 3;
    if (debug) cout << "CPX $" << hex << (int)addr << endl;
}

void CPU65C02::CPX_ABS() {
    uint16_t addr = fetch_word();
    uint8_t operand = RAM[addr];
    uint8_t result = X - operand;
    update_flags(result);
    status = (status & ~0x01) | (X >= operand);
    cycles += 4;
    if (debug) cout << "CPX $" << hex << setw(4) << setfill('0') << addr << endl;
}

void CPU65C02::CPY_IMM() {
    uint8_t operand = fetch_byte();
    uint8_t result = Y - operand;
    update_flags(result);
    status = (status & ~0x01) | (Y >= operand);
    cycles += 2;
    if (debug) cout << "CPY #$" << hex << (int)operand << endl;
}

void CPU65C02::CPY_ZP() {
    uint8_t addr = fetch_byte();
    uint8_t operand = RAM[addr];
    uint8_t result = Y - operand;
    update_flags(result);
    status = (status & ~0x01) | (Y >= operand);
    cycles += 3;
    if (debug) cout << "CPY $" << hex << (int)addr << endl;
}

void CPU65C02::CPY_ABS() {
    uint16_t addr = fetch_word();
    uint8_t operand = RAM[addr];
    uint8_t result = Y - operand;
    update_flags(result);
    status = (status & ~0x01) | (Y >= operand);
    cycles += 4;
    if (debug) cout << "CPY $" << hex << setw(4) << setfill('0') << addr << endl;
}

// Additional 65C02-specific instructions Implementation
void CPU65C02::BRA() {
    int8_t offset = fetch_byte();
    PC += offset;
    cycles += 3;
    if (debug) cout << "BRA: Branch taken, new PC = $" << hex << (int)PC << endl;
}

void CPU65C02::PHX() {
    push(X);
    cycles += 3;
    if (debug) cout << "PHX: Pushed X ($" << hex << (int)X << ") to stack" << endl;
}

void CPU65C02::PHY() {
    push(Y);
    cycles += 3;
    if (debug) cout << "PHY: Pushed Y ($" << hex << (int)Y << ") to stack" << endl;
}

void CPU65C02::PLX() {
    X = pull();
    update_NZ_flags(X);
    cycles += 4;
    if (debug) cout << "PLX: Pulled $" << hex << (int)X << " from stack to X" << endl;
}

void CPU65C02::PLY() {
    Y = pull();
    update_NZ_flags(Y);
    cycles += 4;
    if (debug) cout << "PLY: Pulled $" << hex << (int)Y << " from stack to Y" << endl;
}

void CPU65C02::STZ_ZP() {
    uint8_t addr = fetch_byte();
    RAM[addr] = 0;
    cycles += 3;
    if (debug) cout << "STZ $" << hex << (int)addr << endl;
}

void CPU65C02::STZ_ZP_X() {
    uint8_t addr = fetch_byte() + X;
    RAM[addr] = 0;
    cycles += 4;
    if (debug) cout << "STZ $" << hex << (int)addr << ",X" << endl;
}

void CPU65C02::STZ_ABS() {
    uint16_t addr = fetch_word();
    RAM[addr] = 0;
    cycles += 4;
    if (debug) cout << "STZ $" << hex << setw(4) << setfill('0') << addr << endl;
}

void CPU65C02::STZ_ABS_X() {
    uint16_t addr = fetch_word() + X;
    RAM[addr] = 0;
    cycles += 5;
    if (debug) cout << "STZ $" << hex << setw(4) << setfill('0') << addr << ",X" << endl;
}

void CPU65C02::TRB_ZP() {
    uint8_t addr = fetch_byte();
    uint8_t operand = RAM[addr];
    uint8_t result = operand & ~A;  // Reset bits that are set in A
    RAM[addr] = result;
    update_NZ_flags(result);
    cycles += 5;
    if (debug) cout << "TRB $" << hex << (int)addr << endl;
}

void CPU65C02::TRB_ABS() {
    uint16_t addr = fetch_word();
    uint8_t operand = RAM[addr];
    uint8_t result = operand & ~A;  // Reset bits that are set in A
    RAM[addr] = result;
    update_NZ_flags(result);
    cycles += 6;
    if (debug) cout << "TRB $" << hex << setw(4) << setfill('0') << addr << endl;
}

void CPU65C02::TSB_ZP() {
    uint8_t addr = fetch_byte();
    uint8_t operand = RAM[addr];
    uint8_t result = operand | A;  // Set bits that are set in A
    RAM[addr] = result;
    update_NZ_flags(result);
    cycles += 5;
    if (debug) cout << "TSB $" << hex << (int)addr << endl;
}

void CPU65C02::TSB_ABS() {
    uint16_t addr = fetch_word();
    uint8_t operand = RAM[addr];
    uint8_t result = operand | A;  // Set bits that are set in A
    RAM[addr] = result;
    update_NZ_flags(result);
    cycles += 6;
    if (debug) cout << "TSB $" << hex << setw(4) << setfill('0') << addr << endl;
} 
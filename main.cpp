#include <iostream>
#include <algorithm>
#include <cstdio>
#include <iomanip>
// #define SINGLE_STEP
// #define DEBUG
using namespace std;

// make a 6502CPU emulator
class CPU {
private:
    uint8_t A, X, Y, S, P; // 8-bit registers // S is the stack pointer register
    uint16_t PC; // 16-bit address counter
    uint8_t status; // 8-bit status register
    uint8_t RAM[65536]; // 64KB of RAM, 16 bits address
    typedef void (CPU::*OpCodeFn)();
    OpCodeFn opcode_table[256];

    uint8_t fetch_byte() {
        return RAM[PC++];
    }

    uint16_t fetch_word() {
        uint16_t low_byte = fetch_byte();
        uint16_t high_byte = fetch_byte();
        return (high_byte << 8) | low_byte;  // 6502 is little-endian
    }

    void update_flags(uint8_t value) {
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

public:
    CPU() {
        reset();
        // Simple instruction set with real opcodes
        opcode_table[0xA9] = &CPU::LDA_IMM;   // LDA Immediate
        opcode_table[0xB9] = &CPU::LDA_ABS_Y; // LDA Absolute, Y, explain: 
        opcode_table[0xA1] = &CPU::LDA_IND_X; // LDA Indirect, X, explain: 
        opcode_table[0xB1] = &CPU::LDA_IND_Y; // LDA Indirect, Y, explain: 
        opcode_table[0xB2] = &CPU::LDA_IND; // LDA Indirect
        opcode_table[0xA5] = &CPU::LDA_ZP; // LDA Zero Page
        opcode_table[0xB5] = &CPU::LDA_ZP_X; // LDA Zero Page, X
        opcode_table[0xAD] = &CPU::LDA_ABS; // LDA Absolute
        opcode_table[0xBD] = &CPU::LDA_ABS_X; // LDA Absolute, X

        opcode_table[0xA2] = &CPU::LDX_IMM;  // LDX Immediate
        opcode_table[0xA6] = &CPU::LDX_ZP;  // LDX Zero Page
        opcode_table[0xB6] = &CPU::LDX_ZP_Y;  // LDX Zero Page, Y
        opcode_table[0xAE] = &CPU::LDX_ABS;  // LDX Absolute, X
        opcode_table[0xBE] = &CPU::LDX_ABS_Y;  // LDX Absolute, Y

        opcode_table[0xA0] = &CPU::LDY_IMM;  // LDY Immediate
        opcode_table[0xA4] = &CPU::LDY_ZP;  // LDY Zero Page
        opcode_table[0xAC] = &CPU::LDY_ABS;  // LDY Absolute
        opcode_table[0xBC] = &CPU::LDY_ABS_Y;  // LDY Absolute, Y

        opcode_table[0x85] = &CPU::STA;  // STA Zero Page
        opcode_table[0x86] = &CPU::STX;  // STX Zero Page
        opcode_table[0x84] = &CPU::STY;  // STY Zero Page
        opcode_table[0x4C] = &CPU::JMP;  // JMP Absolute
        opcode_table[0x00] = &CPU::BRK;  // BRK
        opcode_table[0xEA] = &CPU::NOP;  // NOP
        opcode_table[0x40] = &CPU::RTI;  // RTI
    }

    // Basic instructions
    void LDA_ZP() {
        uint8_t addr = fetch_byte();
        A = RAM[addr];
        update_flags(A);
        cout << "LDA $" << hex << (int)addr << endl;
        print_registers();
    }
    void LDA_IMM() {
        A = fetch_byte();
        update_flags(A);
        cout << "LDA #$" << hex << (int)A << endl;
        print_registers();
    }
    void LDX_IMM() {
        X = fetch_byte();
        update_flags(X);
        cout << "LDX #$" << hex << (int)X << endl;
        print_registers();
    }
    void LDY() {
        Y = fetch_byte();
        update_flags(Y);
        cout << "LDY #$" << hex << (int)Y << endl;
    }
    void STX() {
        uint8_t addr = fetch_byte();
        RAM[addr] = X;
        cout << "STX $" << hex << (int)addr << endl;
    }
    void STY() {
        uint8_t addr = fetch_byte();
        RAM[addr] = Y;
        cout << "STY $" << hex << (int)addr << endl;
    }
    void STA() {
        uint8_t addr = fetch_byte();
        RAM[addr] = A;
        cout << "STA $" << hex << (int)addr << endl;
    }
    void JMP() {
        PC = fetch_byte();
        cout << "JMP $" << hex << (int)PC << endl;
    }
    void BRK() {
        cout << "BRK" << endl;
        PC = 0;
    }
    void NOP() {
        cout << "NOP" << endl;
    }
    void RTI() {
        cout << "RTI" << endl;
        PC = 0;
    }
    // Rest of the code (reset, load_program, execute) remains the same
    void input() {
        cout << "\nPress Enter to continue...";
        cin.get();
    }
    void reset() {
        A = 0;
        X = 0;
        Y = 0;
        P = 0;
        S = 0;
        PC = 0;
        status = 0;
    }
    void load_program(uint8_t* program, size_t size) {
        memcpy(RAM, program, size);
    }
    void execute() {
        while (PC < 65535) {
            #ifdef DEBUG
                cout << "PC: " << hex << (int)PC << endl;
            #endif
            #ifdef SINGLE_STEP
                input();
            #endif  
            (this->*opcode_table[RAM[PC++]])();
            
        }
    }

    // LDA Absolute
    void LDA_ABS() {
        uint16_t addr = fetch_word();
        A = RAM[addr];
        update_flags(A);
        cout << "LDA $" << hex << setw(4) << setfill('0') << addr << endl;
        print_registers();
    }

    void print_registers() {
        cout << "A: $" << hex << (int)A << ", X: $" << (int)X << ", Y: $" << (int)Y << ", P: $" << (int)P << ", S: $" << (int)S << ", PC: $" << PC << endl;
    }
};

int main() {
    cout << hex << uppercase;
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    CPU cpu;
    
    // Simple test program
    uint8_t program[] = {
        0xA9, 0x42,    // LDA #$42
        0x85, 0x20,    // STA $20
        0xA0, 0x55,    // LDY #$55
        0x84, 0x21,    // STY $21
        0x00           // BRK
    };

    cpu.load_program(program, sizeof(program));

    cpu.execute();

    return 0;
}
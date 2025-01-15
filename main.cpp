#include <iostream>
#include <algorithm>
#include <cstdio>
using namespace std;

// make a 6502CPU emulator
class CPU {
private:
    uint8_t A, X, Y, S, P; // 8-bit registers // S is the stack pointer register
    uint16_t PC; // 16-bit address counter
    uint8_t status; // 8-bit status register
    uint8_t RAM[65535]; // 64KB of RAM, 16 bits address
    typedef void (CPU::*OpCodeFn)();
    OpCodeFn opcode_table[256];

    uint8_t fetch_byte() {
        return RAM[PC++];
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
        opcode_table[0xA9] = &CPU::LDA;  // LDA Immediate
        opcode_table[0x85] = &CPU::STA;  // STA Zero Page
        opcode_table[0x4C] = &CPU::JMP;  // JMP Absolute
        opcode_table[0x00] = &CPU::BRK;  // BRK
        opcode_table[0xEA] = &CPU::NOP;  // NOP
        opcode_table[0x40] = &CPU::RTI;  // RTI
    }

    // Basic instructions
    void LDA() {
        A = fetch_byte();
        update_flags(A);
        cout << "LDA #$" << hex << (int)A << endl;
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
            (this->*opcode_table[RAM[PC++]])();
        }
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
        0xA9, 0x55,    // LDA #$55
        0x85, 0x21,    // STA $21
        0x00           // BRK
    };

    cpu.load_program(program, sizeof(program));

    cpu.execute();

    return 0;
}
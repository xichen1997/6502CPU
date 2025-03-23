#include "CPU65C02.h"
#include <iostream>
#include <iomanip>

using namespace std;

int main() {
    cout << "Hello, World!" << endl;
    // cout << hex << uppercase;
    // cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    CPU65C02 cpu;
    
    // Test program
    uint8_t program[] = {
        0xA9, 0x42,       // LDA #$42
        0x69, 0x37,       // ADC #$37 (should result in $79)
        0x85, 0x20,       // STA $20
        0xE9, 0x15,       // SBC #$15 (should result in $64)
        0x85, 0x21,       // STA $21
        
        0xA2, 0x05,       // LDX #$05
        0xE8,             // INX (should be $06)
        0x86, 0x22,       // STX $22
        
        0xA0, 0x0A,       // LDY #$0A
        0x88,             // DEY (should be $09)
        0x84, 0x23,       // STY $23
        
        0xE6, 0x20,       // INC $20 (should be $7A)
        0xC6, 0x21,       // DEC $21 (should be $63)
        
        0x00              // BRK
    };

    cpu.load_program(program, sizeof(program));
    cpu.execute();
    return 0;
}
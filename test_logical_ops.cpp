#include "CPU65C02.h"
#include <iostream>
#include <iomanip>
#include <cassert>

using namespace std;

void print_test_header(const char* test_name) {
    cout << "\n=== Testing " << test_name << " ===\n";
}

void print_test_result(bool passed) {
    cout << (passed ? "PASSED" : "FAILED") << endl;
}

// Helper function to load a program into CPU
void load_program(CPU65C02& cpu, const uint8_t* program, size_t size) {
    uint8_t* program_copy = new uint8_t[size];
    memcpy(program_copy, program, size);
    cpu.load_program(program_copy, size);
    delete[] program_copy;
}

// Test AND operations
void test_and_operations() {
    print_test_header("AND Operations");
    
    CPU65C02 cpu;
    
    // Test AND_IMM
    {
        uint8_t program[] = {
            0xA9, 0x0F,  // LDA #$0F
            0x29, 0xF0,  // AND #$F0
            0x00        // BRK
        };
        load_program(cpu, program, sizeof(program));
        cpu.reset();
        cpu.execute();
        print_test_result(cpu.get_A() == 0x00);
    }
    
    // Test AND_ZP
    {
        uint8_t program[] = {
            0xA9, 0xFF,  // LDA #$FF
            0x85, 0x00,  // STA $00
            0xA9, 0x0F,  // LDA #$0F
            0x25, 0x00,  // AND $00
            0x00        // BRK
        };
        load_program(cpu, program, sizeof(program));
        cpu.reset();
        cpu.execute();
        print_test_result(cpu.get_A() == 0x0F);
    }
}

// Test ORA operations
void test_ora_operations() {
    print_test_header("ORA Operations");
    
    CPU65C02 cpu;
    
    // Test ORA_IMM
    {
        uint8_t program[] = {
            0xA9, 0x0F,  // LDA #$0F
            0x09, 0xF0,  // ORA #$F0
            0x00        // BRK
        };
        load_program(cpu, program, sizeof(program));
        cpu.reset();
        cpu.execute();
        print_test_result(cpu.get_A() == 0xFF);
    }
    
    // Test ORA_ZP
    {
        uint8_t program[] = {
            0xA9, 0x0F,  // LDA #$0F
            0x85, 0x00,  // STA $00
            0xA9, 0xF0,  // LDA #$F0
            0x05, 0x00,  // ORA $00
            0x00        // BRK
        };
        load_program(cpu, program, sizeof(program));
        cpu.reset();
        cpu.execute();
        print_test_result(cpu.get_A() == 0xFF);
    }
}

// Test EOR operations
void test_eor_operations() {
    print_test_header("EOR Operations");
    
    CPU65C02 cpu;
    
    // Test EOR_IMM
    {
        uint8_t program[] = {
            0xA9, 0x0F,  // LDA #$0F
            0x49, 0xF0,  // EOR #$F0
            0x00        // BRK
        };
        load_program(cpu, program, sizeof(program));
        cpu.reset();
        cpu.execute();
        print_test_result(cpu.get_A() == 0xFF);
    }
    
    // Test EOR_ZP
    {
        uint8_t program[] = {
            0xA9, 0x0F,  // LDA #$0F
            0x85, 0x00,  // STA $00
            0xA9, 0xF0,  // LDA #$F0
            0x45, 0x00,  // EOR $00
            0x00        // BRK
        };
        load_program(cpu, program, sizeof(program));
        cpu.reset();
        cpu.execute();
        print_test_result(cpu.get_A() == 0xFF);
    }
}

// Test ASL operations
void test_asl_operations() {
    print_test_header("ASL Operations");
    
    CPU65C02 cpu;
    
    // Test ASL_ACC
    {
        uint8_t program[] = {
            0xA9, 0x42,  // LDA #$42
            0x0A,       // ASL A
            0x00        // BRK
        };
        load_program(cpu, program, sizeof(program));
        cpu.reset();
        cpu.execute();
        print_test_result(cpu.get_A() == 0x84);
    }
    
    // Test ASL_ZP
    {
        uint8_t program[] = {
            0xA9, 0x42,  // LDA #$42
            0x85, 0x00,  // STA $00
            0x06, 0x00,  // ASL $00
            0xA5, 0x00,  // LDA $00
            0x00        // BRK
        };
        load_program(cpu, program, sizeof(program));
        cpu.reset();
        cpu.execute();
        print_test_result(cpu.get_A() == 0x84);
    }
}

// Test LSR operations
void test_lsr_operations() {
    print_test_header("LSR Operations");
    
    CPU65C02 cpu;
    
    // Test LSR_ACC
    {
        uint8_t program[] = {
            0xA9, 0x84,  // LDA #$84
            0x4A,       // LSR A
            0x00        // BRK
        };
        load_program(cpu, program, sizeof(program));
        cpu.reset();
        cpu.execute();
        print_test_result(cpu.get_A() == 0x42);
    }
    
    // Test LSR_ZP
    {
        uint8_t program[] = {
            0xA9, 0x84,  // LDA #$84
            0x85, 0x00,  // STA $00
            0x46, 0x00,  // LSR $00
            0xA5, 0x00,  // LDA $00
            0x00        // BRK
        };
        load_program(cpu, program, sizeof(program));
        cpu.reset();
        cpu.execute();
        print_test_result(cpu.get_A() == 0x42);
    }
}

// Test ROL operations
void test_rol_operations() {
    print_test_header("ROL Operations");
    
    CPU65C02 cpu;
    
    // Test ROL_ACC
    {
        uint8_t program[] = {
            0xA9, 0x42,  // LDA #$42
            0x2A,       // ROL A
            0x00        // BRK
        };
        load_program(cpu, program, sizeof(program));
        cpu.reset();
        cpu.execute();
        print_test_result(cpu.get_A() == 0x84);
    }
    
    // Test ROL_ZP
    {
        uint8_t program[] = {
            0xA9, 0x42,  // LDA #$42
            0x85, 0x00,  // STA $00
            0x26, 0x00,  // ROL $00
            0xA5, 0x00,  // LDA $00
            0x00        // BRK
        };
        load_program(cpu, program, sizeof(program));
        cpu.reset();
        cpu.execute();
        print_test_result(cpu.get_A() == 0x84);
    }
}

// Test ROR operations
void test_ror_operations() {
    print_test_header("ROR Operations");
    
    CPU65C02 cpu;
    
    // Test ROR_ACC
    {
        uint8_t program[] = {
            0xA9, 0x84,  // LDA #$84
            0x6A,       // ROR A
            0x00        // BRK
        };
        load_program(cpu, program, sizeof(program));
        cpu.reset();
        cpu.execute();
        print_test_result(cpu.get_A() == 0x42);
    }
    
    // Test ROR_ZP
    {
        uint8_t program[] = {
            0xA9, 0x84,  // LDA #$84
            0x85, 0x00,  // STA $00
            0x66, 0x00,  // ROR $00
            0xA5, 0x00,  // LDA $00
            0x00        // BRK
        };
        load_program(cpu, program, sizeof(program));
        cpu.reset();
        cpu.execute();
        print_test_result(cpu.get_A() == 0x42);
    }
}

int main() {
    cout << "Starting Logical Operations Tests\n";
    
    test_and_operations();
    test_ora_operations();
    test_eor_operations();
    test_asl_operations();
    test_lsr_operations();
    test_rol_operations();
    test_ror_operations();
    
    cout << "\nAll tests completed.\n";
    return 0;
} 
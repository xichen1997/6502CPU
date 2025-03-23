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

// Test PHA/PLA operations
void test_pha_pla_operations() {
    print_test_header("PHA/PLA Operations");
    
    CPU65C02 cpu;
    
    // Test PHA followed by PLA
    {
        uint8_t program[] = {
            0xA9, 0x42,  // LDA #$42
            0x48,       // PHA
            0xA9, 0x00,  // LDA #$00 (clear A)
            0x68,       // PLA
            0x00        // BRK
        };
        load_program(cpu, program, sizeof(program));
        cpu.reset();
        cpu.execute();
        print_test_result(cpu.get_A() == 0x42);
    }
}

// Test PHP/PLP operations
void test_php_plp_operations() {
    print_test_header("PHP/PLP Operations");
    
    CPU65C02 cpu;
    
    // Test PHP followed by PLP
    {
        uint8_t program[] = {
            0x38,       // SEC (set carry flag)
            0x08,       // PHP
            0x18,       // CLC (clear carry flag)
            0x28,       // PLP
            0x00        // BRK
        };
        load_program(cpu, program, sizeof(program));
        cpu.reset();
        cpu.execute();
        print_test_result(cpu.get_P() & 0x01);  // Check if carry flag is set
    }
}

// Test TSX/TXS operations
void test_tsx_txs_operations() {
    print_test_header("TSX/TXS Operations");
    
    CPU65C02 cpu;
    
    // Test TSX
    {
        uint8_t program[] = {
            0xBA,       // TSX
            0x00        // BRK
        };
        load_program(cpu, program, sizeof(program));
        cpu.reset();
        cpu.execute();
        print_test_result(cpu.get_X() == 0xFF);  // Initial SP value
    }
    
    // Test TXS
    {
        uint8_t program[] = {
            0xA2, 0x42,  // LDX #$42
            0x9A,       // TXS
            0xBA,       // TSX (to verify)
            0x00        // BRK
        };
        load_program(cpu, program, sizeof(program));
        cpu.reset();
        cpu.execute();
        print_test_result(cpu.get_X() == 0x42);
    }
}

// Test stack operations with multiple pushes and pulls
void test_stack_sequence() {
    print_test_header("Stack Sequence Operations");
    
    CPU65C02 cpu;
    
    // Test multiple pushes and pulls
    {
        uint8_t program[] = {
            0xA9, 0x42,  // LDA #$42
            0x48,       // PHA
            0xA9, 0x84,  // LDA #$84
            0x48,       // PHA
            0x68,       // PLA (should get 0x84)
            0x68,       // PLA (should get 0x42)
            0x00        // BRK
        };
        load_program(cpu, program, sizeof(program));
        cpu.reset();
        cpu.execute();
        print_test_result(cpu.get_A() == 0x42);
    }
}

int main() {
    cout << "Starting Stack Operations Tests\n";
    
    test_pha_pla_operations();
    test_php_plp_operations();
    test_tsx_txs_operations();
    test_stack_sequence();
    
    cout << "\nAll tests completed.\n";
    return 0;
} 
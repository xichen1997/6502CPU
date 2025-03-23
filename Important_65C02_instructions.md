Here are the important 65C02 instructions that we should add to make the emulator more complete:

1. Arithmetic Operations:
- `ADC` (Add with Carry)
- `SBC` (Subtract with Carry)
- `INC` (Increment Memory)
- `INX` (Increment X Register)
- `INY` (Increment Y Register)
- `DEC` (Decrement Memory)
- `DEX` (Decrement X Register)
- `DEY` (Decrement Y Register)

2. Logical Operations:
- `AND` (Logical AND)
- `ORA` (Logical OR)
- `EOR` (Exclusive OR)
- `ASL` (Arithmetic Shift Left)
- `LSR` (Logical Shift Right)
- `ROL` (Rotate Left)
- `ROR` (Rotate Right)

3. Stack Operations:
- `PHA` (Push Accumulator)
- `PHP` (Push Processor Status)
- `PLA` (Pull Accumulator)
- `PLP` (Pull Processor Status)
- `TSX` (Transfer Stack Pointer to X)
- `TXS` (Transfer X to Stack Pointer)

4. Branch Instructions:
- `BCC` (Branch if Carry Clear)
- `BCS` (Branch if Carry Set)
- `BEQ` (Branch if Equal)
- `BNE` (Branch if Not Equal)
- `BMI` (Branch if Minus)
- `BPL` (Branch if Plus)
- `BVC` (Branch if Overflow Clear)
- `BVS` (Branch if Overflow Set)

5. Status Flag Operations:
- `CLC` (Clear Carry Flag)
- `SEC` (Set Carry Flag)
- `CLD` (Clear Decimal Mode)
- `SED` (Set Decimal Mode)
- `CLI` (Clear Interrupt Disable)
- `SEI` (Set Interrupt Disable)
- `CLV` (Clear Overflow Flag)

6. Comparison Operations:
- `CMP` (Compare with Accumulator)
- `CPX` (Compare with X Register)
- `CPY` (Compare with Y Register)

7. Additional 65C02-specific instructions:
- `BRA` (Branch Always)
- `PHX` (Push X Register)
- `PHY` (Push Y Register)
- `PLX` (Pull X Register)
- `PLY` (Pull Y Register)
- `STZ` (Store Zero)
- `TRB` (Test and Reset Bits)
- `TSB` (Test and Set Bits)

Would you like me to implement these instructions? We can add them in groups, starting with the most fundamental ones first. Which group would you like to implement first?

The implementation will involve:
1. Adding function declarations to the header file
2. Adding opcode mappings in the constructor
3. Implementing each instruction function
4. Updating the status flags appropriately
5. Adding debug prints for each instruction

Let me know which group of instructions you'd like to implement first, and I'll help you add them to the code.

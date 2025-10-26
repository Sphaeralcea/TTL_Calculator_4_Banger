# TTL_Calculator_4_Banger
A 4-function calculator implemented in 74hcXX chips

Add, subtract, multiply, and divide
8 numeric digits plus an Error and sign digit
    Max num: +99,999,999 to -99,999,999
    Min Num:  +0.0000001 to -0.0000001
Floating decimal point
Signals error on overflow for all operations, keyboard input overflow, and divide by 0 
Leading zero blanking
All 74hcXX logic implementation (plus 2 4XXX) chips
3 major logic blocks:
    SEQ:  Micro-sequencer engine with a 2816 EEPROM microprogram store
        24-bit micro-instruction word
        Unconditional branch, 16 conditional branches, 1 NMI-type interrupt
        Single level loop counter (no nesting)
        Clock and reset generation
    DSKY:  Keyboard and Display unit
        9-digit LED display sequencing, 20-key keyboard encoding (3 keys not used)
    REG:  Register-Arithmetic unit
        1 digit BCD adder/subtractor
        3, 8-digit registers (4-bits X 8)
        Exponent calculator (counters), 8 status and control register bits, operation progress tracking state machine
Architectural Deficits (intentional)
    No rounding logic
    No guard digits
Architectural Bugs (unintentional)
    Accuracy loss dividing 2 8-digit integers
    Underflow condition in addition not accounted for  
<img width="1127" height="893" alt="image" src="https://github.com/user-attachments/assets/270bdc8d-0353-4b5a-8da9-f5fc1efa8b27" />

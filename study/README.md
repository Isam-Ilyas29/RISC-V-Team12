# Team 12 single-cycle CPU study pack

The hardware reference is [Team 12 revision 407b67f](https://github.com/Isam-Ilyas29/RISC-V-Team12/tree/407b67fde9f2f18782142810234341de85573590/rtl).

## Flashcards

Import **Team12.apkg** into Anki. Expand **Team12 CPU** and work through the four subdecks in order. Each answer links to the relevant source file.

## Issie

Download the project folder and open **Team12/Team12.dprj** in the [Issie desktop application](https://tomcl.github.io/issie/index.html). Keep the seven .dgm files together. Open **CPU** and use step simulation. Set Reset to 1 for one clock edge, then return it to 0. The PC now reads BFC00000. Select hexadecimal display to match the values below.

| PC offset from reset | Instruction | What to watch after stepping |
|---|---|---|
| 0x00 | addi x6,x0,255 | x6 becomes 255 |
| 0x04 | addi a0,x0,0 | a0 becomes zero |
| 0x08 | addi x11,x0,0 | The counter starts at zero |
| 0x0c | addi a0,x11,0 | The current counter appears on a0 |
| 0x10 | addi x11,x11,1 | The next counter value is ready |
| 0x14 | bne x11,x6,-8 | PC returns to offset 0x0c until x11 reaches 255 |

At offset 0x18, bne x6,x0,-16 restarts the counter at offset 0x08. The highest displayed count in each pass is **254**. The copy to a0 happens before the increment and branch. In the waveform viewer, select PC, Instruction, WritebackData and A0. Check the destination register and its enable before treating a writeback value as a committed write.

| Sheet | What to inspect |
|---|---|
| CPU | Fetch, reset, PC+4, relative and indirect targets, memory-mapped trigger and writeback muxes |
| Decode | A lookup ROM represents the control unit case statements. Address bits 6:0 are opcode, 9:7 are funct3, and 10 is instruction bit 30. The labelled outputs unpack its control word. |
| Registers | Two read muxes, one enabled register per writable destination, constant x0 and the x10 tap |
| Immediate | Follow instruction slices into each I, B, S, J or U immediate |
| Compare | Compare sign bits and the sign of the difference to distinguish signed and unsigned ordering |
| ALU | Follow both operands into each arithmetic, logic or shift operation and the result selector |
| DataMemory | Four 32 KiB byte banks implement adjacent-byte reads and partial writes. Follow the low address bits to see byte order. |

Issie supplies the clock through its step controls. The model uses four read copies of the same 512-byte instruction ROM for fetch and starts data RAM at zero. To change the program, update all four ROM copies with identical byte data. Reset suppresses writes and resets the PC. The model expresses the architectural operations with native Issie components. Its banked RAM wraps individual byte accesses at the memory boundary, so out-of-range and unknown-value RTL behaviour is outside the model's scope.

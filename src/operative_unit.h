#pragma once

#include "alu.h"
#include "instruction_memory.h"
#include "registers.h"
#include <sysc/kernel/sc_module.h>
#include <systemc.h>

SC_MODULE(OperativeUnit) {
    ALU *alu;
    Registers *registers;
    InstructionMemory *instruction_memory;
    DataMemory *data_memory;

    sc_out<sc_uint<6>> opcode;
    sc_signal<sc_uint<5>> rs, rt, rd;
    sc_signal<sc_uint<8>> address;
    sc_signal<sc_uint<8>> pc;

    sc_signal<sc_uint<32>> fetched_instruction;

    SC_CTOR(OperativeUnit) {
        alu = new ALU("ALU");
        registers = new Registers("Registers");
        instruction_memory = new InstructionMemory("InstructionMemory");
        data_memory = new DataMemory("DataMemory");

        instruction_memory->fetched_instruction(fetched_instruction);

        registers->rs(rs);
        registers->rt(rt);
        registers->rd(rd);

        pc = 0;

        SC_METHOD(decode);
        sensitive << fetched_instruction;
    }

    void decode() {
        opcode = fetched_instruction.read().range(31, 26);
        rs = fetched_instruction.read().range(25, 21);
        rt = fetched_instruction.read().range(20, 16);
        rd = fetched_instruction.read().range(15, 11);
        address = fetched_instruction.read().range(7, 0);
    }
};

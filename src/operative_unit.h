#pragma once

#include "alu.h"
#include "data_memory.h"
#include "instruction_memory.h"
#include "pc.h"
#include "registers.h"
#include <sysc/kernel/sc_module.h>
#include <systemc.h>

SC_MODULE(OperativeUnit) {
    sc_in<sc_uint<32>> fetched_instruction;

    sc_out<sc_uint<6>> opcode;
    sc_out<sc_uint<5>> rs, rt, rd;
    sc_out<sc_uint<8>> address;

    ALU *alu;
    Registers *registers;
    InstructionMemory *instruction_memory;
    DataMemory *data_memory;
    PC *pc;

    SC_CTOR(OperativeUnit) {
        alu = new ALU("ALU");
        registers = new Registers("Registers");
        instruction_memory = new InstructionMemory("InstructionMemory");
        data_memory = new DataMemory("DataMemory");
        pc = new PC("PC");

        SC_METHOD(decode);
        sensitive << fetched_instruction;
    }

    ~OperativeUnit() {
        delete alu;
        delete registers;
        delete instruction_memory;
        delete data_memory;
        delete pc;
    }

    void decode() {
        opcode = fetched_instruction.read().range(31, 26);
        rs = fetched_instruction.read().range(25, 21);
        rt = fetched_instruction.read().range(20, 16);
        rd = fetched_instruction.read().range(15, 11);
        address = fetched_instruction.read().range(7, 0);
    }
};

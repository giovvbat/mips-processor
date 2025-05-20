#pragma once

#include <sysc/kernel/sc_module.h>
#include <systemc.h>
#include "opcode.h"

SC_MODULE(InstructionMemory) {
    sc_in<sc_uint<8>> address;
    sc_in<bool> inst_mem_read;

    sc_out<sc_uint<32>> fetched_instruction;

    sc_uint<32> memory[256];

    SC_CTOR(InstructionMemory) {
        SC_METHOD(read);
        sensitive << inst_mem_read.pos();
        dont_initialize();
    }

    void read() {
        fetched_instruction.write(memory[address.read()]);
    }
};

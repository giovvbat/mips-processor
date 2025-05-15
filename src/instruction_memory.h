#pragma once

#include <sysc/kernel/sc_module.h>
#include <systemc.h>

SC_MODULE(InstructionMemory) {
    sc_signal<sc_uint<32>> memory[256];
    sc_in<sc_uint<8>> pc;
    sc_in<bool> inst_mem_read;
    sc_out<sc_uint<32>> fetched_instruction;

    SC_CTOR(InstructionMemory) {
        SC_METHOD(read);
        sensitive << inst_mem_read.pos();
    }

    void read() {
        fetched_instruction = memory[pc.read()];
    }
};

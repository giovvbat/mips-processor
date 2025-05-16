#pragma once

#include <systemc.h>
#include <sysc/kernel/sc_module.h>

SC_MODULE(DataMemory) {
    sc_in<bool> data_mem_read;
    sc_in<bool> data_mem_write;
    sc_in<sc_uint<8>> address;
    sc_in<sc_uint<32>> write_data;

    sc_signal<sc_uint<32>> mem_data;

    sc_uint<32> data_memory[256];

    SC_CTOR(DataMemory) {
        for (int i = 0; i < 256; i++) {
            data_memory[i] = 0;
        }

        SC_METHOD(read);
        sensitive << data_mem_read.pos();

        SC_METHOD(write);
        sensitive << data_mem_write.pos();
    }

    void read() {
        mem_data = data_memory[address.read()];
    }

    void write() {
        data_memory[address.read()] = write_data;
    }
};

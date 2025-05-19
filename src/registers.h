#pragma once

#include <sysc/kernel/sc_module.h>
#include <systemc.h>

SC_MODULE(Registers) {
    sc_in<sc_uint<5>> rs, rt, rd;
    sc_in<sc_uint<32>> mem_write_data;
    sc_in<sc_uint<32>> alu_write_data;
    sc_in<bool> reg_write{"reg_write"};
    sc_in<bool> mem_to_reg{"mem_to_reg"}; // 0: ULA  1: MEM

    sc_out<sc_uint<32>> read_data_s{"read_data_s"};
    sc_out<sc_uint<32>> read_data_t{"read_data_t"};

    sc_uint<32> registers[32];

    SC_CTOR(Registers) {
        for (int i = 0; i < 32; i++) {
            registers[i] = 0;
        }

        SC_METHOD(read);
        sensitive << rs << rt;

        SC_METHOD(write);
        sensitive << reg_write.pos();
    }

    void read() {
        read_data_s.write(registers[rs.read()]);
        read_data_t.write(registers[rt.read()]);
    }

    void write() {
        if (mem_to_reg.read()) {
            registers[rd.read()] = mem_write_data.read();
        } else {
            registers[rd.read()] = alu_write_data.read();
        }
    }
};

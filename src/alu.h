#pragma once

#include <systemc.h>
#include "opcode.h"

SC_MODULE(ALU) {
    sc_in<sc_uint<32>> operand_s;
    sc_in<sc_uint<32>> operand_t;
    sc_in<sc_uint<6>> opcode;
    sc_in<bool> alu_execute;

    sc_out<sc_uint<32>> result;
    sc_out<bool> zero, negative;

    void compute() {
        sc_uint<32> s = operand_s.read();
        sc_uint<32> t = operand_t.read();
        sc_uint<32> res;

        switch (opcode.read()) {
            case Opcode::ADD: res = (s + t); break;
            case Opcode::SUB: res = (s - t); break;
            case Opcode::AND: res = (s & t); break;
            case Opcode::OR: res = (s | t); break;
            case Opcode::XOR: res = (s ^ t); break;
            case Opcode::NOT: res = (~s); break;
            case Opcode::CMP:
                res = (s - t);
                zero.write(res == 0);
                negative.write((sc_int<32>)res < 0);
                break;
            default: break;
        }

        result.write(res);
    }

    SC_CTOR(ALU) {
        SC_METHOD(compute);
        sensitive << alu_execute.pos();
        dont_initialize();
    }
};

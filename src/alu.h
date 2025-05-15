#pragma once

#include <systemc.h>
#include "opcode.h"

SC_MODULE(ALU) {
    sc_in<sc_uint<32>> operand_a;
    sc_in<sc_uint<32>> operand_b;
    sc_in<sc_uint<6>> opcode;
    sc_out<sc_uint<32>> result;
    sc_out<bool> zero, negative;

    void compute() {
        sc_uint<32> a = operand_a.read();
        sc_uint<32> b = operand_b.read();
        sc_uint<32> res;

        switch (opcode.read()) {
            case Opcode::ADD: res = (a + b); break;
            case Opcode::SUB: res = (a - b); break;
            case Opcode::AND: res = (a & b); break;
            case Opcode::OR: res = (a | b); break;
            case Opcode::XOR: res = (a ^ b); break;
            case Opcode::NOT: res = (~a); break;
            case Opcode::CMP: res = (a - b); break;
            default: res = 0; break;
        }

        result.write(res);
        zero.write(res == 0);
        negative.write((int32_t)res < 0);
    }

    SC_CTOR(ALU) {
        SC_METHOD(compute);
        sensitive << operand_a << operand_b << opcode;
    }
};

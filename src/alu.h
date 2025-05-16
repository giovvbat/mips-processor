#pragma once

#include <systemc.h>
#include "opcode.h"

SC_MODULE(ALU) {
    sc_in<sc_uint<32>> operand_s;
    sc_in<sc_uint<32>> operand_t;
    sc_in<sc_uint<6>> opcode;
    sc_in<bool> alu_execute;
    sc_signal<sc_uint<32>> result;
    sc_signal<bool> zero, negative;

    void compute() {
        sc_uint<32> s = operand_s.read();
        sc_uint<32> t = operand_t.read();
        sc_uint<32> res;

        cout << operand_s.read() << endl << operand_t.read() << endl << endl;

        switch (opcode.read()) {
            case Opcode::ADD: res = (s + t); break;
            case Opcode::SUB: res = (s - t); break;
            case Opcode::AND: res = (s & t); break;
            case Opcode::OR: res = (s | t); break;
            case Opcode::XOR: res = (s ^ t); break;
            case Opcode::NOT: res = (~s); break;
            case Opcode::CMP: res = (s - t); break;
            default: break;
        }

        result.write(res);
        zero.write(res == 0);
        negative.write((int32_t)res < 0);
    }

    SC_CTOR(ALU) {
        SC_METHOD(compute);
        sensitive << alu_execute.pos();
        dont_initialize();
    }
};

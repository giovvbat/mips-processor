#pragma once

#include <systemc.h>
#include "opcode.h"

SC_MODULE(ALU) {
    sc_in<sc_uint<32>> operand_a;
    sc_in<sc_uint<32>> operand_b;
    sc_in<sc_uint<6>> opcode;
    sc_out<sc_uint<32>> result;

    void compute() {
        switch (opcode.read()) {
            case Opcode::ADD:
                result.write(operand_a.read() + operand_b.read());
                break;
            case Opcode::SUB:
                result.write(operand_a.read() - operand_b.read());
                break;
            case Opcode::AND:
                result.write(operand_a.read() & operand_b.read());
                break;
            case Opcode::OR:
                result.write(operand_a.read() | operand_b.read());
                break;
            case Opcode::XOR:
                result.write(operand_a.read() ^ operand_b.read());
                break;
            case Opcode::NOT:
                result.write(~operand_a.read());
                break;
            case Opcode::CMP:
                result.write((operand_a.read() < operand_b.read()) ? 1 : 0);
                break;
            default:
                cout << "Instrução não reconhecida pela ULA!" << endl;
                break;
        }
    }

    SC_CTOR(ALU) {
        SC_METHOD(compute);
        sensitive << operand_a << operand_b << opcode;
        dont_initialize();
    }
};

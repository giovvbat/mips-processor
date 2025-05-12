#include <sysc/datatypes/int/sc_nbdefs.h>
#include <systemc.h>
#include "opcode.h"

SC_MODULE(Processor) {
    sc_signal<sc_uint<8>> pc;
    sc_signal<sc_uint<32>> instruction_memory[256];
    sc_signal<sc_uint<32>> data_memory[256];
    sc_signal<sc_uint<32>> registers[32];

    SC_CTOR(Processor) {
        //                             v    v    v    v
        instruction_memory[0] = 0b11111100100000110001000000000000;

        for (int i = 0; i < 32; i++) {
            registers[i] = i;
        }

        pc = 0;

        SC_METHOD(pipeline);
        sensitive << pc;
    }

    void pipeline() {
        sc_uint<32> instruction = instruction_memory[pc.read()];

        decode_and_execute(instruction);

        if (pc.read() < 0) {
            pc.write(pc.read() + 1);
        }
    }

    void decode_and_execute(sc_uint<32> instruction) {
        sc_uint<6> opcode = instruction.range(31, 26);

        sc_uint<5> rs = instruction.range(25, 21);
        sc_uint<5> rt = instruction.range(20, 16);
        sc_uint<5> rd = instruction.range(15, 11);

        sc_uint<8> address = instruction.range(7, 0);

        switch (opcode) {
            case Opcode::ADD:
                registers[rd] = registers[rs].read() + registers[rt].read();
                break;
            case Opcode::SUB:
                registers[rd] = registers[rs].read() - registers[rt].read();
                break;
            case Opcode::AND:
                registers[rd] = registers[rs].read() & registers[rt].read();
                break;
            case Opcode::OR:
                registers[rd] = registers[rs].read() | registers[rt].read();
                break;
            case Opcode::XOR:
                registers[rd] = registers[rs].read() ^ registers[rt].read();
                break;
            case Opcode::NOT:
                registers[rd] = ~registers[rs].read();
                break;
            case Opcode::CMP:
                registers[rd] = registers[rs].read() < registers[rt].read();
                break;
            case Opcode::LD:
                registers[rd] = data_memory[address].read();
                break;
            case Opcode::ST:
                data_memory[address] = registers[rs].read();
                break;
            case Opcode::J:
                pc = address;
                break;
            case Opcode::JN:
                /*
                    if (ula.result() < 0) {
                        pc = address;
                    }
                */
                break;
            case Opcode::JZ:
                /*
                    if (ula.result() == 0) {
                        pc = address;
                    }
                */
                break;
            default:
                cout << "Instrução desconhecida!" << endl;
        }
    }
};

int sc_main(int argc, char* argv[]) {
    Processor cpu("cpu");

    cout << "Iniciando a simulação...\n";
    sc_start(1000, SC_NS);

    cout << cpu.registers[2] << endl;

    return 0;
}

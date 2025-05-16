#pragma once

#include <sysc/kernel/sc_time.h>
#include <sysc/kernel/sc_wait.h>
#include <systemc.h>
#include "opcode.h"
#include "alu.h"
#include "states.h"

SC_MODULE(Processor) {
    sc_signal<sc_uint<8>> pc;
    sc_signal<sc_uint<32>> instruction_memory[256];
    sc_signal<sc_uint<32>> data_memory[256];
    sc_signal<sc_uint<32>> registers[32];

    sc_signal<sc_uint<32>> alu_operand_a, alu_operand_b;
    sc_signal<sc_uint<6>> alu_opcode;
    sc_signal<sc_uint<32>> alu_result;
    sc_signal<bool> alu_negative;
    sc_signal<bool> alu_zero;

    sc_signal<sc_uint<32>> fetched_instruction;
    sc_signal<sc_uint<5>> rs, rt, rd;
    sc_signal<sc_uint<8>> address;
    sc_signal<sc_uint<6>> opcode;

    ALU* alu;
    States current_state;

    SC_CTOR(Processor) {
        alu = new ALU("ALU");
        alu->operand_s(alu_operand_a);
        alu->operand_t(alu_operand_b);
        alu->opcode(alu_opcode);
        alu->result(alu_result);
        alu->zero(alu_zero);
        alu->negative(alu_negative);

        instruction_memory[0] = (Opcode::CMP << 26) | (5 << 21) | (5 << 16);
        instruction_memory[1] = (Opcode::JN << 26) | 3;
        instruction_memory[2] = (Opcode::ADD << 26) | (5 << 21) | (5 << 16) | (5 << 11);
        instruction_memory[3] = (Opcode::ADD << 26) | (5 << 21) | (5 << 16) | (5 << 11);

        for (int i = 0; i < 32; i++) {
            registers[i] = i;
        }

        pc = 0;
        current_state = FETCH;

        SC_THREAD(pipeline);
    }

    void pipeline() {
        while (true) {
            switch (current_state) {
                case FETCH:
                    fetched_instruction = instruction_memory[pc.read()];
                    current_state = DECODE;
                    break;
                case DECODE:
                    opcode = fetched_instruction.read().range(31, 26);
                    rs = fetched_instruction.read().range(25, 21);
                    rt = fetched_instruction.read().range(20, 16);
                    rd = fetched_instruction.read().range(15, 11);
                    address = fetched_instruction.read().range(7, 0);

                    current_state = DELEGATE;
                    break;
                case DELEGATE:
                    switch (opcode.read()) {
                        case Opcode::ADD:
                        case Opcode::SUB:
                        case Opcode::AND:
                        case Opcode::OR:
                        case Opcode::XOR:
                        case Opcode::NOT:
                        case Opcode::CMP:
                            current_state = LOAD;
                            break;
                        case Opcode::LD:
                        case Opcode::ST:
                            current_state = MEMORY;
                            break;
                        case Opcode::J:
                        case Opcode::JN:
                        case Opcode::JZ:
                            current_state = JUMP;
                            break;
                        default:
                            cout << "DECODE: opcode desconhecido (" << opcode.read() << "), ignorando.\n";
                            current_state = WRITEBACK;
                            break;
                    }
                    break;
                case LOAD:
                    alu_operand_a.write(registers[rs.read()].read());
                    alu_operand_b.write(registers[rt.read()].read());
                    alu_opcode.write(opcode);
                    current_state = EXECUTE;
                    break;
                case EXECUTE:
                    if (opcode.read() != Opcode::CMP) {
                        registers[rd.read()].write(alu_result.read());
                    }

                    cout << "EXECUTE: ALU result = " << alu_result.read() << endl;
                    current_state = WRITEBACK;
                    break;
                case MEMORY:
                    switch (opcode.read()) {
                        case Opcode::LD:
                            registers[rd.read()] = data_memory[address.read()].read();
                            break;
                        case Opcode::ST:
                            data_memory[address.read()] = registers[rs.read()].read();
                            break;
                        default:
                            break;
                    }

                    current_state = WRITEBACK;
                    break;
                case JUMP:
                    switch (opcode.read()) {
                        case Opcode::J:
                            pc = address.read();
                            current_state = FETCH;
                            break;
                        case Opcode::JZ:
                            if (alu_zero.read()) {
                                pc = address.read();
                                current_state = FETCH;
                            } else {
                                current_state = WRITEBACK;
                            }
                            break;
                        case Opcode::JN:
                            if (alu_negative.read()) {
                                pc = address.read();
                                current_state = FETCH;
                            } else {
                                current_state = WRITEBACK;
                            }
                            break;
                        default:
                            break;
                    }
                    break;
                case WRITEBACK:
                    if (pc.read() >= 3) {
                        return;
                    }
                    pc.write(pc.read() + 1);
                    current_state = FETCH;
                    break;
                }

            wait(10, SC_NS);
        }
    }
};

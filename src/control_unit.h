#pragma once

#include "opcode.h"
#include "states.h"
#include <sysc/kernel/sc_module.h>
#include <systemc.h>

SC_MODULE(ControlUnit) {
    sc_in<bool> clk;
    sc_in<sc_uint<6>> opcode;
    sc_in<bool> alu_zero, alu_negative;

    sc_signal<sc_uint<6>> alu_opcode;
    sc_signal<bool> alu_execute;
    sc_signal<bool> pc_write;
    sc_signal<bool> pc_source; // 0: incrementa  1: jump
    sc_signal<bool> reg_write;
    sc_signal<bool> mem_to_reg; // 0: ULA  1: MEM
    sc_signal<bool> inst_mem_read;
    sc_signal<bool> data_mem_read;
    sc_signal<bool> data_mem_write;

    States current_state;

    SC_CTOR(ControlUnit) {
        current_state = FETCH;
        alu_execute = pc_write = pc_source = reg_write = data_mem_write = inst_mem_read = data_mem_read = mem_to_reg = false;

        SC_METHOD(fsm);
        sensitive << clk.pos();
    }

    void fsm() {
        switch (current_state) {
            case FETCH:
                pc_source = pc_write = reg_write = false;
                inst_mem_read = true;
                current_state = DECODE;
                break;
            case DECODE:
                inst_mem_read = false;
                current_state = EXECUTE;
                break;
            case EXECUTE:
                alu_opcode = opcode;
                alu_execute = true;
                current_state = MEMORY;
                break;
            case MEMORY:
                alu_execute = false;

                switch (opcode.read()) {
                    case Opcode::ST:
                        data_mem_write = true;
                        break;
                    case Opcode::LD:
                        data_mem_read = true;
                        break;
                    default:
                        break;
                }

                current_state = WRITEBACK;
                break;
            case WRITEBACK:
                switch (opcode.read()) {
                    case Opcode::J:
                        pc_source = true;
                        break;
                    case Opcode::JN:
                        if (alu_negative.read()) {
                            pc_source = true;
                        }
                        break;
                    case Opcode::JZ:
                        if (alu_zero.read()) {
                            pc_source = true;
                        }
                        break;
                    case Opcode::LD:
                        mem_to_reg = true;
                        reg_write = true;
                        break;
                    case Opcode::ADD:
                    case Opcode::SUB:
                    case Opcode::AND:
                    case Opcode::OR:
                    case Opcode::XOR:
                    case Opcode::NOT:
                        reg_write = true;
                        break;
                    default:
                        break;
                }

                pc_write = true;
                data_mem_read = data_mem_write = false;
                break;
            default:
                break;
        }
    }

};

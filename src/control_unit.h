#pragma once

#include "opcode.h"
#include "states.h"
#include <sysc/kernel/sc_module.h>
#include <systemc.h>

SC_MODULE(ControlUnit) {
    sc_in<bool> clk;
    sc_in<sc_uint<6>> opcode;
    sc_in<bool> alu_zero, alu_negative;

    sc_out<sc_uint<6>> alu_opcode;
    sc_out<bool> alu_execute;
    sc_out<bool> pc_write;
    sc_out<bool> pc_source; // 0: incrementa  1: jump
    sc_out<bool> reg_write;
    sc_out<bool> mem_to_reg; // 0: ULA  1: MEM
    sc_out<bool> inst_mem_read;
    sc_out<bool> data_mem_read;
    sc_out<bool> data_mem_write;

    sc_signal<States> current_state;

    SC_CTOR(ControlUnit) {
        current_state.write(FETCH);

        SC_METHOD(fsm);
        sensitive << clk.pos();
        dont_initialize();
    }

    void fsm() {
        // Zera todos os sinais de controle no início de cada ciclo
        alu_opcode = 0;
        alu_execute = false;
        pc_write = false;
        pc_source = false;
        reg_write = false;
        mem_to_reg = false;
        inst_mem_read = false;
        data_mem_read = false;
        data_mem_write = false;

        switch (current_state.read()) {
            case FETCH:
                inst_mem_read = true;
                current_state.write(DECODE);
                break;
            case DECODE:
                current_state.write(EXECUTE);
                break;
            case EXECUTE:
                alu_opcode = opcode;
                alu_execute = true;
                current_state.write(MEMORY);
                break;
            case MEMORY:
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

                current_state.write(WRITEBACK);
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
                current_state.write(FETCH);
                break;
            default:
                break;
        }
    }

};

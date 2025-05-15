#pragma once

#include "processor.h"
#include <sysc/kernel/sc_module.h>
#include <systemc.h>

SC_MODULE(ControlUnit) {
    sc_in<sc_uint<6>> opcode;

    sc_out<sc_uint<6>> alu_opcode;
    sc_out<bool> pc_write;
    sc_out<bool> reg_write;
    sc_out<bool> mem_write;
    sc_out<bool> inst_mem_read;
    sc_out<bool> data_mem_read;
    sc_out<bool> mem_to_reg; // 0: ULA  1: MEM

    States current_state;

    SC_CTOR(ControlUnit) {
        current_state = FETCH;
        pc_write = reg_write = mem_write = inst_mem_read = data_mem_read = mem_to_reg = false;

        SC_METHOD(fsm);
        sensitive << opcode;
    }

    void fsm() {
        switch (current_state) {
            case FETCH:
                inst_mem_read = true;
                current_state = DECODE;
                break;
            case DECODE:
            case DELEGATE:
            case LOAD:
            case EXECUTE:
            case MEMORY:
            case JUMP:
            case WRITEBACK:
                break;
            default:
                break;
        }
    }

};

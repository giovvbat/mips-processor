#pragma once

#include "control_unit.h"
#include "operative_unit.h"
#include "states.h"
#include "opcode.h"
#include <sysc/kernel/sc_module.h>
#include <systemc.h>
#include <iostream>

SC_MODULE(Processor) {
    sc_in<bool> clk;

    sc_signal<sc_uint<6>> alu_opcode;
    sc_signal<bool> alu_execute;
    sc_signal<bool> pc_write;
    sc_signal<bool> pc_source; // 0: incrementa  1: jump
    sc_signal<bool> reg_write;
    sc_signal<bool> mem_to_reg; // 0: ULA  1: MEM
    sc_signal<bool> inst_mem_read;
    sc_signal<bool> data_mem_read;
    sc_signal<bool> data_mem_write;

    sc_signal<sc_uint<6>> opcode;
    sc_signal<sc_uint<5>> rs, rt, rd;
    sc_signal<sc_uint<8>> address;

    sc_signal<sc_uint<32>> reg_data_s, reg_data_t;

    sc_signal<sc_uint<32>> alu_result;
    sc_signal<bool> alu_zero;
    sc_signal<bool> alu_negative;

    sc_signal<sc_uint<32>> mem_data;

    sc_signal<sc_uint<8>> pc_value;

    sc_signal<sc_uint<32>> fetched_instruction;

    OperativeUnit* operative_unit;
    ControlUnit* control_unit;

    SC_CTOR(Processor) {
        operative_unit = new OperativeUnit("OperativeUnit");
        control_unit = new ControlUnit("ControlUnit");

        // Connect control unit ports
        control_unit->clk(clk);
        control_unit->opcode(opcode);
        control_unit->alu_zero(alu_zero);
        control_unit->alu_negative(alu_negative);

        control_unit->alu_opcode(alu_opcode);
        control_unit->alu_execute(alu_execute);
        control_unit->pc_write(pc_write);
        control_unit->pc_source(pc_source);
        control_unit->reg_write(reg_write);
        control_unit->mem_to_reg(mem_to_reg);
        control_unit->inst_mem_read(inst_mem_read);
        control_unit->data_mem_read(data_mem_read);
        control_unit->data_mem_write(data_mem_write);

        operative_unit->fetched_instruction(fetched_instruction);
        operative_unit->opcode(opcode);
        operative_unit->rs(rs);
        operative_unit->rt(rt);
        operative_unit->rd(rd);
        operative_unit->address(address);

        // Connect ALU ports
        operative_unit->alu->operand_s(reg_data_s);
        operative_unit->alu->operand_t(reg_data_t);
        operative_unit->alu->opcode(alu_opcode);
        operative_unit->alu->alu_execute(alu_execute);

        operative_unit->alu->result(alu_result);
        operative_unit->alu->zero(alu_zero);
        operative_unit->alu->negative(alu_negative);

        // Connect Registers ports
        operative_unit->registers->rs(rs);
        operative_unit->registers->rt(rt);
        operative_unit->registers->rd(rd);
        operative_unit->registers->mem_write_data(mem_data);
        operative_unit->registers->alu_write_data(alu_result);
        operative_unit->registers->reg_write(reg_write);
        operative_unit->registers->mem_to_reg(mem_to_reg);

        operative_unit->registers->read_data_s(reg_data_s);
        operative_unit->registers->read_data_t(reg_data_t);

        // Connect Data Memory ports
        operative_unit->data_memory->data_mem_read(data_mem_read);
        operative_unit->data_memory->data_mem_write(data_mem_write);
        operative_unit->data_memory->address(address);
        operative_unit->data_memory->write_data(reg_data_s);
        operative_unit->data_memory->mem_data(mem_data);

        // Connect Instruction Memory ports
        operative_unit->instruction_memory->address(pc_value);
        operative_unit->instruction_memory->inst_mem_read(inst_mem_read);
        operative_unit->instruction_memory->fetched_instruction(fetched_instruction);

        // Connect PC ports
        operative_unit->pc->pc_source(pc_source);
        operative_unit->pc->pc_write(pc_write);
        operative_unit->pc->jump_address(address);
        operative_unit->pc->pc(pc_value);
    }

    ~Processor() {
        delete operative_unit;
        delete control_unit;
    }
};

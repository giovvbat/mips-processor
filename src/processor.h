#pragma once

#include "control_unit.h"
#include "operative_unit.h"
#include <sysc/kernel/sc_module.h>
#include <systemc.h>

SC_MODULE(Processor) {
    sc_in<bool> clk;

    OperativeUnit* operative_unit;
    ControlUnit* control_unit;

    SC_CTOR(Processor) {
        operative_unit = new OperativeUnit("OperativeUnit");
        control_unit = new ControlUnit("ControlUnit");

        control_unit->clk(clk);
        control_unit->opcode(operative_unit->opcode);
        control_unit->alu_zero(operative_unit->alu->zero);
        control_unit->alu_negative(operative_unit->alu->negative);

        operative_unit->alu->opcode(control_unit->alu_opcode);
        operative_unit->alu->alu_execute(control_unit->alu_execute);
        operative_unit->alu->operand_s(operative_unit->registers->read_data_s);
        operative_unit->alu->operand_t(operative_unit->registers->read_data_t);

        operative_unit->registers->rs(operative_unit->rs);
        operative_unit->registers->rt(operative_unit->rt);
        operative_unit->registers->rd(operative_unit->rd);
        operative_unit->registers->alu_write_data(operative_unit->alu->result);
        operative_unit->registers->mem_write_data(operative_unit->data_memory->mem_data);
        operative_unit->registers->reg_write(control_unit->reg_write);
        operative_unit->registers->mem_to_reg(control_unit->mem_to_reg);

        operative_unit->instruction_memory->address(operative_unit->pc->pc);
        operative_unit->instruction_memory->inst_mem_read(control_unit->inst_mem_read);
        operative_unit->fetched_instruction(operative_unit->instruction_memory->fetched_instruction);

        operative_unit->data_memory->write_data(operative_unit->registers->read_data_s);
        operative_unit->data_memory->address(operative_unit->address);
        operative_unit->data_memory->data_mem_read(control_unit->data_mem_read);
        operative_unit->data_memory->data_mem_write(control_unit->data_mem_write);

        operative_unit->pc->pc_write(control_unit->pc_write);
        operative_unit->pc->pc_source(control_unit->pc_source);
        operative_unit->pc->jump_address(operative_unit->address);
    }
};

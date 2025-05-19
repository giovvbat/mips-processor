#include "../src/operative_unit.h"
#include <iostream>
#include <iomanip>

// Function to print test results with proper formatting
void print_test_results(const std::string& test_name,
                        sc_uint<6> expected_op, sc_uint<5> expected_rs,
                        sc_uint<5> expected_rt, sc_uint<5> expected_rd,
                        sc_uint<8> expected_addr,
                        sc_uint<6> actual_op, sc_uint<5> actual_rs,
                        sc_uint<5> actual_rt, sc_uint<5> actual_rd,
                        sc_uint<8> actual_addr) {

    bool pass = (expected_op == actual_op &&
                expected_rs == actual_rs &&
                expected_rt == actual_rt &&
                expected_rd == actual_rd &&
                expected_addr == actual_addr);

    std::cout << "=== " << test_name << " ===" << std::endl;
    std::cout << "Expected: opcode=0x" << std::hex << std::setw(2) << std::setfill('0') << expected_op
              << ", rs=0x" << std::setw(2) << expected_rs
              << ", rt=0x" << std::setw(2) << expected_rt
              << ", rd=0x" << std::setw(2) << expected_rd
              << ", address=0x" << std::setw(2) << expected_addr << std::dec << std::endl;

    std::cout << "Actual:   opcode=0x" << std::hex << std::setw(2) << std::setfill('0') << actual_op
              << ", rs=0x" << std::setw(2) << actual_rs
              << ", rt=0x" << std::setw(2) << actual_rt
              << ", rd=0x" << std::setw(2) << actual_rd
              << ", address=0x" << std::setw(2) << actual_addr << std::dec << std::endl;

    std::cout << "Result:   " << (pass ? "PASS" : "FAIL") << std::endl << std::endl;
}

int sc_main(int argc, char* argv[]) {
    // Clock signal for synchronous components
    sc_clock clock("clock", 10, SC_NS);

    // ========== Input signals for OperativeUnit ==========
    sc_signal<sc_uint<32>> fetched_instruction;

    // ========== Output signals for OperativeUnit ==========
    sc_signal<sc_uint<6>> opcode;
    sc_signal<sc_uint<5>> rs, rt, rd;
    sc_signal<sc_uint<8>> address;

    // ========== Control Signals ==========
    sc_signal<sc_uint<6>> alu_opcode;
    sc_signal<bool> alu_execute;
    sc_signal<bool> pc_write;
    sc_signal<bool> pc_source; // 0: incrementa  1: jump
    sc_signal<bool> reg_write;
    sc_signal<bool> mem_to_reg; // 0: ULA  1: MEM
    sc_signal<bool> inst_mem_read;
    sc_signal<bool> data_mem_read;
    sc_signal<bool> data_mem_write;

    // ========== Data Signals ==========
    sc_signal<sc_uint<32>> reg_data_s, reg_data_t;
    sc_signal<sc_uint<32>> alu_result;
    sc_signal<bool> alu_zero;
    sc_signal<bool> alu_negative;
    sc_signal<sc_uint<32>> mem_data;
    sc_signal<sc_uint<8>> pc_value;

    // Instantiate the module under test
    OperativeUnit operative_unit("operative_unit");

    // ========== Connect OperativeUnit ports ==========
    operative_unit.fetched_instruction(fetched_instruction);
    operative_unit.opcode(opcode);
    operative_unit.rs(rs);
    operative_unit.rt(rt);
    operative_unit.rd(rd);
    operative_unit.address(address);

    // ========== Connect ALU ports ==========
    operative_unit.alu->operand_s(reg_data_s);
    operative_unit.alu->operand_t(reg_data_t);
    operative_unit.alu->opcode(alu_opcode);
    operative_unit.alu->alu_execute(alu_execute);
    operative_unit.alu->result(alu_result);
    operative_unit.alu->zero(alu_zero);
    operative_unit.alu->negative(alu_negative);

    // ========== Connect Registers ports ==========
    operative_unit.registers->rs(rs);
    operative_unit.registers->rt(rt);
    operative_unit.registers->rd(rd);
    operative_unit.registers->mem_write_data(mem_data);
    operative_unit.registers->alu_write_data(alu_result);
    operative_unit.registers->reg_write(reg_write);
    operative_unit.registers->mem_to_reg(mem_to_reg);
    operative_unit.registers->read_data_s(reg_data_s);
    operative_unit.registers->read_data_t(reg_data_t);

    // ========== Connect Data Memory ports ==========
    operative_unit.data_memory->data_mem_read(data_mem_read);
    operative_unit.data_memory->data_mem_write(data_mem_write);
    operative_unit.data_memory->address(address);
    operative_unit.data_memory->write_data(reg_data_s);
    operative_unit.data_memory->mem_data(mem_data);

    // ========== Connect Instruction Memory ports ==========
    operative_unit.instruction_memory->address(pc_value);
    operative_unit.instruction_memory->inst_mem_read(inst_mem_read);
    operative_unit.instruction_memory->fetched_instruction(fetched_instruction);

    // ========== Connect PC ports ==========
    operative_unit.pc->pc_source(pc_source);
    operative_unit.pc->pc_write(pc_write);
    operative_unit.pc->jump_address(address);
    operative_unit.pc->pc(pc_value);

    // Open VCD file
    sc_trace_file* wf = sc_create_vcd_trace_file("waveforms/operative_unit_tb");
    if (!wf) {
        std::cerr << "Error: Could not create waveform file." << std::endl;
        return -1;
    }

    // Trace signals
    // Main OperativeUnit signals
    sc_trace(wf, clock, "clock");
    sc_trace(wf, fetched_instruction, "fetched_instruction");
    sc_trace(wf, opcode, "opcode");
    sc_trace(wf, rs, "rs");
    sc_trace(wf, rt, "rt");
    sc_trace(wf, rd, "rd");
    sc_trace(wf, address, "address");

    // ALU signals
    sc_trace(wf, reg_data_s, "reg_data_s");
    sc_trace(wf, reg_data_t, "reg_data_t");
    sc_trace(wf, alu_opcode, "alu_opcode");
    sc_trace(wf, alu_execute, "alu_execute");
    sc_trace(wf, alu_result, "alu_result");
    sc_trace(wf, alu_zero, "alu_zero");
    sc_trace(wf, alu_negative, "alu_negative");

    // Other control signals
    sc_trace(wf, pc_write, "pc_write");
    sc_trace(wf, pc_source, "pc_source");
    sc_trace(wf, reg_write, "reg_write");
    sc_trace(wf, mem_to_reg, "mem_to_reg");
    sc_trace(wf, inst_mem_read, "inst_mem_read");
    sc_trace(wf, data_mem_read, "data_mem_read");
    sc_trace(wf, data_mem_write, "data_mem_write");

    // Data signals
    sc_trace(wf, mem_data, "mem_data");
    sc_trace(wf, pc_value, "pc_value");

    // Start simulation
    sc_start(0, SC_NS);

    std::cout << "\n=========================================" << std::endl;
    std::cout << "  OPERATIVE UNIT TESTBENCH" << std::endl;
    std::cout << "=========================================" << std::endl;

    // Test Case 1: ADD instruction (R-type)
    // opcode=0x00, rs=0x01, rt=0x02, rd=0x03, address=0x20
    fetched_instruction.write((Opcode::ADD << 26) | (1 << 21) | (2 << 16) | (3 << 11) | (0x20));
    sc_start(10, SC_NS);
    print_test_results("Test Case 1: ADD (R-type)",
                      0x00, 0x01, 0x02, 0x03, 0x20,
                      opcode.read(), rs.read(), rt.read(), rd.read(), address.read());

    // Test Case 2: SUB instruction (R-type)
    // opcode=0x01, rs=0x05, rt=0x06, rd=0x07, address=0x00
    fetched_instruction.write((Opcode::SUB << 26) | (5 << 21) | (6 << 16) | (7 << 11) | (0));
    sc_start(10, SC_NS);
    print_test_results("Test Case 2: SUB (R-type)",
                      0x01, 0x05, 0x06, 0x07, 0x00,
                      opcode.read(), rs.read(), rt.read(), rd.read(), address.read());

    // Test Case 3: LD instruction (I-type)
    // opcode=0x07, rs=0x0A, rt=0x0B, address=0x42
    fetched_instruction.write((Opcode::LD << 26) | (0x0A << 21) | (0x0B << 16) | (0x42));
    sc_start(10, SC_NS);
    print_test_results("Test Case 3: LD (I-type)",
                      0x07, 0x0A, 0x0B, 0, 0x42,
                      opcode.read(), rs.read(), rt.read(), rd.read(), address.read());

    // Test Case 4: ST instruction (I-type)
    // opcode=0x08, rs=0x0C, rt=0x0D, address=0x55
    fetched_instruction.write((Opcode::ST << 26) | (0x0C << 21) | (0x0D << 16) | (0x55));
    sc_start(10, SC_NS);
    print_test_results("Test Case 4: ST (I-type)",
                      0x08, 0x0C, 0x0D, 0, 0x55,
                      opcode.read(), rs.read(), rt.read(), rd.read(), address.read());

    // Test Case 5: J instruction (J-type)
    // opcode=0x09, address=0x77
    fetched_instruction.write((Opcode::J << 26) | (0x77));
    sc_start(10, SC_NS);
    print_test_results("Test Case 5: J (J-type)",
                      0x09, 0, 0, 0, 0x77,
                      opcode.read(), rs.read(), rt.read(), rd.read(), address.read());

    // Test Case 6: JZ instruction (J-type)
    // opcode=0x0B, address=0x77
    fetched_instruction.write((Opcode::JZ << 26) | (0x77));
    sc_start(10, SC_NS);
    print_test_results("Test Case 6: JZ (J-type)",
                      0x0B, 0, 0, 0, 0x77,
                      opcode.read(), rs.read(), rt.read(), rd.read(), address.read());

    // Test Case 7: AND instruction (R-type)
    // opcode=0x02, rs=0x10, rt=0x11, rd=0x12, address=0x00
    fetched_instruction.write((Opcode::AND << 26) | (0x10 << 21) | (0x11 << 16) | (0x12 << 11) | (0));
    sc_start(10, SC_NS);
    print_test_results("Test Case 7: AND (R-type)",
                      0x02, 0x10, 0x11, 0x12, 0x00,
                      opcode.read(), rs.read(), rt.read(), rd.read(), address.read());

    // Test Case 8: Random values to test bit extraction
    // opcode=0x3F, rs=0x1F, rt=0x1F, rd=0x1F, address=0xFF
    fetched_instruction.write((0x3F << 26) | (0x1F << 21) | (0x1F << 16) | (0x1F << 11) | (0xFF));
    sc_start(10, SC_NS);
    print_test_results("Test Case 8: Maximum Values",
                      0x3F, 0x1F, 0x1F, 0x1F, 0xFF,
                      opcode.read(), rs.read(), rt.read(), rd.read(), address.read());

    // Close VCD file
    sc_close_vcd_trace_file(wf);

    std::cout << "=========================================" << std::endl;
    std::cout << "  TESTBENCH COMPLETED" << std::endl;
    std::cout << "=========================================" << std::endl;

    return 0;
}

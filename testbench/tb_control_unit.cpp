#include <systemc.h>
#include "../src/control_unit.h"
#include <iomanip>

SC_MODULE(ControlUnitTB) {
    sc_clock clk{"clk", 10, SC_NS};
    sc_signal<sc_uint<6>> opcode;
    sc_signal<bool> alu_zero, alu_negative;

    sc_signal<sc_uint<6>> alu_opcode;
    sc_signal<bool> alu_execute;
    sc_signal<bool> pc_write;
    sc_signal<bool> pc_source; // 0: incrementa  1: jump
    sc_signal<bool> reg_write;
    sc_signal<bool> mem_to_reg; // 0: ULA  1: MEM
    sc_signal<bool> inst_mem_read;
    sc_signal<bool> data_mem_read;
    sc_signal<bool> data_mem_write;

    ControlUnit* cu;

    void print_state() {
        std::cout << "State: ";
        switch (cu->current_state) {
            case FETCH: std::cout << "FETCH"; break;
            case DECODE: std::cout << "DECODE"; break;
            case EXECUTE: std::cout << "EXECUTE"; break;
            case MEMORY: std::cout << "MEMORY"; break;
            case WRITEBACK: std::cout << "WRITEBACK"; break;
            default: std::cout << "UNKNOWN"; break;
        }
        std::cout << std::endl;
    }

    void print_control_signals() {
        std::cout << "Control Signals:" << std::endl;
        std::cout << "  alu_opcode:     " << std::setw(4) << cu->alu_opcode.read() << std::endl;
        std::cout << "  alu_execute:    " << std::setw(4) << cu->alu_execute.read() << std::endl;
        std::cout << "  pc_write:       " << std::setw(4) << cu->pc_write.read() << std::endl;
        std::cout << "  pc_source:      " << std::setw(4) << cu->pc_source.read() << std::endl;
        std::cout << "  reg_write:      " << std::setw(4) << cu->reg_write.read() << std::endl;
        std::cout << "  mem_to_reg:     " << std::setw(4) << cu->mem_to_reg.read() << std::endl;
        std::cout << "  inst_mem_read:  " << std::setw(4) << cu->inst_mem_read.read() << std::endl;
        std::cout << "  data_mem_read:  " << std::setw(4) << cu->data_mem_read.read() << std::endl;
        std::cout << "  data_mem_write: " << std::setw(4) << cu->data_mem_write.read() << std::endl;
        std::cout << "-------------------------------------" << std::endl;
    }

    void print_test_header(const std::string& operation, const std::string& description) {
        std::cout << "\n\n========================================" << std::endl;
        std::cout << "TEST: " << operation << std::endl;
        std::cout << "Description: " << description << std::endl;
        std::cout << "========================================" << std::endl;
    }

    void run_instruction_cycle() {
        std::cout << "Starting instruction cycle..." << std::endl;

        std::cout << "T0: "; print_state();
        wait(10, SC_NS); // FETCH -> DECODE
        print_control_signals();

        std::cout << "T1: "; print_state();
        wait(10, SC_NS); // DECODE -> EXECUTE
        print_control_signals();

        std::cout << "T2: "; print_state();
        wait(10, SC_NS); // EXECUTE -> MEMORY
        print_control_signals();

        std::cout << "T3: "; print_state();
        wait(10, SC_NS); // MEMORY -> WRITEBACK
        print_control_signals();

        std::cout << "T4: "; print_state();
        wait(10, SC_NS); // WRITEBACK -> FETCH (back to beginning for next test)
        print_control_signals();
    }

    void test_process() {
        // Test arithmetic operations
        print_test_header("ADD", "Addition operation (reg_write=1, pc_write=1)");
        opcode = Opcode::ADD;
        alu_zero = false;
        alu_negative = false;
        run_instruction_cycle();

        print_test_header("SUB", "Subtraction operation (reg_write=1, pc_write=1)");
        opcode = Opcode::SUB;
        run_instruction_cycle();

        print_test_header("AND", "Logical AND operation (reg_write=1, pc_write=1)");
        opcode = Opcode::AND;
        run_instruction_cycle();

        print_test_header("OR", "Logical OR operation (reg_write=1, pc_write=1)");
        opcode = Opcode::OR;
        run_instruction_cycle();

        print_test_header("XOR", "Logical XOR operation (reg_write=1, pc_write=1)");
        opcode = Opcode::XOR;
        run_instruction_cycle();

        print_test_header("NOT", "Logical NOT operation (reg_write=1, pc_write=1)");
        opcode = Opcode::NOT;
        run_instruction_cycle();

        // Test jump operations
        print_test_header("J", "Unconditional Jump (pc_source=1, pc_write=1)");
        opcode = Opcode::J;
        run_instruction_cycle();

        // Test conditional jump operations with various conditions
        print_test_header("JZ (zero=true)", "Jump if Zero with zero flag set (pc_source=1, pc_write=1)");
        opcode = Opcode::JZ;
        alu_zero = true;
        run_instruction_cycle();

        print_test_header("JZ (zero=false)", "Jump if Zero with zero flag not set (pc_source=0, pc_write=1)");
        opcode = Opcode::JZ;
        alu_zero = false;
        run_instruction_cycle();

        print_test_header("JN (negative=true)", "Jump if Negative with negative flag set (pc_source=1, pc_write=1)");
        opcode = Opcode::JN;
        alu_negative = true;
        run_instruction_cycle();

        print_test_header("JN (negative=false)", "Jump if Negative with negative flag not set (pc_source=0, pc_write=1)");
        opcode = Opcode::JN;
        alu_negative = false;
        run_instruction_cycle();

        // Test memory operations
        print_test_header("LD", "Load from memory (mem_to_reg=1, reg_write=1, data_mem_read=1)");
        opcode = Opcode::LD;
        run_instruction_cycle();

        print_test_header("ST", "Store to memory (data_mem_write=1)");
        opcode = Opcode::ST;
        run_instruction_cycle();

        std::cout << "\nAll control unit tests completed successfully!" << std::endl;
        sc_stop();
    }

    SC_CTOR(ControlUnitTB) {
        cu = new ControlUnit("control_unit");

        cu->clk(clk);
        cu->opcode(opcode);
        cu->alu_zero(alu_zero);
        cu->alu_negative(alu_negative);

        cu->alu_opcode(alu_opcode);
        cu->alu_execute(alu_execute);
        cu->pc_write(pc_write);
        cu->pc_source(pc_source);
        cu->reg_write(reg_write);
        cu->mem_to_reg(mem_to_reg);
        cu->inst_mem_read(inst_mem_read);
        cu->data_mem_read(data_mem_read);
        cu->data_mem_write(data_mem_write);

        SC_THREAD(test_process);
    }

    ~ControlUnitTB() {
        delete cu;
    }
};

int sc_main(int argc, char* argv[]) {
    ControlUnitTB tb("tb_control_unit_improved");
    sc_start();
    return 0;
}

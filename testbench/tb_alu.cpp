#include <systemc.h>
#include "../src/alu.h"
#include "../src/opcode.h"

SC_MODULE(ALU_TB) {
    sc_signal<sc_uint<32>> operand_s, operand_t;
    sc_signal<sc_uint<6>> opcode;
    sc_signal<bool> alu_execute;
    sc_signal<sc_uint<32>> result;
    sc_signal<bool> zero, negative;

    ALU* alu;

    void test() {
        // Test ADD operation
        operand_s.write(10);
        operand_t.write(20);
        opcode.write(Opcode::ADD);
        alu_execute.write(true);
        wait(1, SC_NS);
        alu_execute.write(false);
        wait(1, SC_NS);
        cout << "ADD: Result = " << result.read() << ", Zero = " << zero.read() << ", Negative = " << negative.read() << endl;

        // Test SUB operation
        operand_s.write(30);
        operand_t.write(10);
        opcode.write(Opcode::SUB);
        alu_execute.write(true);
        wait(1, SC_NS);
        alu_execute.write(false);
        wait(1, SC_NS);
        cout << "SUB: Result = " << result.read() << ", Zero = " << zero.read() << ", Negative = " << negative.read() << endl;

        // Test AND operation
        operand_s.write(0b1100);
        operand_t.write(0b1010);
        opcode.write(Opcode::AND);
        alu_execute.write(true);
        wait(1, SC_NS);
        alu_execute.write(false);
        wait(1, SC_NS);
        cout << "AND: Result = " << result.read() << ", Zero = " << zero.read() << ", Negative = " << negative.read() << endl;

        // Test OR operation
        operand_s.write(0b1100);
        operand_t.write(0b1010);
        opcode.write(Opcode::OR);
        alu_execute.write(true);
        wait(1, SC_NS);
        alu_execute.write(false);
        wait(1, SC_NS);
        cout << "OR: Result = " << result.read() << ", Zero = " << zero.read() << ", Negative = " << negative.read() << endl;

        // Test CMP operation
        operand_s.write(10);
        operand_t.write(10);
        opcode.write(Opcode::CMP);
        alu_execute.write(true);
        wait(1, SC_NS);
        alu_execute.write(false);
        wait(1, SC_NS);
        cout << "CMP: Result = " << result.read() << ", Zero = " << zero.read() << ", Negative = " << negative.read() << endl;

        // Test CMP operation negative
        operand_s.write(9);
        operand_t.write(10);
        opcode.write(Opcode::CMP);
        alu_execute.write(true);
        wait(1, SC_NS);
        alu_execute.write(false);
        wait(1, SC_NS);
        cout << "CMP: Result = " << result.read() << ", Zero = " << zero.read() << ", Negative = " << negative.read() << endl;

        sc_stop(); // End simulation
    }

    SC_CTOR(ALU_TB) {
        alu = new ALU("alu");
        alu->operand_s(operand_s);
        alu->operand_t(operand_t);
        alu->opcode(opcode);
        alu->alu_execute(alu_execute);
        alu->result(result);
        alu->zero(zero);
        alu->negative(negative);

        SC_THREAD(test);
    }

    ~ALU_TB() {
        delete alu;
    }
};

int sc_main(int argc, char* argv[]) {
    ALU_TB tb("tb");

    // Create VCD trace file
    sc_trace_file* tf = sc_create_vcd_trace_file("waveforms/alu_tb");
    if (!tf) {
        cout << "Error: Could not create trace file" << endl;
        return 1;
    }

    // Trace the signals
    sc_trace(tf, tb.operand_s, "operand_s");
    sc_trace(tf, tb.operand_t, "operand_t");
    sc_trace(tf, tb.opcode, "opcode");
    sc_trace(tf, tb.alu_execute, "alu_execute");
    sc_trace(tf, tb.result, "result");
    sc_trace(tf, tb.zero, "zero");
    sc_trace(tf, tb.negative, "negative");

    sc_start();

    // Close trace file
    sc_close_vcd_trace_file(tf);

    return 0;
}

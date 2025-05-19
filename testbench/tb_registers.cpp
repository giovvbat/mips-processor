#include <systemc.h>
#include "../src/registers.h"

SC_MODULE(RegistersTB) {
    sc_signal<sc_uint<5>> rs, rt, rd;
    sc_signal<sc_uint<32>> mem_write_data, alu_write_data;
    sc_signal<bool> reg_write, mem_to_reg;
    sc_signal<sc_uint<32>> read_data_s, read_data_t;

    Registers* registers;

    void test() {
        // Test initialization
        cout << "Starting testbench..." << endl;

        // Test read operation
        rs.write(1);
        rt.write(2);
        wait(SC_ZERO_TIME);

        // Test write operation with ALU data
        mem_to_reg.write(false);
        rd.write(3);
        alu_write_data.write(42);
        reg_write.write(true);
        wait(SC_ZERO_TIME);
        cout << "Writing 42 to register 3 using ALU data..." << endl;
        wait(1, SC_NS); // Wait for write to propagate
        reg_write.write(false);

        // Verify write
        rs.write(3);
        wait(1, SC_NS);
        cout << "Read Data S after ALU write: " << read_data_s.read() << " (Expected: 42)" << endl;

        // Test write operation with MEM data
        mem_to_reg.write(true);
        rd.write(4);
        mem_write_data.write(84);
        reg_write.write(true);
        wait(SC_ZERO_TIME);
        cout << "Writing 84 to register 4 using MEM data..." << endl;
        wait(1, SC_NS); // Wait for write to propagate
        reg_write.write(false);

        // Verify write
        rs.write(4);
        wait(1, SC_NS); // Wait for write to propagate
        cout << "Read Data S after MEM write: " << read_data_s.read() << " (Expected: 84)" << endl;

        // End simulation
        sc_stop();
    }

    SC_CTOR(RegistersTB) {
        registers = new Registers("Registers");
        registers->rs(rs);
        registers->rt(rt);
        registers->rd(rd);
        registers->mem_write_data(mem_write_data);
        registers->alu_write_data(alu_write_data);
        registers->reg_write(reg_write);
        registers->mem_to_reg(mem_to_reg);

        registers->read_data_s(read_data_s);
        registers->read_data_t(read_data_t);

        SC_THREAD(test);
    }

    ~RegistersTB() {
        delete registers;
    }
};

int sc_main(int argc, char* argv[]) {
    RegistersTB tb("RegistersTB");

    // Create VCD trace file
    sc_trace_file* tf = sc_create_vcd_trace_file("waveforms/registers_tb");
    if (!tf) {
        std::cout << "Error: Could not create trace file" << std::endl;
        return 1;
    }

    // Trace the signals
    sc_trace(tf, tb.rs, "rs");
    sc_trace(tf, tb.rt, "rt");
    sc_trace(tf, tb.rd, "rd");
    sc_trace(tf, tb.mem_write_data, "mem_write_data");
    sc_trace(tf, tb.alu_write_data, "alu_write_data");
    sc_trace(tf, tb.reg_write, "reg_write");
    sc_trace(tf, tb.mem_to_reg, "mem_to_reg");
    sc_trace(tf, tb.read_data_s, "read_data_s");
    sc_trace(tf, tb.read_data_t, "read_data_t");

    sc_start();

    // Close trace file
    sc_close_vcd_trace_file(tf);

    return 0;
}

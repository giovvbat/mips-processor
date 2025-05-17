#include <systemc.h>
#include "../src/processor.h"
#include "../src/opcode.h"
#include <iostream>
#include <iomanip>

SC_MODULE(ProcessorTB) {
    // Clock signal
    sc_clock clk{"clk", 10, SC_NS};

    // Processor instance
    Processor* processor;

    // Counter to track instruction cycles
    int cycle_counter = 0;

    // Tracing utilities
    sc_trace_file* tf;

    void print_header(const std::string& title) {
        std::cout << "\n========================================" << std::endl;
        std::cout << title << std::endl;
        std::cout << "========================================" << std::endl;
    }

    void print_state() {
        std::cout << "Cycle: " << std::setw(3) << cycle_counter
                  << " | PC: " << std::setw(3) << processor->pc_value.read().to_uint()
                  << " | Instruction: 0x" << std::hex << std::setw(8) << std::setfill('0')
                  << processor->fetched_instruction.read().to_uint() << std::dec << std::setfill(' ');

        // Decode and display current instruction
        sc_uint<6> op = processor->opcode.read();
        std::string op_name;

        switch(op) {
            case Opcode::ADD: op_name = "ADD"; break;
            case Opcode::SUB: op_name = "SUB"; break;
            case Opcode::AND: op_name = "AND"; break;
            case Opcode::OR:  op_name = "OR";  break;
            case Opcode::XOR: op_name = "XOR"; break;
            case Opcode::NOT: op_name = "NOT"; break;
            case Opcode::CMP: op_name = "CMP"; break;
            case Opcode::LD:  op_name = "LD";  break;
            case Opcode::ST:  op_name = "ST";  break;
            case Opcode::J:   op_name = "J";   break;
            case Opcode::JN:  op_name = "JN";  break;
            case Opcode::JZ:  op_name = "JZ";  break;
            default:          op_name = "UNKNOWN";
        }

        std::cout << " | Op: " << op_name
                  << " | RS: " << processor->rs.read().to_uint()
                  << " | RT: " << processor->rt.read().to_uint()
                  << " | RD: " << processor->rd.read().to_uint()
                  << " | Addr: 0x" << std::hex << processor->address.read().to_uint() << std::dec;

        // Display processor state based on control signals
        std::cout << " | State: ";
        switch(processor->control_unit->current_state) {
            case FETCH:     std::cout << "FETCH";     break;
            case DECODE:    std::cout << "DECODE";    break;
            case EXECUTE:   std::cout << "EXECUTE";   break;
            case MEMORY:    std::cout << "MEMORY";    break;
            case WRITEBACK: std::cout << "WRITEBACK"; break;
            default:        std::cout << "OTHER";     break;
        }

        std::cout << std::endl;

        // Print additional debug information
        std::cout << "  ALU: " << processor->alu_result.read()
                  << " | Zero: " << processor->alu_zero.read()
                  << " | Neg: " << processor->alu_negative.read();

        std::cout << " | RS Data: " << processor->reg_data_s.read()
                  << " | RT Data: " << processor->reg_data_t.read();

        std::cout << " | Mem Data: " << processor->mem_data.read() << std::endl;

        // Print control signals
        std::cout << "  Control Signals: ";
        std::cout << "PC_Write=" << processor->pc_write.read() << " ";
        std::cout << "PC_Source=" << processor->pc_source.read() << " ";
        std::cout << "RegWrite=" << processor->reg_write.read() << " ";
        std::cout << "MemToReg=" << processor->mem_to_reg.read() << " ";
        std::cout << "ALU_Execute=" << processor->alu_execute.read() << " ";
        std::cout << "InstMemRead=" << processor->inst_mem_read.read() << " ";
        std::cout << "DataMemRead=" << processor->data_mem_read.read() << " ";
        std::cout << "DataMemWrite=" << processor->data_mem_write.read();
        std::cout << std::endl << std::endl;
    }

    void run_test() {
        // Wait for processor initialization
        wait(10, SC_NS);
        print_header("Starting Processor Test");

        // Run the processor for enough cycles to execute the test program
        for(int i = 0; i < 100; i++) {
            wait(10, SC_NS);  // Wait for one clock cycle
            cycle_counter++;
            print_state();
        }

        print_header("Test Completed");

        // Print final register values
        std::cout << "\nFinal Register Values:" << std::endl;
        for (int i = 0; i < 10; i++) { // Print the first 10 registers
            std::cout << "R" << i << ": " << processor->operative_unit->registers->registers[i] << std::endl;
        }

        // Print final memory values
        std::cout << "\nFinal Memory Values:" << std::endl;
        for (int i = 0; i < 32; i += 4) {
            std::cout << "MEM[0x" << std::hex << i << "]: 0x"
                      << processor->operative_unit->data_memory->data_memory[i] << std::dec << std::endl;
        }

        sc_stop();
    }

    // Custom initialization of the instruction memory with test program
    void initialize_instruction_memory() {
        // Get access to the instruction memory
        InstructionMemory* imem = processor->operative_unit->instruction_memory;

        // R-type instructions (ADD, SUB, AND, OR, etc.)
        imem->memory[0] = (Opcode::ADD << 26) | (1 << 21) | (2 << 16) | (3 << 11);  // ADD r3, r1, r2
        imem->memory[1] = (Opcode::SUB << 26) | (3 << 21) | (1 << 16) | (4 << 11);  // SUB r4, r3, r1
        imem->memory[2] = (Opcode::AND << 26) | (4 << 21) | (2 << 16) | (5 << 11);  // AND r5, r4, r2
        imem->memory[3] = (Opcode::OR  << 26) | (5 << 21) | (3 << 16) | (6 << 11);  // OR r6, r5, r3

        // Memory operations
        imem->memory[4] = (Opcode::ST << 26) | (6 << 21) | (0 << 16) | (0x10);      // ST r6, 0x10 (Store r6 to address 0x10)
        imem->memory[5] = (Opcode::LD << 26) | (0 << 21) | (7 << 16) | (0x10);      // LD r7, 0x10 (Load address 0x10 to r7)

        // Compare and jump
        imem->memory[6] = (Opcode::CMP << 26) | (7 << 21) | (6 << 16) | (0 << 11);  // CMP r7, r6
        imem->memory[7] = (Opcode::JZ << 26) | (0x0A);                              // JZ 0x0A (Jump if Zero to addr 10)
        imem->memory[8] = (Opcode::ADD << 26) | (7 << 21) | (1 << 16) | (7 << 11);  // ADD r7, r7, r1 (Executed if r7 != r6)
        imem->memory[9] = (Opcode::J << 26) | (0x0C);                               // J 0x0C (Unconditional jump to addr 12)

        // Jump target addresses
        imem->memory[10] = (Opcode::SUB << 26) | (7 << 21) | (1 << 16) | (7 << 11); // SUB r7, r7, r1 (Executed if r7 == r6)
        imem->memory[11] = (Opcode::J << 26) | (0x0C);                              // J 0x0C

        // More operations
        imem->memory[12] = (Opcode::XOR << 26) | (5 << 21) | (6 << 16) | (8 << 11); // XOR r8, r5, r6
        imem->memory[13] = (Opcode::NOT << 26) | (8 << 21) | (0 << 16) | (9 << 11); // NOT r9, r8
    }

    // Initialize register file with test values
    void initialize_registers() {
        // Initialize some registers with values for testing
        processor->operative_unit->registers->registers[0] = 0;    // r0 is always 0
        processor->operative_unit->registers->registers[1] = 10;   // r1 = 10
        processor->operative_unit->registers->registers[2] = 20;   // r2 = 20
    }

    SC_CTOR(ProcessorTB) {
        // Initialize processor
        processor = new Processor("processor");
        processor->clk(clk);

        // Initialize memory and registers with test data
        initialize_instruction_memory();
        initialize_registers();

        // Create trace file
        tf = sc_create_vcd_trace_file("processor_waveform");
        if (!tf) {
            std::cerr << "Error: Could not create waveform file." << std::endl;
            return;
        }

        // Add signals to trace
        sc_trace(tf, clk, "clk");
        sc_trace(tf, processor->pc_value, "pc_value");
        sc_trace(tf, processor->fetched_instruction, "fetched_instruction");
        sc_trace(tf, processor->opcode, "opcode");
        sc_trace(tf, processor->rs, "rs");
        sc_trace(tf, processor->rt, "rt");
        sc_trace(tf, processor->rd, "rd");
        sc_trace(tf, processor->address, "address");
        sc_trace(tf, processor->alu_result, "alu_result");
        sc_trace(tf, processor->alu_zero, "alu_zero");
        sc_trace(tf, processor->alu_negative, "alu_negative");
        sc_trace(tf, processor->reg_data_s, "reg_data_s");
        sc_trace(tf, processor->reg_data_t, "reg_data_t");
        sc_trace(tf, processor->mem_data, "mem_data");

        // Control signals
        sc_trace(tf, processor->pc_write, "pc_write");
        sc_trace(tf, processor->pc_source, "pc_source");
        sc_trace(tf, processor->reg_write, "reg_write");
        sc_trace(tf, processor->mem_to_reg, "mem_to_reg");
        sc_trace(tf, processor->inst_mem_read, "inst_mem_read");
        sc_trace(tf, processor->data_mem_read, "data_mem_read");
        sc_trace(tf, processor->data_mem_write, "data_mem_write");
        sc_trace(tf, processor->alu_execute, "alu_execute");
        sc_trace(tf, processor->alu_opcode, "alu_opcode");

        SC_THREAD(run_test);
    }

    ~ProcessorTB() {
        sc_close_vcd_trace_file(tf);
        delete processor;
    }
};

int sc_main(int argc, char* argv[]) {
    ProcessorTB tb("processor_testbench");
    sc_start();
    return 0;
}

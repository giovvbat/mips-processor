#include <systemc.h>
#include "../src/processor.h"
#include <iostream>

SC_MODULE(AlgorithmsTB) {
    // Clock signal
    sc_clock clk{"clk", 10, SC_NS};

    // Processor instance
    Processor* processor;

    // Tracing utilities
    sc_trace_file* tf;

    void print_header(const std::string& title) {
        std::cout << "\n========================================" << std::endl;
        std::cout << title << std::endl;
        std::cout << "========================================" << std::endl;
    }

    void print_registers() {
        std::cout << "Register values:" << std::endl;
        for (int i = 0; i < 10; i++) { // Print first 10 registers
            std::cout << "R" << i << ": " << static_cast<int32_t>(processor->operative_unit->registers->registers[i]) << std::endl;
        }
        std::cout << std::endl;
    }

    void run_test() {
        // Wait for processor initialization
        wait(10, SC_NS);

        print_header("Starting Algorithm Tests");

        // Setup instruction memory for both algorithms sequentially
        setup_memory();

        // Initialize register values for testing
        for (int i = 0; i < 32; i++) {
            processor->operative_unit->registers->registers[i] = 0;  // Initialize all registers to 0
        }

        processor->operative_unit->registers->registers[0] = 0;     // r0 is always 0
        processor->operative_unit->registers->registers[1] = static_cast<uint32_t>(-73);  // r1 = -73 for absolute value test
        processor->operative_unit->registers->registers[5] = 25;    // r5 = 25 for max test
        processor->operative_unit->registers->registers[6] = 50;    // r6 = 50 for max test

        // Run the processor for enough cycles to execute all instructions
        print_header("Running Absolute Value Algorithm");
        std::cout << "Input value: " << static_cast<int32_t>(processor->operative_unit->registers->registers[1]) << std::endl;

        // Run enough cycles to finish the absolute value algorithm
        run_cycles(60);

        // Display result of absolute value calculation
        std::cout << "Absolute value result (in R2): " << static_cast<int32_t>(processor->operative_unit->registers->registers[2]) << std::endl;
        std::cout << "Expected result: 73" << std::endl;
        std::cout << "Test " << (static_cast<int32_t>(processor->operative_unit->registers->registers[2]) == 73 ? "PASSED" : "FAILED") << std::endl;

        print_header("Running Maximum Value Algorithm");
        std::cout << "Values to compare: " << processor->operative_unit->registers->registers[5]
                  << " and " << processor->operative_unit->registers->registers[6] << std::endl;

        // Continue for more cycles to finish the max algorithm
        run_cycles(60);

        // Display result of max value calculation
        std::cout << "Maximum value result (in R7): " << processor->operative_unit->registers->registers[7] << std::endl;
        std::cout << "Expected result: 50" << std::endl;
        std::cout << "Test " << (processor->operative_unit->registers->registers[7] == 50 ? "PASSED" : "FAILED") << std::endl;

        processor->operative_unit->registers->registers[4] = 1;   // R4 = 1 (decremento)
        processor->operative_unit->registers->registers[8] = 7;   // R8 = 7 (multiplicando)
        processor->operative_unit->registers->registers[9] = 6;   // R9 = 6 (multiplicador)

        print_header("Running Multiplication Algorithm");
        std::cout << "Multiplicando: " << processor->operative_unit->registers->registers[8]
                  << ", Multiplicador: " << processor->operative_unit->registers->registers[9] << std::endl;

        run_cycles(200);

        std::cout << "Multiplication result (in R10): " << processor->operative_unit->registers->registers[10] << std::endl;
        std::cout << "Expected result: 42" << std::endl;
        std::cout << "Test " << (processor->operative_unit->registers->registers[10] == 42 ? "PASSED" : "FAILED") << std::endl;

        print_header("All Tests Completed");
        print_registers();

        sc_stop();
    }

    void run_cycles(int count) {
        for (int i = 0; i < count; i++) {
            wait(10, SC_NS);
        }
    }

    // Set up instruction memory with both algorithm implementations
    void setup_memory() {
        // Get access to the instruction memory
        InstructionMemory* imem = processor->operative_unit->instruction_memory;

        // Clear instruction memory first
        for (int i = 0; i < 256; i++) {
            imem->memory[i] = 0;
        }

        // 2: CMP R1, R3
        imem->memory[2] = (Opcode::CMP << 26) | (1 << 21) | (3 << 16) | (0 << 11);
        // 3: NOP (ADD R0, R0, R0)
        imem->memory[3] = (Opcode::ADD << 26) | (0 << 21) | (0 << 16) | (0 << 11);
        // 4: JN 6
        imem->memory[4] = (Opcode::JN << 26) | (6);
        // 5: J 8
        imem->memory[5] = (Opcode::J << 26) | (8);
        // 6: SUB R2, R3, R1
        imem->memory[6] = (Opcode::SUB << 26) | (3 << 21) | (1 << 16) | (2 << 11);
        // 7: J 8
        imem->memory[7] = (Opcode::J << 26) | (9);

        imem->memory[8] = (Opcode::ADD << 26) | (0 << 21) | (1 << 16) | (2 << 11);


        // 9: CMP R5, R6
        imem->memory[9] = (Opcode::CMP << 26) | (5 << 21) | (6 << 16) | (0 << 11);
        // 10: NOP
        imem->memory[10] = (Opcode::ADD << 26) | (0 << 21) | (0 << 16) | (0 << 11);
        // 11: JN 13
        imem->memory[11] = (Opcode::JN << 26) | (13);
        // 12: ADD R7, R5, R0
        imem->memory[12] = (Opcode::ADD << 26) | (5 << 21) | (0 << 16) | (7 << 11);
        // 13: J 14
        imem->memory[13] = (Opcode::J << 26) | (14);
        // 14: ADD R7, R6, R0
        imem->memory[14] = (Opcode::ADD << 26) | (6 << 21) | (0 << 16) | (7 << 11);


        // 15: CLR R10 (resultado = 0)
        imem->memory[15] = (Opcode::ADD << 26) | (0 << 21) | (0 << 16) | (10 << 11); // R10 = 0
        // 16: CMP R9, R0 (verifica se multiplicador == 0)
        imem->memory[16] = (Opcode::CMP << 26) | (9 << 21) | (0 << 16) | (0 << 11);
        imem->memory[17] = (Opcode::ADD << 26) | (0 << 21) | (0 << 16) | (0 << 11);
        // 16: JN 22 (se R9 <= 0, pula para o fim)
        imem->memory[18] = (Opcode::JN << 26) | (22);
        // 19: ADD R10, R10, R8 (resultado += multiplicando)
        imem->memory[19] = (Opcode::ADD << 26) | (10 << 21) | (8 << 16) | (10 << 11);
        // 20: SUB R9, R9, R4 (decrementa multiplicador; R4 deve ser 1)
        imem->memory[20] = (Opcode::SUB << 26) | (9 << 21) | (4 << 16) | (9 << 11);
        // 21: J 16 (volta para o teste do loop)
        imem->memory[21] = (Opcode::J << 26) | (16);
        // 23: (fim do algoritmo)
        imem->memory[22] = (Opcode::ADD << 26) | (0 << 21) | (0 << 16) | (0 << 11);

    }

    SC_CTOR(AlgorithmsTB) {
        // Initialize processor
        processor = new Processor("processor");
        processor->clk(clk);

        // Create trace file
        tf = sc_create_vcd_trace_file("waveforms/algorithms_tb");
        if (!tf) {
            std::cerr << "Error: Could not create waveform file." << std::endl;
            return;
        }

        // Add signals to trace
        sc_trace(tf, clk, "clk");
        sc_trace(tf, processor->pc_value, "pc_value");
        sc_trace(tf, processor->opcode, "opcode");
        sc_trace(tf, processor->alu_result, "alu_result");
        sc_trace(tf, processor->alu_zero, "alu_zero");
        sc_trace(tf, processor->alu_negative, "alu_negative");

        SC_THREAD(run_test);
    }

    ~AlgorithmsTB() {
        sc_close_vcd_trace_file(tf);
        delete processor;
    }
};

int sc_main(int argc, char* argv[]) {
    AlgorithmsTB tb("algorithms_testbench");
    sc_start();
    return 0;
}

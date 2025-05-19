#include <systemc.h>
#include "../src/instruction_memory.h"

SC_MODULE(InstructionMemoryTB) {
    sc_signal<sc_uint<8>> address;
    sc_signal<bool> inst_mem_read;
    sc_signal<sc_uint<32>> fetched_instruction;

    InstructionMemory* imem;

    void test_process() {
        // Inicialização
        address = 0;
        inst_mem_read = false;
        wait(10, SC_NS);

        // Solicita leitura do endereço 0
        inst_mem_read = true;
        wait(10, SC_NS);
        inst_mem_read = false;
        wait(10, SC_NS);

        // Exibe a instrução lida
        std::cout << "Instrução lida do endereço 0: 0x"
                  << imem->fetched_instruction.read().to_string(SC_BIN) << std::endl;

        // Finaliza simulação
        sc_stop();
    }

    SC_CTOR(InstructionMemoryTB) {
        imem = new InstructionMemory("instruction_memory");
        imem->address(address);
        imem->inst_mem_read(inst_mem_read);
        imem->fetched_instruction(fetched_instruction);

        SC_THREAD(test_process);
    }

    ~InstructionMemoryTB() {
        delete imem;
    }
};

int sc_main(int argc, char* argv[]) {
    InstructionMemoryTB tb("tb");

    // Create VCD trace file
    sc_trace_file* tf = sc_create_vcd_trace_file("waveforms/instruction_memory_tb");
    if (!tf) {
        std::cout << "Error: Could not create trace file" << std::endl;
        return 1;
    }

    // Trace the signals
    sc_trace(tf, tb.address, "address");
    sc_trace(tf, tb.inst_mem_read, "inst_mem_read");
    sc_trace(tf, tb.fetched_instruction, "fetched_instruction");

    sc_start();

    // Close trace file
    sc_close_vcd_trace_file(tf);

    return 0;
}

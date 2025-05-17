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
    sc_start();
    return 0;
}

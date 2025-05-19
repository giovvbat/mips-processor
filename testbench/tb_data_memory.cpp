#include <systemc.h>
#include "../src/data_memory.h"

SC_MODULE(DataMemoryTB) {
    sc_signal<bool> data_mem_read;
    sc_signal<bool> data_mem_write;
    sc_signal<sc_uint<8>> address;
    sc_signal<sc_uint<32>> write_data;
    sc_signal<sc_uint<32>> mem_data;

    DataMemory* mem;

    void test_process() {
        // Inicialização
        data_mem_read = false;
        data_mem_write = false;
        address = 0;
        write_data = 0;
        wait(10, SC_NS);

        // Escreve valor 0xDEADBEEF no endereço 10
        address = 10;
        write_data = 0xDEADBEEF;
        data_mem_write = true;
        wait(10, SC_NS);
        data_mem_write = false;
        wait(10, SC_NS);

        // Lê do endereço 10
        data_mem_read = true;
        wait(10, SC_NS);
        data_mem_read = false;
        wait(10, SC_NS);

        // Exibe o valor lido
        std::cout << "Valor lido do endereço 10: 0x"
                  << std::hex << mem->mem_data.read() << std::endl;

        // Escreve valor 0xDEADBEEF no endereço 10
        address = 10;
        write_data = 0xFEE1DEAD;
        data_mem_write = true;
        wait(10, SC_NS);
        data_mem_write = false;
        wait(10, SC_NS);

        // Lê do endereço 10
        data_mem_read = true;
        wait(10, SC_NS);
        data_mem_read = false;
        wait(10, SC_NS);

        // Exibe o valor lido
        std::cout << "Valor lido do endereço 10: 0x"
        << std::hex << mem->mem_data.read() << std::endl;

        // Finaliza simulação
        sc_stop();
    }

    SC_CTOR(DataMemoryTB) {
        mem = new DataMemory("data_memory");
        mem->data_mem_read(data_mem_read);
        mem->data_mem_write(data_mem_write);
        mem->address(address);
        mem->write_data(write_data);
        mem->mem_data(mem_data);

        SC_THREAD(test_process);
    }

    ~DataMemoryTB() {
        delete mem;
    }
};

int sc_main(int argc, char* argv[]) {
    DataMemoryTB tb("tb");

    // Create VCD trace file
    sc_trace_file* tf = sc_create_vcd_trace_file("waveforms/data_memory_tb");
    if (!tf) {
        std::cout << "Error: Could not create trace file" << std::endl;
        return 1;
    }

    // Trace the signals
    sc_trace(tf, tb.data_mem_read, "data_mem_read");
    sc_trace(tf, tb.data_mem_write, "data_mem_write");
    sc_trace(tf, tb.address, "address");
    sc_trace(tf, tb.write_data, "write_data");
    sc_trace(tf, tb.mem_data, "mem_data");

    sc_start();

    // Close trace file
    sc_close_vcd_trace_file(tf);

    return 0;
}

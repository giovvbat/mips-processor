#include <systemc.h>
#include "../src/pc.h"

SC_MODULE(PCTB) {
    sc_signal<bool> pc_source;
    sc_signal<bool> pc_write;
    sc_signal<sc_uint<8>> jump_address;
    sc_signal<sc_uint<8>> pc_value;

    PC* pc_inst;

    void test_process() {
        // Inicialização
        pc_write = false;
        pc_source = false;
        jump_address = 0;
        std::cout << "PC no início: " << pc_inst->pc.read().to_uint() << std::endl;
        wait(10, SC_NS);

        std::cout << "PC antes do incremento: " << pc_inst->pc.read().to_uint() << std::endl;

        // Incrementa PC normalmente
        pc_write = true;
        wait(10, SC_NS);
        pc_write = false;
        wait(10, SC_NS);
        std::cout << "PC após incremento: " << pc_inst->pc.read().to_uint() << std::endl;

        // Incrementa novamente
        pc_write = true;
        wait(10, SC_NS);
        pc_write = false;
        wait(10, SC_NS);
        std::cout << "PC após segundo incremento: " << pc_inst->pc.read().to_uint() << std::endl;

        // Faz um salto para o endereço 42
        pc_source = true;
        jump_address = 42;
        pc_write = true;
        wait(10, SC_NS);
        pc_write = false;
        wait(10, SC_NS);
        std::cout << "PC após salto para 42: " << pc_inst->pc.read().to_uint() << std::endl;

        sc_stop();
    }

    SC_CTOR(PCTB) {
        pc_inst = new PC("pc");
        pc_inst->pc_source(pc_source);
        pc_inst->pc_write(pc_write);
        pc_inst->jump_address(jump_address);
        pc_inst->pc(pc_value);

        SC_THREAD(test_process);
    }

    ~PCTB() {
        delete pc_inst;
    }
};

int sc_main(int argc, char* argv[]) {
    PCTB tb("tb");
    sc_start();
    return 0;
}

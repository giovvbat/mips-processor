#include <sysc/communication/sc_clock.h>
#include <systemc.h>
#include "processor.h"

int sc_main(int argc, char* argv[]) {
    Processor cpu("cpu");

    sc_clock clk("clk", 10, SC_NS);
    cpu.clk(clk);

    cout << "Iniciando simulação..." << endl;
    sc_start(12800, SC_NS);

    for (int i = 0; i < 32; i++)
        cout << "r" << i << " = " << cpu.operative_unit->registers->registers[i] << endl;

    return 0;
}

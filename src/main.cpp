#include <systemc.h>
#include "processor.h"

int sc_main(int argc, char* argv[]) {
    Processor cpu("cpu");

    cout << "Iniciando simulação..." << endl;
    sc_start(10, SC_NS);

    cout << "Valor final de r3 (registrador 3): " << cpu.registers[3].read() << endl;

    cout << "Valor final de r5 (registrador 5): " << cpu.registers[5].read() << endl;

    return 0;
}

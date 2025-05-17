#include <systemc.h>
#include <iostream>

SC_MODULE(TestClock) {
    sc_in<bool> clk;
    
    SC_CTOR(TestClock) {
        // Testing available methods on sc_in<bool> clk
        SC_METHOD(method);
        sensitive << clk;
        std::cout << "Available methods: pos() method registered" << std::endl;
    }
    
    void method() {
        std::cout << "Clock edge detected" << std::endl;
    }
};

int sc_main(int argc, char* argv[]) {
    sc_clock clock("clock", 10, SC_NS);
    TestClock test("test");
    test.clk(clock);
    sc_start(100, SC_NS);
    return 0;
}


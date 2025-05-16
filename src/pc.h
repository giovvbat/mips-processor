#pragma once

#include <sysc/kernel/sc_module.h>
#include <systemc.h>

SC_MODULE(PC) {
    sc_in<bool> pc_source;
    sc_in<bool> pc_write;
    sc_in<sc_uint<8>> jump_address;

    sc_signal<sc_uint<8>> pc;

    SC_CTOR(PC) {
        SC_METHOD(update);
        sensitive << pc_write.pos();
    }

    void update() {
        if (pc_source.read()) {
            pc.write(jump_address.read());
        } else {
            // if (pc.read() < 0) {
                pc.write(pc.read() + 1);
            // }
        }
    }
};

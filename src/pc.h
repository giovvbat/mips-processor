#pragma once

#include <sysc/kernel/sc_module.h>
#include <systemc.h>

SC_MODULE(PC) {
    sc_in<bool> pc_source{"pc_source"};
    sc_in<bool> pc_write{"pc_write"};
    sc_in<sc_uint<8>> jump_address{"jump_address"};

    sc_out<sc_uint<8>> pc;

    SC_CTOR(PC) {
        SC_METHOD(update);
        sensitive << pc_write.pos();
        dont_initialize();
    }

    void update() {
        if (pc_source.read()) {
            pc.write(jump_address.read());
        } else {
            pc.write(pc.read() + 1);
        }
    }
};

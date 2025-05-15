#include <systemc.h>
#include "alu.h"
#include "opcode.h"

SC_MODULE(ALUTestbench) {
    sc_signal<sc_uint<32>> operand_a_sig, operand_b_sig;
    sc_signal<sc_uint<6>> opcode_sig;
    sc_signal<sc_uint<32>> result_sig;
    sc_signal<bool> zero_sig, negative_sig;

    ALU* alu;

    void test() {
        // Vetor de testes: {a, b, opcode, descrição}
        struct TestVector {
            sc_uint<32> a;
            sc_uint<32> b;
            Opcode opcode;
            const char* desc;
        } tests[] = {
            {10, 5, Opcode::ADD, "ADD 10 + 5"},
            {10, 5, Opcode::SUB, "SUB 10 - 5"},
            {10, 5, Opcode::AND, "AND 10 & 5"},
            {10, 5, Opcode::OR,  "OR  10 | 5"},
            {10, 5, Opcode::XOR, "XOR 10 ^ 5"},
            {10, 0, Opcode::NOT, "NOT ~10"},
            {10, 10, Opcode::CMP, "CMP 10 - 10"},
            {0, 0, (Opcode)99, "Invalid opcode"},
        };

        for (auto& t : tests) {
            operand_a_sig.write(t.a);
            operand_b_sig.write(t.b);
            opcode_sig.write(t.opcode);

            wait(1, SC_NS);

            std::cout << "[ " << t.desc << " ] "
                      << "a=" << t.a
                      << " b=" << t.b
                      << " -> result=" << result_sig.read()
                      << " zero=" << zero_sig.read()
                      << " neg=" << negative_sig.read()
                      << std::endl;
        }

        sc_stop(); // finaliza simulação
    }

    SC_CTOR(ALUTestbench) {
        alu = new ALU("alu");
        alu->operand_a(operand_a_sig);
        alu->operand_b(operand_b_sig);
        alu->opcode(opcode_sig);
        alu->result(result_sig);
        alu->zero(zero_sig);
        alu->negative(negative_sig);

        SC_THREAD(test);
    }

    ~ALUTestbench() {
        delete alu;
    }
};

int sc_main(int argc, char* argv[]) {
    ALUTestbench tb("tb");
    sc_start();
    return 0;
}

#include <systemc.h>
#include "processor.h"
#include "opcode.h"

SC_MODULE(Testbench) {
    sc_clock clk;
    Processor* processor;

    SC_CTOR(Testbench)
        : clk("clk", 10, SC_NS)
    {
        processor = new Processor("Processor");
        processor->clk(clk);

        SC_THREAD(run);
    }

    void run() {
        // Reset estado
        for (int i = 0; i < 256; ++i) {
            processor->instruction_memory[i] = 0;
            processor->data_memory[i] = 0;
        }

        for (int i = 0; i < 32; ++i) {
            processor->registers[i] = 0;
        }

        // Testes
        int i = 0;

        // R1 = 10
        processor->registers[1] = 10;
        // R2 = 5
        processor->registers[2] = 5;

        // ADD R1 + R2 -> R3  ; R3 = 15
        processor->instruction_memory[i++] = (Opcode::ADD << 26) | (1 << 21) | (2 << 16) | (3 << 11);

        // SUB R1 - R2 -> R4  ; R4 = 5
        processor->instruction_memory[i++] = (Opcode::SUB << 26) | (1 << 21) | (2 << 16) | (4 << 11);

        // AND R1 & R2 -> R5  ; R5 = 0
        processor->instruction_memory[i++] = (Opcode::AND << 26) | (1 << 21) | (2 << 16) | (5 << 11);

        // OR R1 | R2 -> R6   ; R6 = 15
        processor->instruction_memory[i++] = (Opcode::OR << 26) | (1 << 21) | (2 << 16) | (6 << 11);

        // XOR R1 ^ R2 -> R7  ; R7 = 15
        processor->instruction_memory[i++] = (Opcode::XOR << 26) | (1 << 21) | (2 << 16) | (7 << 11);

        // NOT ~R1 -> R8      ; R8 = ~10
        processor->instruction_memory[i++] = (Opcode::NOT << 26) | (1 << 21) | (0 << 16) | (8 << 11);

        // CMP R1, R1         ; Z = 1, N = 0
        processor->instruction_memory[i++] = (Opcode::CMP << 26) | (1 << 21) | (1 << 16);

        // JZ to index 10 (should jump) if zero
        processor->instruction_memory[i++] = (Opcode::JZ << 26) | 10;

        // ADD R1 + R2 -> R9  ; Skipped if JZ worked
        processor->instruction_memory[i++] = (Opcode::ADD << 26) | (1 << 21) | (2 << 16) | (9 << 11);

        // ADD R1 + R2 -> R10 ; This is instruction 10 (jump target)
        processor->instruction_memory[i++] = (Opcode::ADD << 26) | (1 << 21) | (2 << 16) | (10 << 11);

        // ST R1 -> mem[20]
        processor->instruction_memory[i++] = (Opcode::ST << 26) | (1 << 21) | 20;

        // LD mem[20] -> R11
        processor->instruction_memory[i++] = (Opcode::LD << 26) | (0 << 21) | (11 << 11) | 20;

        // CMP R2 - R1 ; should set negative
        processor->instruction_memory[i++] = (Opcode::CMP << 26) | (2 << 21) | (1 << 16);

        // JN to index 15 (should jump)
        processor->instruction_memory[i++] = (Opcode::JN << 26) | 15;

        // ADD R1 + R2 -> R12 ; skipped if JN worked
        processor->instruction_memory[i++] = (Opcode::ADD << 26) | (1 << 21) | (2 << 16) | (12 << 11);

        // ADD R1 + R1 -> R13 ; this is 15
        processor->instruction_memory[i++] = (Opcode::ADD << 26) | (1 << 21) | (1 << 16) | (13 << 11);

        // Espera o processador executar tudo
        wait(500, SC_NS);
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");

    // Opcional: VCD trace
    sc_trace_file* tf = sc_create_vcd_trace_file("processor_trace");
    sc_trace(tf, tb.clk, "clk");
    sc_trace(tf, tb.processor->pc, "pc");
    sc_trace(tf, tb.processor->fetched_instruction, "fetched_instruction");
    sc_trace(tf, tb.processor->alu_result, "alu_result");
    sc_trace(tf, tb.processor->alu_zero, "alu_zero");
    sc_trace(tf, tb.processor->alu_negative, "alu_negative");

    sc_start();
    sc_close_vcd_trace_file(tf);
    return 0;
}

reg:
	g++ testbench/tb_registers.cpp -o build/tb_registers -I/usr/local/include -L/usr/local/lib -lsystemc -lm
	build/tb_registers

alu:
	g++ testbench/tb_alu.cpp -o build/tb_alu -I/usr/local/include -L/usr/local/lib -lsystemc -lm
	build/tb_alu

data_mem:
	g++ testbench/tb_data_memory.cpp -o build/tb_data_memory -I/usr/local/include -L/usr/local/lib -lsystemc -lm
	build/tb_data_memory

inst_mem:
	g++ testbench/tb_instruction_memory.cpp -o build/tb_instruction_memory -I/usr/local/include -L/usr/local/lib -lsystemc -lm
	build/tb_instruction_memory

pc:
	g++ testbench/tb_pc.cpp -o build/tb_pc -I/usr/local/include -L/usr/local/lib -lsystemc -lm
	build/tb_pc

control:
	g++ testbench/tb_control_unit.cpp -o build/tb_control_unit -I/usr/local/include -L/usr/local/lib -lsystemc -lm
	build/tb_control_unit

operative:
	g++ testbench/tb_operative_unit.cpp -o build/tb_operative_unit -I/usr/local/include -L/usr/local/lib -lsystemc -lm
	build/tb_operative_unit

processor:
	g++ testbench/tb_processor.cpp -o build/tb_processor -I/usr/local/include -L/usr/local/lib -lsystemc -lm
	build/tb_processor

algorithms:
	g++ testbench/tb_algorithms.cpp -o build/tb_algorithms -I/usr/local/include -L/usr/local/lib -lsystemc -lm
	build/tb_algorithms

main:
	g++ src/main.cpp -o build/main -I/usr/local/include -L/usr/local/lib -lsystemc -lm
	build/main

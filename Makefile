all:

monoprocessor:
	gcc -o trabSO control_structures/*.c data_structures/*.c monocore/*.c schedulers/*.c -lm

multiprocessor:
	gcc -o trabSO control_structures/*.c data_structures/*.c multicore/*.c schedulers/*.c -lm

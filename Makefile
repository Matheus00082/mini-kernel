all:

monoprocessador:
	gcc -o trabSO control_structures/*.c data_structures/*.c monocore/*.c schedulers/*.c -lm -pthread

multiprocessador:
	gcc -o trabSO control_structures/*.c data_structures/*.c multicore/*.c schedulers/*.c -lm -pthread

monoprocessor: monoprocessador

multiprocessor: multiprocessador

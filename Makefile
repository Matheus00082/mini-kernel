all:

monoprocessor:
	gcc -o trabalho control_structures/*.c data_structures/*.c monocore/*.c schedulers/*.c -lm

multiprocessor:
	gcc -o trabalho control_structures/*.c data_structures/*.c multicore/*.c schedulers/*.c -lm
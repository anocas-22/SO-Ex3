all:
	make -C CircuitRouter-SeqSolver
	make -C CircuitRouter-AdvShell
	make -C CircuitRouter-Client

clean:
	make clean -C CircuitRouter-SeqSolver
	make clean -C CircuitRouter-AdvShell
	make clean -C CircuitRouter-Client

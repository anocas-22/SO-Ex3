#!/bin/bash
make
mkdir /tmp/ex3
cp -r ../CircuitRouter-AdvShell /tmp/ex3/
cp -r ../CircuitRouter-SeqSolver /tmp/ex3/
cd /tmp/ex3/CircuitRouter-AdvShell
./AdvShell

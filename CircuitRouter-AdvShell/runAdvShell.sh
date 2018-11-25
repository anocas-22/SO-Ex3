#!/bin/bash
make
mkdir /tmp/ex3
cp -r ../CircuitRouter-SeqSolver /tmp/
mv AdvShell /tmp/ex3/
cd /tmp/ex3
./AdvShell

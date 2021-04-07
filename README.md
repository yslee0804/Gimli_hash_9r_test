# A New Preimage Attack on Gimli-Hash

This repository contains implementations

# Results

# List of files
- Makefile
- README.md
- gimli.cpp : modified Gimli codes from Gimli reference implementation
- gimli.hpp : modified Gimli codes from Gimli reference implementation
- gimli_9r_solver.cpp /.hpp : solving problems with SAT solver
- eq_test.cpp : match and time test for functions in "gimli_9r_solver.cpp"

# Requirements

The folder of "cryptominisat" obtained from "https://github.com/msoos/cryptominisat" should be download and build then modify directions in "makefile"

# Testing environment

The testing is performed on the following testing environment:

- Ubuntu 16.04.4 LTS
- g++ 5.5.0

# compilation

test_O2 : run "eq_test.cpp" with -O2
test_O3 : run "eq_test.cpp" with -O3 

# Contact

For more information on this code, please contact the author Youngsung Kim via [E-mail](yslee.CIST@gmail.com)


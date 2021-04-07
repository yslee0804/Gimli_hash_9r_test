# A New Preimage Attack on Gimli-Hash

This repository contains implementations

논문 Abstract 정도의 소개 (공격 결과), 본 코드는 전체 공격 중 어떤 과정인지 소개

# Results

이 코드가 전체 공격 중에서 테스트하고자 하는 것이 무엇이고, 테스트 결과는 무엇인지?

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

# Compilation

test_O2 : run "eq_test.cpp" with -O2
test_O3 : run "eq_test.cpp" with -O3 

# Contact

For more information on this code, please contact the author Youngsung Kim via [E-mail](yslee.CIST@gmail.com)


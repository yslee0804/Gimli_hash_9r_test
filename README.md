# A New Preimage Attack on Gimli-Hash

This repository contains implementations about solving equations which replace precomputation table in preimage attack on 5-9-round Gimli-Hash.
Our code tests two things:
1. Average time to solve the following equations using CryptoMiniSAT
2. Average number of solutions for each equations

The equations as follow
Eq1 : Given h_03, c, f, and x, find a, b such that SP(h_03,a,b) = (c,d,e) and SP^2(x,d,e) = (f,g,h)
Eq2 : Given h_02, d, and e, find c such that SP(h_02,a,b)=(c,d,e)
Eq3 : Given b, c, and d, find a, e, and f such that SP^2(a,b,c)=(d,e,f)
Eq4 : Given a, e, and f, find b, c, and d such that SP^2(a,b,c)=(d,e,f)

# Results
All equations has average about 1 solutions
The average running time as follow :

  Full-round(24-round) Gimli : 0.5875 micro second
  Eq1 : 343200.2 micro second
  Eq2 : 125.9327 micro second
  Eq3 : 763.1252 micro second
  Eq4 : 171148.6 micro second

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

- test_O2 : run "eq_test.cpp" with -O2
- test_O3 : run "eq_test.cpp" with -O3 

# Contact

For more information on this code, please contact the author Youngsung Kim via [E-mail](yslee.CIST@gmail.com)


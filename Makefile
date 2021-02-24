all: test


.PHONY: clean

clean:
	rm -f *.o

%.o: %.cpp
	g++ -o $@ -c $< -Wall -pedantic -std=c++11 -O2 -g

test_debug:
	g++ -I../cryptominisat-5.8.0/build/include -L../cryptominisat-5.8.0/build/lib/ -Wl,-rpath=../cryptominisat-5.8.0/build/lib/ gimli.cpp gimli_9r_solver.cpp eq_test.cpp -o $@ -lcryptominisat5 -Wall -pedantic -std=c++11 -O0 -g 
	./test_debug

test_O2:
	g++ -I../cryptominisat-5.8.0/build/include -L../cryptominisat-5.8.0/build/lib/ -Wl,-rpath=../cryptominisat-5.8.0/build/lib/ gimli.cpp gimli_9r_solver.cpp eq_test.cpp -o $@ -lcryptominisat5 -Wall -pedantic -std=c++11 -O2 -g 
	./test_O2

test_O3:
	g++ -I../cryptominisat-5.8.0/build/include -L../cryptominisat-5.8.0/build/lib/ -Wl,-rpath=../cryptominisat-5.8.0/build/lib/ gimli.cpp gimli_9r_solver.cpp eq_test.cpp -o $@ -lcryptominisat5 -Wall -pedantic -std=c++11 -O3 -g 
	./test_O3
all: mt st vs

clean:
	rm mt st vs

mt: main_multithreaded.cc wolves.cc wolves.h
	$(CXX) -std=c++14 -O3 -march=native -DNUM_THREADS=4 main_multithreaded.cc wolves.cc -o $@

st: main_singlethreaded.cc wolves.cc wolves.h
	$(CXX) -std=c++14 -O3 -march=native main_singlethreaded.cc wolves.cc -o $@

vs: main_verifysolution.cc
	$(CXX) -std=c++14 -O3 -march=native main_verifysolution.cc -o $@

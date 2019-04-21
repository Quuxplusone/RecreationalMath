all: mt st

clean:
	rm mt st

mt: main_multithreaded.cc wolves.cc wolves.h
	$(CXX) -std=c++14 -O3 -march=native -DNUM_THREADS=4 main_multithreaded.cc wolves.cc -o $@

st: main_singlethreaded.cc wolves.cc wolves.h
	$(CXX) -std=c++14 -O3 -march=native -DPRINT_CANDIDATE_WOLVES main_singlethreaded.cc wolves.cc -o $@

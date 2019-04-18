all: mt st

clean:
	rm mt st

mt: main_multithreaded.cc wolves.cc wolves.h
	$(CXX) -std=c++14 -O3 -DNUM_THREADS=4 main_multithreaded.cc wolves.cc -o $@

st: main_singlethreaded.cc wolves.cc wolves.h
	$(CXX) -std=c++14 -O3 -DPRINT_CANDIDATE_WOLVES main_singlethreaded.cc wolves.cc -o $@

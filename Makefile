all: cm mt st vs

clean:
	rm cm mt st vs

cm: canonicalize_matrix.cc
	$(CXX) -std=c++14 -O3 -march=native canonicalize_matrix.cc -lnauty -o $@

mt: main_multithreaded.cc wolves.cc wolves.h
	$(CXX) -std=c++14 -O3 -march=native -DNUM_THREADS=4 main_multithreaded.cc wolves.cc -o $@

st: main_singlethreaded.cc wolves.cc wolves.h
	$(CXX) -std=c++14 -O3 -march=native main_singlethreaded.cc wolves.cc -o $@

vs: main_verifysolution.cc
	$(CXX) -std=c++14 -O3 -march=native main_verifysolution.cc -o $@

all: cm mt st vs wolfy

clean:
	rm cm mt st vs wolfy

cm: canonicalize_matrix.cpp
	$(CXX) -std=c++14 -O3 -march=native canonicalize_matrix.cpp -lnauty -o $@

mt: main_multithreaded.cpp wolves.cpp wolves.h
	$(CXX) -std=c++14 -O3 -march=native -DNUM_THREADS=4 main_multithreaded.cpp wolves.cpp -o $@

st: main_singlethreaded.cpp wolves.cpp wolves.h
	$(CXX) -std=c++14 -O3 -march=native main_singlethreaded.cpp wolves.cpp -o $@

vs: main_verifysolution.cpp
	$(CXX) -std=c++14 -O3 -march=native main_verifysolution.cpp -o $@

wolfy: main_wolfy.cpp verify_strategy.cpp verify_strategy.h
	$(CXX) -std=c++14 -O3 -march=native main_wolfy.cpp verify_strategy.cpp -o $@

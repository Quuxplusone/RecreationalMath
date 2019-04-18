st: main_singlethreaded.cc wolves.cc wolves.h
	$(CXX) -std=c++14 -O3 main_singlethreaded.cc wolves.cc -o $@

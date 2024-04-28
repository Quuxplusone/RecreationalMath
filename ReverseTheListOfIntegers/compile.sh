~/llvm-project/build2/bin/clang++ nssolver.cpp -std=c++23 -DN=$1 -DS=$2 -O2 -o ./a-$1-$2.out
./a-$1-$2.out


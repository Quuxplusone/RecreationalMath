
all: play-against-oracle make-oracle

play-against-oracle: play-against-oracle.cpp shared-code.h
	$(CXX) -std=c++20 -O2 -DB=3 $(CXXFLAGS) $< -o $@

make-oracle: make-oracle.cpp shared-code.h
	$(CXX) -std=c++20 -O2 -DB=3 $(CXXFLAGS) $< -o $@

clean:
	rm -f play-against-oracle make-oracle

.PHONY: clean

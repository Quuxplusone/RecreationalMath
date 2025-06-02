
all: compress-oracle decompress-oracle make-oracle play-against-oracle

compress-oracle: compress-oracle.cpp shared-code.h
	$(CXX) -std=c++20 -O2 $(CXXFLAGS) $< -o $@

decompress-oracle: decompress-oracle.cpp shared-code.h
	$(CXX) -std=c++20 -O2 $(CXXFLAGS) $< -o $@

make-oracle: make-oracle.cpp shared-code.h
	$(CXX) -std=c++20 -O2 $(CXXFLAGS) $< -o $@

play-against-oracle: play-against-oracle.cpp shared-code.h
	$(CXX) -std=c++20 -O2 $(CXXFLAGS) $< -o $@

clean:
	rm -f compress-oracle decompress-oracle make-oracle play-against-oracle

.PHONY: clean

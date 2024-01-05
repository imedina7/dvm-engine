CFLAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

DVMEngine: src/main.cpp
	g++ $(CFLAGS) -o DVMEngine src/main.cpp $(LDFLAGS)

.PHONY: test clean
test: DVMEngine
	./DVMEngine

clean: DVMEngine
	rm -f DVMEngine
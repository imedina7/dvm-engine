include .env

CFLAGS = -std=c++17 -O2
LDFLAGS = -L$(VULKAN_SDK_PATH) -lglfw -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi -lvulkan

DVMEngine: src/*.cpp src/*.hpp
	g++ $(CFLAGS) src/*.cpp -o DVMEngine  $(LDFLAGS)

.PHONY: test clean
test: DVMEngine
	./DVMEngine

clean: 
	rm -f DVMEngine
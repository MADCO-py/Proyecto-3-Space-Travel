# Generated with OpenAI Codex. Raylib itself is third-party zlib-licensed code.
CC ?= cc
RAYLIB = zig-pkg/raylib-5.5/src
CFLAGS = -std=c99 -O2 -Wall -Wextra -Wpedantic -I$(RAYLIB) -Isrc
LDLIBS = $(RAYLIB)/libraylib.a -lGL -lm -lpthread -ldl -lrt -lX11
.PHONY: all run test clean setup
all: build/heliograph
setup: $(RAYLIB)/libraylib.a
$(RAYLIB)/libraylib.a:
	./tools/setup.sh
build/heliograph: src/main.c src/sim.c src/sim.h src/obj.c src/obj.h $(RAYLIB)/libraylib.a
	mkdir -p build
	$(CC) $(CFLAGS) src/main.c src/sim.c src/obj.c -o $@ $(LDLIBS)
run: all
	./build/heliograph
test: $(RAYLIB)/libraylib.a
	mkdir -p build
	$(CC) $(CFLAGS) tests/test_sim.c src/sim.c src/obj.c -o build/test_sim $(LDLIBS)
	./build/test_sim
clean:
	rm -rf build

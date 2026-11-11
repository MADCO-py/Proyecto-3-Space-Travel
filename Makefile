# Generated with OpenAI Codex. Raylib itself is third-party zlib-licensed code.
CC ?= cc
RAYLIB = zig-pkg/raylib-5.5/src
CFLAGS = -D_POSIX_C_SOURCE=200809L -std=c99 -O2 -Wall -Wextra -Wpedantic -I$(RAYLIB) -Isrc
LDLIBS = $(RAYLIB)/libraylib.a -lGL -lm -lpthread -ldl -lrt -lX11
.PHONY: all run test clean setup headless
all: build/heliograph
setup: $(RAYLIB)/libraylib.a
$(RAYLIB)/libraylib.a:
	./tools/setup.sh
build/heliograph: src/main.c src/sim.c src/sim.h src/obj.c src/obj.h $(RAYLIB)/libraylib.a
	mkdir -p build
	$(CC) $(CFLAGS) src/main.c src/sim.c src/obj.c -o $@ $(LDLIBS)
run: all
	./tools/run.sh
test: $(RAYLIB)/libraylib.a
	mkdir -p build
	$(CC) $(CFLAGS) tests/test_sim.c src/sim.c src/obj.c -o build/test_sim $(LDLIBS)
	./build/test_sim
headless: all
	./build/heliograph --headless --near --frames 10 --capture build/cpu.ppm
clean:
	rm -rf build

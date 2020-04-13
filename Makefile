# This Makefile is for compiling apps with clang (to get IR)

# Use clang as gcc
# clang hello.c -o hello

# Generate IR (bitcode)
ir:
	clang -O0 -emit-llvm app.c -c -o app.bc
	@#clang -O3 -emit-llvm app.c -c -o app.bc
	@#llc app.bc -o app.s

# Take a look at the LLVM assembly code
# llvm-dis < app.bc

# Generate ASM file
asm:
	llc app.bc -o app.s
	llc new.bc -o new.s

clean:
	rm -f *.bc app.s tmp

# Compile our pass
pass:
	make -C build

# Use our pass
run:
	opt -load build/tmac_hello_pass/libjmpsafe.so -Jmpsafe < app.bc > new.bc

all: pass ir run asm

# This Makefile is for compiling apps with clang (to get IR)

# Use clang as gcc
# clang hello.c -o hello

# Generate IR (bitcode)
default:
	clang -O3 -emit-llvm app.c -c -o app.bc
	llc app.bc -o app.s

# Take a look at the LLVM assembly code
# llvm-dis < app.bc

# Generate ASM file
# llc app.bc -o app.s

clean:
	rm -f *.bc app.s

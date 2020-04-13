#include <stdio.h>

int bar(int n)
{
	// In IR: inline asm is treated as a CallInst
	// asm("nopl 0xf(%rax, %rax, 4)");
	printf("This is bar: %d\n", n);
	return n + 1;
}

void foo(int n)
{
	n = bar(n);
	printf("This is foo: %d\n", n);

	switch (n) {
	case 99:
		printf("Keep going!\n");
		break;
	case 100:
		printf("Perfect!\n");
		break;
	default:
		printf("Not bad!\n");
		break;
	}
}

int main()
{
	printf("Hello LLVM\n");
	foo(99);
	return 0;
}

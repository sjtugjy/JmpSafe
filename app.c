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
	case 101:
		printf("%d\n", __LINE__);
		break;
	case 102:
		printf("%d\n", __LINE__);
		break;
	case 103:
		printf("%d\n", __LINE__);
		break;
	case 109:
		printf("%d\n", __LINE__);
		break;
	case 210:
		printf("%d\n", __LINE__);
		break;
	default:
		printf("Not bad!\n");
		break;
	}
}

void __attribute__((noinline)) baz(int n)
{
	printf("I am baz\n");
}

int main()
{
	void (*fp)(int) = &baz;

	printf("Hello LLVM\n");
	foo(99);
	fp(0);
	return 0;
}

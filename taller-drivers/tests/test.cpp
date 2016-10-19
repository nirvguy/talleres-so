#include <iostream>
#include <string.h>

#define KRAND_MAX                  2147483648UL
#define KRAND_A                    1103515245UL
#define KRAND_C                    12345UL
#define CANT                       10
#define BUFFER_SIZE                1024

unsigned long int get_rand_number(unsigned long int n) {
	return (KRAND_A * n + KRAND_C) % KRAND_MAX;
}

void test_random_char() {
	char buffer[BUFFER_SIZE];
	char* p = buffer;
	unsigned long int n = 0;
	unsigned int k = BUFFER_SIZE / sizeof(int);
	for (unsigned int i=0; i<k; i++, p += sizeof(int)) {
		unsigned int t = n;
		strncpy(p, (char*) (&t), sizeof(int));
		n = get_rand_number(n);
	}

	if (BUFFER_SIZE % sizeof(int) > 0) {
		unsigned int t = n;
		strncpy(p, (char*) (&t), sizeof(int));
		n = get_rand_number(n);
	}

	fwrite(buffer, BUFFER_SIZE, 1, stdout);
	fflush(stdout);
	return;
}

int main() {
	test_random_char();
	return 0;
}

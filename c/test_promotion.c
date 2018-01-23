#include <stdio.h>

int main(int argc, char *argv[])
{
	int a = 1;
	float b = 1.0f;
	double c = 2.0;
	long d = 4L;
	char f = 'A';
	short g = 1;
	unsigned int h = 1;
	printf("sizeof('A') = %lu\n", sizeof('A'));
	printf("sizeof(a) = %lu\n", sizeof(a));
	printf("sizeof(b) = %lu\n", sizeof(b));
	printf("sizeof(c) = %lu\n", sizeof(c));
	printf("sizeof(d) = %lu\n", sizeof(d));
	printf("sizeof(f) = %lu\n", sizeof(f));
	printf("sizeof(f+f) = %lu\n", sizeof(f+f));
	printf("sizeof(f+g) = %lu\n", sizeof(f+g));
	printf("sizeof(a+f+g) = %lu\n", sizeof(a+f+g));
	printf("sizeof(f+g+h) = %lu\n", sizeof(f+g+h));
	printf("sizeof(a+d) = %lu\n", sizeof(a+d));
	c = a + b;
	d = a + 10;
	printf("c = %f\n", c);
	printf("d = %ld\n", d);

        int sa = -128;
        unsigned int ua = 5;
        if (sa < ua) {
          printf("%d < %u \n", sa, ua);
        } else if(sa == ua) {
          printf("%d == %u \n", sa, ua);
        } else {
          printf("Aha! Oops: %d > %u \n", sa, ua);
        }
	return 0;
}

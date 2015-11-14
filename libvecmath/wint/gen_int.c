#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void gen_mul(FILE *fd, int w)
{
	int i, j, k;

	fprintf(fd, "_math_wint_mul%d:\n", w*32);
	fprintf(fd, 
		"push ebp\n"
		"mov ebp, esp\n"
		"push ebx\n"
		"push esi\n"
		"push edi\n"
		"\n"
		"mov esi, [ebp+8]\n"
		"mov edi, [ebp+12]\n"
//		"mov ebx, [ebp+16]\n"
		"\n"
		"xor eax, eax\n\n"
	);

//	for(i=0; i<2*w; i++)
//		fprintf(fd, "mov [ebx+%d], eax\n", i*4);
//	fprintf(fd, "\n");

	fprintf(fd, "xor ecx, ecx\n");

//	a=0;
	for(i=0; i<w; i++)
	{
//		fprintf(fd, "push ebx\n");
		fprintf(fd, "xor ebx, ebx\n");
		for(j=0; j<=i; j++)
		{
//			a+=((int)a[j])*b[i-j];
			
			fprintf(fd, "mov eax, [esi+%d]\n", j*4);
			fprintf(fd, "mul [edi+%d]\n", (i-j)*4);
			fprintf(fd, "add ecx, eax\n");
			fprintf(fd, "adc ebx, edx\n");
			fprintf(fd, "\n");
		}

		fprintf(fd, "mov eax, ecx\n");
		fprintf(fd, "mov ecx, ebx\n");
//		fprintf(fd, "pop ebx\n");
		fprintf(fd, "mov ebx, [ebp+16]\n");
		fprintf(fd, "mov [ebx+%d], eax\n", i*4);
		fprintf(fd, "\n");

//		c[i]=a;
//		a>>=16;
	}
	for(; i<(2*w-1); i++)
	{
//		fprintf(fd, "push ebx\n");
		fprintf(fd, "xor ebx, ebx\n");

		k=(2*w)-i;
		for(j=0; j<k; j++)
		{
//			a+=((int)a[32-k+j])*b[31-j];

			fprintf(fd, "mov eax, [esi+%d]\n", (w-k+j)*4);
			fprintf(fd, "mul [edi+%d]\n", (w-1-j)*4);
			fprintf(fd, "add ecx, eax\n");
			fprintf(fd, "adc ebx, edx\n");
			fprintf(fd, "\n");
		}

		fprintf(fd, "mov eax, ecx\n");
		fprintf(fd, "mov ecx, ebx\n");
//		fprintf(fd, "pop ebx\n");
		fprintf(fd, "mov ebx, [ebp+16]\n");
		fprintf(fd, "mov [ebx+%d], eax\n", i*4);
		fprintf(fd, "\n");

//		c[i]=a;	a>>=16;
	}

	fprintf(fd, "mov [ebx+%d], ecx\n", i*4);

	fprintf(fd, 
		"pop edi\n"
		"pop esi\n"
		"pop ebx\n"
		"pop ebp\n"
		"ret\n"
		"\n\n"
	);
}

int main()
{
	FILE *fd;

	fd=fopen("math_int_gen.as", "wt");

	gen_mul(fd, 2);
	gen_mul(fd, 4);
	gen_mul(fd, 8);
	gen_mul(fd, 16);

	fclose(fd);
}

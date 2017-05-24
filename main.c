#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <fcntl.h>

#include <sys/mman.h>

#define SIZE 64



typedef union {

  float f;

  unsigned int i;

}foo;



int main(int argc, char** argv)

{

  int i, j;

  foo container[SIZE*(SIZE+1)];


  float flat[SIZE*(SIZE+1)];

  float input_a[SIZE*SIZE];

  float input_b[SIZE];

  float output_cpu[SIZE];

  float output_fpga[SIZE];

// initialization

  FILE *fd;

  fd = fopen("./input.txt", "r");



  unsigned int a;
i = 0;

  while ( !feof(fd) )

  {

    if (fscanf(fd, "%X", &a) != EOF)

    {
	container[i].i = a;
        
        if (i < SIZE)
	  input_b[i] = container[i].f;
	else
	  input_a[i-SIZE] = container[i].f;

	flat[i] = container[i].f;
	i++;

    }

  }

	fclose(fd);
  for(i=0; i< SIZE; i++)
    output_cpu[i] = 0.0f;

	for (j = 0; j < SIZE; j++) {

          for (i = 0; i < SIZE; i++)
	    output_cpu[j] += input_a[(SIZE-j)*SIZE-1-i] * input_b[SIZE-1-i];
	}

int foo;

  foo = open("/dev/mem", O_RDWR | O_NONBLOCK);

  float *fpga_bram = mmap(NULL, (SIZE*(SIZE+1)* sizeof(float)), PROT_WRITE, MAP_SHARED, foo, 0x40000000);

for (i = 0; i < (SIZE+1) * SIZE; i++)

  {

    *(fpga_bram +i ) = flat[i];

  }

  unsigned int *fpga_ip = mmap(NULL, sizeof(float),PROT_WRITE, MAP_SHARED, foo, 0x43c00000);

  *fpga_ip = 0x5555;

	while (*fpga_ip == 0x5555);



 printf("%-5s%-15s%-15s%-15s\n", "index", "CPU", "FPGA", "FPGA(hex)");
 // result
  for(i=0; i<SIZE; i++){
    output_fpga[i] = *(fpga_bram + i); 

    container[i].f = output_fpga[i];
    printf("%-10d%-15f%-15f%-15X\n", i, output_cpu[SIZE-1-i], output_fpga[i], container[i].i);
}
close(foo);



  return 0;

}

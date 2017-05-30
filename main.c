#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/time.h>
#define SIZE 64

struct timeval st[8];

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

///////////
// TIME 1//
///////////

gettimeofday(&st[0], NULL);
//computation (st0,st1)
	for (j = 0; j < SIZE; j++) {

          for (i = 0; i < SIZE; i++)
	    output_cpu[j] += input_a[(SIZE-j)*SIZE-1-i] * input_b[SIZE-1-i];
	}
gettimeofday(&st[1], NULL);

int foo;

  foo = open("/dev/mem", O_RDWR | O_NONBLOCK);

  float *fpga_bram = mmap(NULL, (SIZE*(SIZE+1)* sizeof(float)), PROT_WRITE, MAP_SHARED, foo, 0x40000000);
///////////
// TIME 2//
///////////
gettimeofday(&st[2], NULL);
//memory load (st2,3)
for (i = 0; i < (SIZE+1) * SIZE; i++)

  {

    *(fpga_bram +i ) = flat[i];

  }
gettimeofday(&st[3], NULL);
  unsigned int *fpga_ip = mmap(NULL, sizeof(float),PROT_WRITE, MAP_SHARED, foo, 0x43c00000);

  *fpga_ip = 0x5555;
///////////
// TIME 3//
///////////
gettimeofday(&st[4], NULL);
//wait (st4,5)
	while (*fpga_ip == 0x5555);
gettimeofday(&st[5], NULL);


 printf("%-5s%-15s%-15s%-15s\n", "index", "CPU", "FPGA", "FPGA(hex)");
///////////
// TIME 4//
///////////
gettimeofday(&st[6], NULL);
 // result ( st 6,7)
  for(i=0; i<SIZE; i++){
    output_fpga[i] = *(fpga_bram + i); 
}
gettimeofday(&st[7], NULL);

  for(i=0; i<SIZE; i++) {
    container[i].f = output_fpga[i];
    printf("%-10d%-15f%-15f%-15X\n", i, output_cpu[SIZE-1-i], output_fpga[i], container[i].i);
}

close(foo);

  for (i=0; i<4; i++) {
    float msec = st[2*i+1].tv_sec * 1000000 + st[2*i+1].tv_usec;
    msec -= st[2*i].tv_sec * 1000000 + st[2*i].tv_usec;
    printf("time %d'th: %f\n", i, msec);
  }

  return 0;

}

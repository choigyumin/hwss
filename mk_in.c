#include <stdio.h>
#include <stdlib.h>
#include <math.h>	
void main()
{
	int i,a,b;
	for(i=0; i<65*64 ; i++){
		a=pow(2,30);
		b=pow(2,26);
	
		printf("%02X\n", (a+ rand()%b ));
	}
	
	return;
}

#include <stdio.h>
#include "fft_socket_header.h"
#include "gtk_final.c"s

int numofsamples;

int main()
{
	fftheader* header;
	//read from port
	numofsamples = header->ptsPerFFT;
	start(numofsamples);
}

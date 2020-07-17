#include "EightQueens.h"

void main()
{
	while (TRUE)
	{
		// clear screen and create initial population
		init();
		
		// carry out the required number of generations
		completeGenerations();
		
		// display results
		displayResults();
		
		// see if the user wants to restart
		promptRestart();
	}
}

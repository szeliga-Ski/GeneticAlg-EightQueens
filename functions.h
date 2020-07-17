#ifndef	FUNCTIONS
#define FUNCTIONS

#include <input.h> 	// routines to read keyboard
#include <stdio.h>	// printf()
#include <stdlib.h>	// srand() ,rand()
#include <string.h>	// memcpy()
#include <time.h>	// time_t
#include "defines.h"

// used to seed the random number generator
time_t	t;

// e.g. 50 in a population, 8 queens
UCHAR 	population[POPULATION_SIZE * CHROMOSOME_LENGTH];
UCHAR 	matingArea[POPULATION_SIZE * CHROMOSOME_LENGTH];

// used to store the fitness of each population member (chromosome)
UCHAR 	fitness[POPULATION_SIZE];

// best solution found so far - i.e. number of queens that are safe
UCHAR	bestFitnessFound;

// best layout so far
UCHAR	bestLayoutFound[CHROMOSOME_LENGTH];

// flags to indicate whether each queen in the best layout is safe or not
BOOLEAN	bestLayoutQueensSafe[CHROMOSOME_LENGTH];

// in which generation was the best layout found?
UINT	bestLayout_Generation;

// total fitness of the population - used for mating
UINT	totalFitness;

// for printing results
UCHAR	printRow;

// variables used to display the "progress" indicator
UCHAR 	ucState; //= 0;
char 	cState[] = {'/', '-', '\\'};

// functions
void	calculateFitness(UINT nGeneration);
void	calculateTotalFitness();
void	chooseMate(UINT *pnMate);
void	completeGenerations();
void	copyMemory(UCHAR *dest, UCHAR *src, UINT numBytesToCopy);
void	displayBoard();
void	displayResults();
void	init();
void	promptRestart();
void	updateProgressIndicator(UINT nCurrGen);

#endif

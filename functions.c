#include "functions.h"

// calculates the fitness of each chromosome in the population and
// stores the results in fitness[]
void calculateFitness(UINT nGeneration)
{
	char  cCurrentQueenRow, cTargetQueenRow, i;
	UINT  nChessBoardCounter;
	UCHAR *pCurrentChessBoard;
	
	// flags for which queens are safe
	BOOLEAN queensSafe[8];
	
	// loop over each chess board
	for (nChessBoardCounter = 0; nChessBoardCounter < POPULATION_SIZE; ++nChessBoardCounter)
	{
		// point to the start of the current chess board
		pCurrentChessBoard = population + (nChessBoardCounter * CHROMOSOME_LENGTH);

		// set flags for safe queens on the current board		
		for (i=0; i<MAX_QUEENS; ++i) {
			queensSafe[i] = 1;
		}
		
		// loop over each queen and see if it can be taken. Don't need to compare the
		// final queen with itself though so "MAX_QUEENS - 1"
		for (cCurrentQueenRow=0; cCurrentQueenRow < MAX_QUEENS - 1; ++cCurrentQueenRow)
		{
			// compare the current queen with the others (the "targets")
			for (cTargetQueenRow=cCurrentQueenRow+1; cTargetQueenRow<MAX_QUEENS; ++cTargetQueenRow)
			{
				int rowDifference = cTargetQueenRow - cCurrentQueenRow;
				
				// check vertically, down & left, then down & right
				if (pCurrentChessBoard[cCurrentQueenRow] == pCurrentChessBoard[cTargetQueenRow]
					|| pCurrentChessBoard[cCurrentQueenRow] == pCurrentChessBoard[cTargetQueenRow] + rowDifference
					|| pCurrentChessBoard[cCurrentQueenRow] == pCurrentChessBoard[cTargetQueenRow] - rowDifference
				)
				{					
					queensSafe[cCurrentQueenRow] = 0;
					queensSafe[cTargetQueenRow] = 0;
				}
			}
		}
		
		// store the fitness for the current board
		fitness[nChessBoardCounter] = queensSafe[0]+queensSafe[1]+queensSafe[2]+queensSafe[3]+queensSafe[4]+queensSafe[5]+queensSafe[6]+queensSafe[7];
		
		// see if we have found a new best solution
		if ( bestFitnessFound < fitness[nChessBoardCounter] )
		{
			// store current generation number
			bestLayout_Generation = nGeneration;
			
			// store value for best fitness found
			bestFitnessFound = fitness[nChessBoardCounter];
			
			//#ifdef DEBUG_CALCULATE_FITNESS
			if (nGeneration) // don't do this for the initial population
			{
				printf("%c%c%c", PRINTF_SET_CURSOR,++printRow, PRINTF_FIRST_COLUMN);
				printf("%c%c",   PRINTF_SET_INK,PRINTF_YELLOW_INK);
				printf("*** New best fitness: %d queens safe (generation %d)", bestFitnessFound, bestLayout_Generation);
			}
			//#endif

			// and store a copy of the best layout so far - memcpy(dest, src, length)
			memcpy(bestLayoutFound, pCurrentChessBoard, CHROMOSOME_LENGTH);
			
			// store flags for which queens are safe in the current best layout
			memcpy(bestLayoutQueensSafe, queensSafe, CHROMOSOME_LENGTH);
		}
		
		#ifdef DEBUG_CALCULATE_FITNESS
		printf("\tBoard #%d = %d; %d%d%d%d%d%d%d%d\n", nChessBoardCounter, fitness[nChessBoardCounter], queensSafe[0],queensSafe[1],queensSafe[2],queensSafe[3],queensSafe[4],queensSafe[5],queensSafe[6],queensSafe[7]);
		#endif
	}
}

// adds up all fitness values - used to choose mates
void calculateTotalFitness()
{
	UINT i;
	
	totalFitness = 0;
	
	for (i=0; i<POPULATION_SIZE; ++i)
	{
		totalFitness += fitness[i];
	}
}

int __CALLEE__ carpetRollCLS(void) {

	#asm
	
	push af
	push bc
	push de
	push hl
	
	defc ATTRP 	= $5c8d
	defc BORDCR = $5c84
	
	START:
		LD L, 0			; HL= Top-left corner of
	NEXTCOL:	
		LD H, $40		; the screen
;		LD B, 3			; do a pause
	;PAUSE:
		;HALT
		;DJNZ PAUSE
	NXTBYTE:
		LD C, (HL)		; blank out the current
		LD (HL), 0		; column, ...
		LD B, 8
	NXTROT:
		RR C			; ... produce its mirror
		RLA				; image
		DJNZ NXTROT
		INC HL			; and OR it with the
		OR (HL)			; next column to the right
		LD (HL), A
		LD DE, $001f	; Move onto the next
		ADD HL, DE		; row of the screen
		LD A, H			; Are we at the bottom?
		CP $58
		JR NZ, NXTBYTE	; If not then repeat the
		INC DE			; process
		LD B, $16		; Now take the ink
		LD A, (ATTRP)	; and paper etc., and
	NXTOP:
		LD (HL), A		; fill out the current
		ADD HL, DE		; columns attributes
		DJNZ NXTOP
		LD A, (BORDCR)	; Use the border colour
		LD (HL), A		; for the bottom two
		ADD HL, DE		; lines
		LD (HL), A
		ADD HL, DE
		INC L			; Have we reached the last
		LD A, L			; column of the screen? If
		CP $1f			; not then roll up the
		JR NZ, NEXTCOL	; next column
		LD H, $40		; Blank out the last
		LD B, $00c0		; column, ...
	NXTROW:
		LD (HL), D
		ADD HL,DE
		DJNZ NXTROW
		LD A,(ATTRP)	; Fill in its attributes
		LD B, $16
	NXTOP2:
		LD (HL), A
		ADD HL,DE
		DJNZ NXTOP2
		LD A, (BORDCR)
		LD (HL), A
		ADD HL,DE
		LD (HL), A
		
		pop hl
		pop de
		pop bc
		pop af
		
		RET	
	#endasm
}

// chooses a mate from the current population and sets the value
// of the passed parameter as an index into the current population
void chooseMate(unsigned int *pnMate)
{
	// generate a random number between zero and the total fitness of the population
	UINT i, nFit=0, randomNumber = rand() % totalFitness; 
	
	// look up the parent that this corresponds to - fitter parents have a greater chance of being chosen

	*pnMate = 0; // default value

	for ( i=0; i<POPULATION_SIZE; ++i )
	{
		nFit += fitness[i];

		if ( nFit >= randomNumber )
		{
			#ifdef DEBUG_CHOOSE_MATES
			printf("nFit=%d, randomNumber=%d, totalFitness=%d\n", nFit, randomNumber, totalFitness);
			printf("\tSetting mate %d", i);
			#endif
			
			*pnMate = i;
			return;
		}
	}
	
	return;
}

void completeGenerations()
{
	UINT i, j, k, mate1, mate2;
	UCHAR *src, dest; // used to copy parts of mating and population arrays with memcpy()
	
	i = 0;
	
	// loop for the required number of generations	
	for (i=0; i<MAX_GENERATIONS; ++i)		
	{
		updateProgressIndicator(i);
		
		// rate the fitness of each chromosome
		calculateFitness(i);

		// find the total fitness of the current population - needed for roulette wheel mating
		calculateTotalFitness();

		// return to main() if all queens are safe
		if (bestFitnessFound == MAX_QUEENS)
		{
			bestLayout_Generation = i;
			return;
		}

		if (!i)
		{
			printf("%c%c",PRINTF_SET_INK,PRINTF_WHITE_INK);
			printf("%c%c%c",PRINTF_SET_CURSOR,PRINTF_FIRST_ROW, PRINTF_FIRST_COLUMN);			
			printf("Initial best fitness is %d queens safe", bestFitnessFound);
			++printRow;
			printf("%c%c",		PRINTF_SET_INK,PRINTF_YELLOW_INK);
		}

		#ifdef DEBUG_COMPLETE_GENERATIONS
			printf("%c%c%c", PRINTF_SET_CURSOR,32,32);	
			printf("Generation %d\n", i);
		#endif
		
		// display initial board layout
		displayBoard();

		//printf("%c%c%c", PRINTF_SET_CURSOR,34,32);
		//printf("Generation %d: Improvement - %d queens safe so far...\r", i++, bestFitnessFound);
		
		for (k=0; k<POPULATION_SIZE/2; ++k)
		{
			updateProgressIndicator(i);

			// choose pairs
			chooseMate(&mate1);
			chooseMate(&mate2);
			
			#ifdef DEBUG_COMPLETE_GENERATIONS
			printf("\n\tMating chromosomes %d and %d\n", mate1, mate2);
			#endif
			
			// mate the pairs into a temporary array			
			for (j=0; j<HALF_CHROMOSOME_LENGTH; ++j)
			{
				updateProgressIndicator(i);
				
				// Keep the first half of each chromosome the same
				matingArea[ mate1 * CHROMOSOME_LENGTH  + j]   							= population[mate1  * CHROMOSOME_LENGTH  + j];
				matingArea[ mate2 * CHROMOSOME_LENGTH  + j]   							= population[mate2  * CHROMOSOME_LENGTH  + j];

				// Swap the second halves of the two mates
				matingArea[(mate1 * CHROMOSOME_LENGTH) + HALF_CHROMOSOME_LENGTH + j]   	= population[(mate2 * CHROMOSOME_LENGTH) + HALF_CHROMOSOME_LENGTH + j];
				matingArea[(mate2 * CHROMOSOME_LENGTH) + HALF_CHROMOSOME_LENGTH + j]   	= population[(mate1 * CHROMOSOME_LENGTH) + HALF_CHROMOSOME_LENGTH + j];

				// potentially mutate the new chromosomes
				if (rand() % 100 < MUTATION_PROBABILITY)
				{
					matingArea[mate1*CHROMOSOME_LENGTH + (rand()%CHROMOSOME_LENGTH)] = rand() % BOARD_WIDTH;
				}

				if (rand() % 100 < MUTATION_PROBABILITY)
				{
					matingArea[mate2*CHROMOSOME_LENGTH + (rand()%CHROMOSOME_LENGTH)] = rand() % BOARD_WIDTH;
				}
			}
			
			#ifdef DEBUG_COMPLETE_GENERATIONS
			printf("\nPop chr %d%d%d%d%d%d%d%d; New chr=%d%d%d%d%d%d%d%d",population[mate1*CHROMOSOME_LENGTH],population[mate1*CHROMOSOME_LENGTH+1],population[mate1*CHROMOSOME_LENGTH+2],population[mate1*CHROMOSOME_LENGTH+3],population[mate1*CHROMOSOME_LENGTH+4],population[mate1*CHROMOSOME_LENGTH+5],population[mate1*CHROMOSOME_LENGTH+6],population[mate1*CHROMOSOME_LENGTH+7],matingArea[mate1*CHROMOSOME_LENGTH],matingArea[mate1*CHROMOSOME_LENGTH+1],matingArea[mate1*CHROMOSOME_LENGTH+2],matingArea[mate1*CHROMOSOME_LENGTH+3],matingArea[mate1*CHROMOSOME_LENGTH+4],matingArea[mate1*CHROMOSOME_LENGTH+5],matingArea[mate1*CHROMOSOME_LENGTH+6],matingArea[mate1*CHROMOSOME_LENGTH+7]);
			printf("\n        %d%d%d%d%d%d%d%d and      %d%d%d%d%d%d%d%d",population[mate2*CHROMOSOME_LENGTH],population[mate2*CHROMOSOME_LENGTH+1],population[mate2*CHROMOSOME_LENGTH+2],population[mate2*CHROMOSOME_LENGTH+3],population[mate2*CHROMOSOME_LENGTH+4],population[mate2*CHROMOSOME_LENGTH+5],population[mate2*CHROMOSOME_LENGTH+6],population[mate2*CHROMOSOME_LENGTH+7],matingArea[mate2*CHROMOSOME_LENGTH],matingArea[mate2*CHROMOSOME_LENGTH+1],matingArea[mate2*CHROMOSOME_LENGTH+2],matingArea[mate2*CHROMOSOME_LENGTH+3],matingArea[mate2*CHROMOSOME_LENGTH+4],matingArea[mate2*CHROMOSOME_LENGTH+5],matingArea[mate2*CHROMOSOME_LENGTH+6],matingArea[mate2*CHROMOSOME_LENGTH+7]);
			#endif
		}	
		
		// copy the temporary array to the main population
		memcpy(population, matingArea, POPULATION_SIZE * CHROMOSOME_LENGTH);
		
		// display the current best layout
		displayBoard();
	}
	
	// display the current best layout
	displayBoard();
	
	#ifdef DEBUG_COMPLETE_GENERATIONS
	printf("\n\nBest fitness found = %d in generation %d", bestFitnessFound, i);
	#endif
}

void copyMemory(unsigned char *dest, unsigned char *src, UINT numBytesToCopy)
{
	UINT i;
	
	for (i=0; i<numBytesToCopy; ++i)
	{
		dest[i] = src[i];
	}
	
	return;
}

void displayBoard()
{
	UCHAR i, j;
	
	//carpetRollCLS();
	
	/* Clear screen in non-ANSI mode*/
	// printf ("%c",12);
	// printf("%c%c%c",PRINTF_SET_CURSOR,32, 35);
	// printf("%c%c",PRINTF_SET_INK,PRINTF_WHITE_INK);
	// printf("Genetic Algorithm for Eight Queens\n");
	// printf("%c%c%c",PRINTF_SET_CURSOR,33, 37);
	// printf("Mark Szeliga Szolkowski - v0.1\n");
	// printf("%c%c",PRINTF_SET_INK,PRINTF_YELLOW_INK);
	
	//printf("%c%c%c",PRINTF_SET_CURSOR,39, 35);	
	printf("%c%c",	PRINTF_SET_INK, PRINTF_CYAN_INK);
	printf("%c%c%c",PRINTF_SET_CURSOR, PRINTF_CURRENT_BEST_BOARD_ROW, PRINTF_FIRST_COLUMN);
	printf("  Current best layout");
	printf("%c%c",	PRINTF_SET_INK, PRINTF_WHITE_INK);
	
	// display each queen on the board
	for (i=0; i<BOARD_HEIGHT; ++i)
	{
		printf("\n");

		for (j=0; j<BOARD_WIDTH; ++j)
		{
			//printf(" %c ", bestLayoutFound[i] == j ? 'Q' : '.');
			
			if (bestLayoutFound[i] == j && bestLayoutQueensSafe[i] == TRUE)
			{
				// set print colour to green - QUEEN SAFE
				printf("%c%c", PRINTF_SET_INK, PRINTF_GREEN_INK);
				printf(" Q ");
			}
			else if (bestLayoutFound[i] == j && bestLayoutQueensSafe[i] == FALSE)
			{
				// set print colour to red - QUEEN CAN BE TAKEN
				printf("%c%c", PRINTF_SET_INK, PRINTF_RED_INK);
				printf(" Q ");
			}
			else
			{
				// set print colour to white and print a '.' for an empty square
				printf("%c%c", PRINTF_SET_INK, PRINTF_WHITE_INK);
				printf(" . ");
			}
		}
	}
}

void displayResults()
{
	BYTE i;

	printf("%c%c%c", PRINTF_SET_CURSOR, printRow, PRINTF_FIRST_COLUMN);
	printf("%c%c",	 PRINTF_SET_INK,    PRINTF_CYAN_INK);
	printf("\n\nGeneration %d: %d queens safe.", bestLayout_Generation, bestFitnessFound);	
	displayBoard();
}

void init()
{
	int i,j;
	
	printRow = PRINTF_FIRST_ROW;
	printf("%c%c%c", 	PRINTF_SET_CURSOR,printRow,PRINTF_FIRST_COLUMN);
	printf("%c%c",		PRINTF_SET_INK,PRINTF_CYAN_INK);
	
	srand((unsigned) time(&t));		// Seed the random number generator

	bpoke(23693,PAPER_BLACK | INK_YELLOW);
	zx_border(INK_BLACK);
	bpoke(0x5c48,0);	// Set 8*border colour in sys var BORDCR (23624 / 0x5c48) - to flash border
	
	printf("%c%c", PRINTF_SET_PAPER,PRINTF_BLACK_PAPER);
	printf("%c%c", PRINTF_SET_INK,	PRINTF_YELLOW_INK);
	carpetRollCLS();
	
	// set up the "progress" indicator
	ucState = 0;
	
	// reset best fitness found so far	
	bestFitnessFound = 0;
	
	// fill all the game boards by storing a random
	// position in the current row for the next queen
	for (i=0; i<POPULATION_SIZE * CHROMOSOME_LENGTH; ++i) {
		population[i] = rand() % BOARD_WIDTH;
		
		#ifdef DEBUG_INIT
		// display board
		printf("i=%d, pos=%d  -  ", i, population[i]);
		
		if (i && (i+1)%8 == 0)	printf("\n\n");
		#endif
	}
}

void promptRestart()
{
	printf("%c%c",PRINTF_SET_INK,PRINTF_GREEN_INK);
	printf("\n\nPress a key to run again...");	
	while (!in_Inkey()) {}
}

void updateProgressIndicator(UINT ncurrGen)
{
	printf("%c%c%c", 	PRINTF_SET_CURSOR, PRINTF_FIRST_ROW, PRINTF_LAST_COLUMN);
	printf("%c%c",		PRINTF_SET_INK,	   PRINTF_RED_INK);
	printf("%c%c%c",	PRINTF_SET_CURSOR, PRINTF_FIRST_ROW, PRINTF_LAST_COLUMN);
	printf("%c",		cState[ucState]);
	printf("%c%c%c", 	PRINTF_SET_CURSOR, PRINTF_FIRST_ROW, PRINTF_GEN_UPDATE_COLUMN);
	printf("%c%c",		PRINTF_SET_INK,	   PRINTF_GREEN_INK);
	printf("Gen. %d/%d", ncurrGen+1, MAX_GENERATIONS);
	
	ucState == 2 ? ucState = 0 : ++ucState;
}

#ifndef DEFINES_H
#define DEFINES_H

//#define DEBUG
//#define DEBUG_CALCULATE_FITNESS
//#define DEBUG_COMPLETE_GENERATIONS
//#define DEBUG_INIT
//#define DEBUG_CHOOSE_MATES

#define FRAMES_SYSTEM_VARIABLE				23672

#define BOOLEAN								char
#define BYTE								char
#define UCHAR								unsigned char
#define UINT								unsigned int

#define FALSE								0
#define TRUE								1

#define BOARD_WIDTH							8
#define BOARD_HEIGHT						8
#define CHROMOSOME_LENGTH					8
#define HALF_CHROMOSOME_LENGTH				4
#define MAX_QUEENS							8
#define MUTATION_PROBABILITY				5
#define MAX_GENERATIONS						100

// Make sure the population size is an even number
// or you won't be able to pair up all chromosomes!
#define POPULATION_SIZE						50

#define QUEEN_PRESENT						1

// Used with printf()

#define PRINTF_FIRST_ROW					32
#define	PRINTF_FIRST_COLUMN					32
#define PRINTF_LAST_COLUMN					92
#define PRINTF_GEN_UPDATE_COLUMN			75
#define PRINTF_CURRENT_BEST_BOARD_ROW		45

#define PRINTF_SET_CURSOR					22
#define PRINTF_SET_INK						16
#define PRINTF_SET_PAPER					17

#define PRINTF_RED_INK						18
#define PRINTF_GREEN_INK					36
#define PRINTF_CYAN_INK						45
#define PRINTF_YELLOW_INK					54
#define PRINTF_WHITE_INK					63

#define PRINTF_BLACK_PAPER					0
#define PRINTF_BLUE_PAPER					9
#define PRINTF_RED_PAPER					18

#endif

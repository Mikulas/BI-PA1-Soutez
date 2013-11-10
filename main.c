#include <stdio.h>
#include <stdlib.h>
#ifndef __PROGTEST__
	#include <time.h>
#endif
#define MAX_WIDTH 32
#define MAX_HEIGHT 32
#define MAX_BOXES 200
#define CERTAIN 200 // index of matrix with certain numbers in solution tensor 

void inputError(int d)
{
	// printf("input error #%d\n", d);

	// read until EOF
	while (getchar() != EOF) {}

	printf("Nespravny vstup.\n");
	exit(1);
}

void loadPuzzle(int board[][MAX_WIDTH], int *width, int *height, int *sum)
{
	*width = 0;
	*height = 0;
	*sum = 0;

	//
	// Load puzzle
	//
	char c;
	char state = 0; // 0: expecting +, 1: expecting - or EOL, 2: expecting -
	do {
		c = getchar();
		if (state == 0 && c != '+') inputError(1);
		if (state == 1 && c != '-' && c != '\n') inputError(2);
		if (state == 2 && c != '-') inputError(3);
		state = (state + 1) % 3;

		if (state == 0) {
			(*width)++;
		}
	} while (c != '\n');
	if (*width == 0 || *width > MAX_WIDTH) inputError(4);

	int column;
	int numbers = 0;
	int firstDigit;
	// read puzzle content
	while (1)
	{
		state = 3;
		column = 0;
		firstDigit = 0;

		// read numbers line
		// 0: expecting space or "|\n", 1: expecting space or digit, 2: expecting space or digit, 3: expecting |
		while (1)
		{
			c = getchar();
			if (state == 3 && c != '|') inputError(5);
			if (state == 0 && c != ' ' && c != '|') inputError(6);
			if (state == 1 && c != ' ' && (c < 49 || c > 57)) inputError(7); // not space or [1-9]
			if (state == 2 && c != ' '
				&& (
					(firstDigit != 0 && (c < 48 || c > 57)) // any digit
				 || (firstDigit == 0 && (c < 49 || c > 57)) // only [1-9] if no number before
				)) inputError(7); // not space or [1-9]

			if (state == 1 && c != ' ')
			{
				firstDigit = c - 48; // ascii number to int
			}
			if (state == 2 && c != ' ')
			{
				int number = firstDigit * 10 + c - 48;
				board[*height][column] = number;
				*sum += number;
				numbers++;
				firstDigit = 0;
			}

			if (state != 0 && c == '\n') inputError(8);
			if (state != 3 && state != 0 && c == '|') inputError(9);
			if (state == 0 && c == '\n' && column != *width) inputError(10);
			if (state == 0 && c == '|') {
				if (getchar() != '\n') inputError(11);
				(*height)++;
				break;
			}

			if (state == 2) {
				column++;
			}
			state = (state + 1) % 3;
		}

		state = 0;
		column = 0;
		int closingLine = -1; // unset, 0 no, 1 yes
		// read border line
		// 0: expecting +, 1: expecting space or EOL (or -), 2: expecting space
		while (1)
		{
			c = getchar();
			if (state == 0 && c != '+') inputError(12);
			if (state == 1 && c != '\n'
				&& (
					(closingLine == 0 && c != ' ')
				 || (closingLine == 1 && c != '-')
				 || (closingLine == -1 && c != '-' && c != ' ')
				 )) inputError(13);
			if (state == 2
				&& (
					(closingLine == 0 && c != ' ')
				 || (closingLine == 1 && c != '-')
				)) inputError(14);

			if (state == 1 && closingLine == -1)
			{
				closingLine = c == '-';
			}
			if (state == 1 && c == '\n')
			{
				if (column != *width) inputError(15);
				break;
			}

			state = (state + 1) % 3;
			if (state == 2) {
				column++;
			}
		}
		if (closingLine)
			break;
	}
	if (*height > MAX_HEIGHT || numbers > MAX_BOXES) inputError(16);
}

/**
 * @return int index of last filled box
 */
int fillCertainBoxes(int board[][MAX_WIDTH], int solution[][MAX_WIDTH][MAX_BOXES + 1], int *width, int *height)
{
	
}

int main()
{
	#ifndef __PROGTEST__
		clock_t begin, end;
		double time_spent;
		begin = clock();
	#endif

	int board[MAX_HEIGHT][MAX_WIDTH];
	int (*p_board)[MAX_WIDTH] = board;
	int solution[MAX_HEIGHT][MAX_WIDTH][MAX_BOXES + 1];

	printf("Zadejte puzzle:\n");
	int width, height, sum;
	loadPuzzle(p_board, &width, &height, &sum);

	//
	// Solve puzzle
	//
	/*
	for (int row = 0; row < height; ++row)
	{
		for (int col = 0; col < width; ++col)
		{
			printf("%d ", board[row][col]);
		}
		printf("\n");
	}
	printf("width=%d height=%d\n", width, height);
	//*/

	if (sum != width * height) // optimization
	{
		printf("Reseni neexistuje.\n");
		// TODO else ale to jeste neznamena, ze existuje
	}

	int blanks = width * height;
	for (int row = 0; row < height; ++row)
	{
		for (int col = 0; col < width; ++col)
		{
			// set unique id for this box
			if (board[row][width] != 0)
			{
				solution[row][width][CERTAIN] = row * MAX_WIDTH + width;
				blanks--;
			}
		}
	}

	if (blanks == 0)
	{
		// done
	}

	int newBoxAssigned = 0;
	// Make deterministic partial solutions.
	// PERFORMANCE: Worst case first number is changed and then the
	// puzzle is reiterated all over again => break after change and
	// loop only once, or continue and break on the changed index.
	do
	{

	} while (newBoxAssigned);

	// FIND all certain numbers, example:
	// 5#  resolves to  5#
	// 2#               22
	//
	// or
	// ##3# always must evaluate to #33#
	// save those to CERTAIN
	// if there is already a CERTAIN number on the location,
	// puzzle has no solution

	// FIND possible numbers
	// 5#3 means # is either 5 or 3 only
	// save this to first empty location
	// when after iteration over all boxes this vector
	// only contains one number, it should be moved to certain

	// loop while no new numbers are added
	// then, there are multiple solutions:
	// pick one, continue, then return and try other paths

	// prime numbers are always in line
	// is factorization any good?
	// many numbers only have one solution

	#ifndef __PROGTEST__
		end = clock();
		time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
		printf("~~~%f~~~\n", time_spent);
	#endif
}

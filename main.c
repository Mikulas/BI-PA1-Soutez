#include <stdio.h>
#include <stdlib.h>
#ifndef __PROGTEST__
	#include <time.h>
#endif
#define MAX_WIDTH 32
#define MAX_HEIGHT 32

void inputError(int d)
{
	// printf("input error #%d\n", d);

	// read until EOF
	while (getchar() != EOF) {}

	printf("Nespravny vstup.\n");
	exit(1);
}

int main()
{
	#ifndef __PROGTEST__
		clock_t begin, end;
		double time_spent;
		begin = clock();
	#endif

	printf("Zadejte puzzle:\n");

	int board[MAX_HEIGHT][MAX_WIDTH] = {{0}};

	//
	// Load puzzle
	//
	char c;
	char state = 0; // 0: expecting +, 1: expecting - or EOL, 2: expecting -
	int width = 0;
	do {
		c = getchar();
		if (state == 0 && c != '+') inputError(1);
		if (state == 1 && c != '-' && c != '\n') inputError(2);
		if (state == 2 && c != '-') inputError(3);
		state = (state + 1) % 3;

		if (state == 0) {
			width++;
		}
	} while (c != '\n');
	if (width == 0 || width > MAX_WIDTH) inputError(4);

	int column;
	int height = 0;
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
				board[height][column] = firstDigit * 10 + c - 48;
			}

			if (state != 0 && c == '\n') inputError(8);
			if (state != 3 && state != 0 && c == '|') inputError(9);
			if (state == 0 && c == '\n' && column != width) inputError(10);
			if (state == 0 && c == '|') {
				if (getchar() != '\n') inputError(11);
				height++;
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
				if (column != width) inputError(15);
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
	if (height > MAX_HEIGHT) inputError(16);

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


	#ifndef __PROGTEST__
		end = clock();
		time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
		printf("~~~%f~~~\n", time_spent);
	#endif
}

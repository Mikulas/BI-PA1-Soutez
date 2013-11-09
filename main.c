#include <stdio.h>
#include <stdlib.h>
#ifndef __PROGTEST__
	#include <time.h>
#endif

void inputError()
{
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

	//
	// Load puzzle
	//
	char c;
	char state = 0; // 0: expecting +, 1: expecting - or EOL, 2: expecting -
	int width = 0;
	do {
		c = getchar();
		if (state == 0 && c != '+') inputError();
		if (state == 1 && c != '-' && c != '\n') inputError();
		if (state == 2 && c != '-') inputError();
		state = (state + 1) % 3;

		if (state == 0) {
			width++;
		}
	} while (c != '\n');
	if (width == 0) inputError();

	int column;
	// read puzzle content
	while (1)
	{
		state = 3;
		column = 0;

		// read numbers line
		// 0: expecting space or "|\n", 1: expecting space or digit, 2: expecting space or digit, 3: expecting |
		while (1)
		{
			c = getchar();
			if (state == 3 && c != '|') inputError();
			if (state == 0 && c != ' ' && c != '|') inputError();
			if ((state == 1 || state == 2) && c != ' ' && (c < 49 || c > 57)) inputError(); // not space or [1-9]
			
			if (state != 0 && c == '\n') inputError();
			if (state != 3 && state != 0 && c == '|') inputError();
			if (state == 0 && c == '\n' && column != width) inputError();
			if (state == 0 && c == '|') {
				if (getchar() != '\n') inputError();
				break;
			}

			state = (state + 1) % 3;
			if (state == 1) {
				column++;
			}
		}

		state = 0;
		column = 0;
		int closingLine = -1; // unset, 0 no, 1 yes
		// read border line
		// 0: expecting +, 1: expecting space or EOL (or -), 2: expecting space
		while (1)
		{
			c = getchar();
			if (state == 0 && c != '+') inputError();
			if (state == 1 && c != '\n'
				&& (
					(closingLine == 0 && c != ' ')
				 || (closingLine == 1 && c != '-')
				 || (closingLine == -1 && c != '-' && c != ' ')
				 )) inputError();
			if (state == 2
				&& (
					(closingLine == 0 && c != ' ')
				 || (closingLine == 1 && c != '-')
				)) inputError();

			if (state == 1 && closingLine == -1)
			{
				closingLine = c == '-';
			}
			if (state == 1 && c == '\n')
			{
				if (column != width) inputError();
				break;
			}

			state = (state + 1) % 3;
			if (state == 0) {
				column++;
			}
		}
		if (closingLine)
			break;
	}

	//
	// Solve puzzle
	//
	puts("todo\n");


	#ifndef __PROGTEST__
		end = clock();
		time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
		printf("~~~%f~~~\n", time_spent);
	#endif
}

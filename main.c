#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#ifndef __PROGTEST__
    #include <time.h>
#endif
#define MAX_WIDTH 32
#define MAX_HEIGHT 32
#define MAX_BOXES 200
#define CERTAIN 200 // index of matrix with certain numbers in solution tensor
#define ORIGINAL 201 // --//--
#define MATRIX_SIZE (MAX_BOXES + 2) // for CERTAIN and ORIGINAL
#define NOT_SET -1 // solutions[][][CERTAIN]
#define NOT_FOUND -1 // fillCertainBoxes return value
#define NO_SOLUTION -2 // fillCertainBoxes return value
#define SOLUTION_FOUND -3 // fillCertainBoxes return value
#define NO_MORE_BRANCHES -4 // fillCertainBoxes return value

// TODO refactor loadPuzzle, then polish and resolve todos

void printSolution(int solution[][MAX_WIDTH][MATRIX_SIZE], char sizes[],
    const int width, const int height, int originalOnly, int printId);

void inputError(int d)
{
    d = 0; // ignore warning
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
            if (state == 2 && firstDigit != 0 && c == ' ') inputError(7); // first is digit, but then nothing

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


// TODO refactor
int counts[MAX_HEIGHT * MAX_WIDTH];

/**
 * Should converge faster with sizes sorted by size desc
 * @return int index of last filled box or NOT_FOUND
 */
int fillCertainBoxes(int solution[][MAX_WIDTH][MATRIX_SIZE], char sizes[],
    const int width, const int height, int usePossible)
{
    // The following algorithm only works if each rectangle contains exactly one id in CERTAIN:
    // For all numbers in CERTAIN, draw all rectangles that number can fill (without backtracking)
    // then, if there are only n filled rectangles for number n, move it to certain and reiterate.
    // This might not lead to solution, but should be easy to implement.
    //
    // Lines are trivial. How about the rest? Factorize and iterate over all rectangles
    //
    // Or better yet, check if the whole rectangle is empty first (against CERTAIN). Than it should
    // converge to solution (or to a crossroad).

    // If there is more than one box for given rectangle id, valid rectangles should originate from
    // either of them and contain the other one.

    // Also, if there is only one possible way from start to next, it is CERTAIN.
    // 3###  even though @ may belong to either of the 2s, it must belong to the leftmost one
    // 2@2#

    // PERFORMANCE idea: start with greatest numbers? That should be very good
    // ie sort ids by size DESC

    // this can also generate certain partial rectangles: if a square is a possible value for all valid
    // rectangles, it is a certain value.

    // So: for each number in certain that is NOT YET closed, find all divisors and their codivisors
    // and write rectangles as possible values to the solution tensor.

    /**
     * For each id on field, find all properly sized rectangles and test they fit on board
     */

    int done = 1;
    int validRectangles[MAX_HEIGHT * MAX_WIDTH];
    for (int row = 0; row < height; ++row)
    {
        for (int col = 0; col < width; ++col)
        {
            if (solution[row][col][ORIGINAL] != 1)
            {
                continue;
            }

            const int id = row * MAX_WIDTH + col;

            // if this rectangle is complete
            if (counts[id] == sizes[id])
                continue;
            const int size = sizes[id];
            done = 0;

            int box_width = 1;
            int box_height;
            validRectangles[id] = 0;
            while (box_width <= size)
            {
                if (size % box_width == 0) {
                    box_height = size / box_width;

                    /**
                     * Tests if this rectangle fits, starting from any point containg
                     * current [row;col]. Moves it all the way up and tests all moves down
                     */
                    for (int y = row + (-box_height + 1); y <= row; ++y)
                    {
                        if (y < 0 || y + box_height > height) continue; // outside of board
                        // move it all the way to the left and test all moves to right
                        for (int x = col + (-box_width + 1); x <= col; ++x)
                        {
                            if (x < 0 || x + box_width > width) continue; // outside of board

                            // test if rectangle contains all ids already on board
                            int sameIdsCoveredByThisRectangle = 0;

                            // test if [x,y] to [x+width, y+height] is empty
                            for (int test_y = y; test_y < y + box_height; ++test_y)
                            {
                                for (int test_x = x; test_x < x + box_width; ++test_x)
                                {
                                    if (solution[test_y][test_x][CERTAIN] != NOT_SET
                                        && solution[test_y][test_x][CERTAIN] != id)
                                        goto next;
                                    if (solution[test_y][test_x][CERTAIN] == id)
                                        sameIdsCoveredByThisRectangle++;
                                }
                            }
                            if (sameIdsCoveredByThisRectangle != counts[id])
                            {
                                // Rectangle does not contain all same ids already on board.
                                goto next;
                            }

                            // Box fits and contains all certain boxes it should.
                            validRectangles[id]++;
                            for (int test_y = y; test_y < y + box_height; ++test_y)
                            {
                                for (int test_x = x; test_x < x + box_width; ++test_x)
                                {
                                    if (solution[test_y][test_x][CERTAIN] == id)
                                        continue;

                                    for (int index = 0; /* must break */; ++index)
                                    {
                                        if (solution[test_y][test_x][index] == NOT_SET)
                                        {
                                            solution[test_y][test_x][index] = id;
                                            break;
                                        }
                                    }
                                }
                            }

                        next:
                            continue;
                        }
                    }
                }
                box_width++;
            }
            if (validRectangles == 0)
            {
                return NO_SOLUTION;
            }
        }
    }

    if (done)
    {
        return SOLUTION_FOUND;
    }

    // TODO if validRectangles == 1, its CERTAIN
    // is this covered by count check below?

    /**
     * For all boxes look if there is only one way to fill a single
     * box. If so, set it to certain and return to runAgain.
     */
    int changeFound = 0;
    int allCertain = 1;
runAgain:
    for (int row = 0; row < height; ++row)
    {
        for (int col = 0; col < width; ++col)
        {
            if (solution[row][col][CERTAIN] != NOT_SET)
            {
                continue;
            }

            allCertain = 0;
            int lastId = NOT_SET;
            int unique = 1;
            int count = 0;
            int differentFromLast;
            for (int index = 0; index < MAX_BOXES; ++index)
            {
                differentFromLast = 0;
                if (solution[row][col][index] == NOT_SET)
                {
                    break;
                }

                if (lastId != NOT_SET && solution[row][col][index] != lastId)
                {
                    unique = 0;
                }

                if (solution[row][col][index] == lastId)
                {
                    count++;
                }
                else
                {
                    lastId = solution[row][col][index];
                    count = 1;
                    differentFromLast = 1;
                }

                if (lastId != NOT_SET && count == validRectangles[lastId])
                {
                    changeFound++;
                    solution[row][col][CERTAIN] = lastId;
                    counts[lastId] += 1;
                    if (counts[lastId] > sizes[lastId])
                    {
                        return NO_SOLUTION;
                    }
                    goto nextBox;
                }
                if (usePossible != NOT_SET)
                {
                    if (differentFromLast && usePossible == 0)
                    {
                        solution[row][col][CERTAIN] = lastId;
                        changeFound++;
                        usePossible = NOT_SET;
                        counts[lastId] += 1;
                        if (counts[lastId] > sizes[lastId])
                        {
                            return NO_SOLUTION;
                        }
                        goto runAgain;
                    }
                    else if (differentFromLast)
                    {
                        usePossible--;
                    }
                }
            }
            if (!unique && usePossible != NOT_SET)
            {
                return NO_MORE_BRANCHES;
            }
            if (unique)
            {
                changeFound++;
                if (solution[row][col][CERTAIN] != NOT_SET
                    && solution[row][col][CERTAIN] != lastId)
                    // PERFORMANCE: Not quite sure why the other one happens, should be eliminated
                {
                    return NO_SOLUTION;
                }
                solution[row][col][CERTAIN] = lastId;
                counts[lastId] += 1;

                if (counts[lastId] > sizes[lastId])
                {
                    return NO_SOLUTION;
                }
            }

        nextBox:
            // clear possible values
            for (int i = 0; i < MAX_BOXES; ++i)
            {
                solution[row][col][i] = NOT_SET;
            }
        }
    }

    return allCertain ? SOLUTION_FOUND : (changeFound ? 1 : NOT_FOUND);
}

void printSolution(int solution[][MAX_WIDTH][MATRIX_SIZE], char sizes[],
    const int width, const int height, int originalOnly, int printId)
{
    // print first border line
    printf("+");
    for (int col = 0; col < width; ++col)
    {
        printf("--+");
    }
    printf("\n");

    for (int row = 0; row < height; ++row)
    {
        // print numbers
        printf("|");
        for (int col = 0; col < width; ++col)
        {
            const int id = solution[row][col][CERTAIN];
            const int complete = counts[id] == sizes[id];

            const int rightId = col + 1 < MAX_WIDTH ? solution[row][col + 1][CERTAIN] : NOT_SET;
            const int rightComplete = rightId != NOT_SET ? counts[rightId] == sizes[rightId] : 0;

            if (id == NOT_SET || ((originalOnly || complete) && !solution[row][col][ORIGINAL]))
            {
                printf("  ");
            }
            else
            {
                const int size = sizes[id];
                if (printId)
                {
                    if (id < 10) // two digits
                        printf(" %d", id);
                    else
                        printf("%d", id);
                }
                else
                {
                    if (size < 10) // two digits
                        printf(" %d", size);
                    else
                        printf("%d", size);
                }
            }
            if (col != width - 1)
            {
                if (col + 1 >= width) // board border
                {
                    printf("|");
                }
                else if (solution[row][col][CERTAIN] == NOT_SET)
                {
                    printf(rightComplete ? "|" : " ");
                }
                else if (solution[row][col + 1][CERTAIN] != NOT_SET
                         && solution[row][col][CERTAIN] != solution[row][col + 1][CERTAIN])
                {
                    printf("|");
                }
                else if (solution[row][col + 1][CERTAIN] == NOT_SET
                         && complete)
                {
                    printf("|");
                }
                else
                {
                    printf(" ");
                }
            }
        }

        if (row != height - 1) // if not last row
        {
            printf("|\n+");
            for (int col = 0; col < width; ++col)
            {
                const int id = solution[row][col][CERTAIN]; // TODO refactor with branch above
                const int complete = counts[id] == sizes[id];
                const int downId = row + 1 < MAX_HEIGHT ? solution[row + 1][col][CERTAIN] : NOT_SET;
                const int downComplete = downId != NOT_SET ? counts[downId] == sizes[downId] : 0;

                if (row + 1 >= height) // board border
                {
                    printf("--+");
                }
                else if (solution[row][col][CERTAIN] == NOT_SET)
                {
                    printf(downComplete ? "--+" : "  +");
                }
                else if (solution[row + 1][col][CERTAIN] != NOT_SET
                         && solution[row][col][CERTAIN] != solution[row + 1][col][CERTAIN])
                {
                    printf("--+");
                }
                else if (solution[row + 1][col][CERTAIN] == NOT_SET
                         && complete)
                {
                    printf("--+");
                }
                else
                {
                    printf("  +");
                }
            }
            // print border
        }
        else
        {
            printf("|");
        }
        printf("\n");
    }

    // print last border line
    printf("+");
    for (int col = 0; col < width; ++col)
    {
        printf("--+");
    }
    printf("\n");
}

int solve(int solution[][MAX_WIDTH][MATRIX_SIZE], char sizes[],
    const int width, const int height, int branch, int dbgDepth)
{
    int res;
    int solutionCount = 0;

    int cloneCounts[MAX_HEIGHT * MAX_WIDTH];
    int cloneCertain[MAX_HEIGHT][MAX_WIDTH]; // MEMORY: not [height][widht] for better dbg, allocate it dynamically
    // save CERTAIN matrix so this branch can be reverted later
    for (int row = 0; row < height; ++row)
    {
        for (int col = 0; col < width; ++col)
        {
            cloneCounts[row * MAX_HEIGHT + col] = counts[row * MAX_HEIGHT + col];
            cloneCertain[row][col] = solution[row][col][CERTAIN];
        }
    }
    int firstRun = 1; // only use branch on first run
    do
    {
        res = fillCertainBoxes(solution, sizes, width, height, firstRun ? branch : NOT_SET);
        firstRun = 0;

        if (res == NO_MORE_BRANCHES)
        {
            solutionCount = NO_MORE_BRANCHES;
            goto cleanUp; // TODO or just break
        }
        else if (res == SOLUTION_FOUND)
        {
            // printf("final solution:\n");
            // printSolution(solution, sizes, width, height, 1, 1);

            solutionCount = 1;
            goto cleanUp;
        }
        else if (res == NO_SOLUTION)
        {
            // reseni tohohle branche neexistuje
            // tzn. pokud mam branch, jenom se popne stack na vyssi branch,
            // jinak se vypise reseni pokud je tohle ve stacku nejvyssi solve
            solutionCount = 0;
            goto cleanUp;
        }
        else if (res == NOT_FOUND)
        {
            int branchToForce = 0; // index
            int status;
            int innerCloneCounts[MAX_HEIGHT * MAX_WIDTH];
            int innerCloneCertain[MAX_HEIGHT][MAX_WIDTH]; // MEMORY: alloc dynamically
            // printf("%*s%s%d\n", dbgDepth * 2, "", "save INNER state depth ", dbgDepth);
            for (int row = 0; row < height; ++row)
            {
                for (int col = 0; col < width; ++col)
                {
                    innerCloneCounts[row * MAX_HEIGHT + col] = counts[row * MAX_HEIGHT + col];
                    innerCloneCertain[row][col] = solution[row][col][CERTAIN];
                }
            }
            do {
                // printf("\nbase board:\n");
                // printSolution(solution, sizes, width, height, 0, 1);
                // printf("%*s%d\n", dbgDepth * 2, "", branchToForce);

                status = solve(solution, sizes, width, height, branchToForce, dbgDepth + 1);
                if (status > 0) {
                    // negative numbers are reserved for status
                    solutionCount += status;
                }

                // printf("%*s%s%d\n", dbgDepth * 2, "", "load INNER state depth ", dbgDepth);
                for (int row = 0; row < height; ++row)
                {
                    for (int col = 0; col < width; ++col)
                    {
                        solution[row][col][CERTAIN] = innerCloneCertain[row][col];
                        counts[row * MAX_HEIGHT + col] = innerCloneCounts[row * MAX_HEIGHT + col];

                        // TODO this should really not be handleded here
                        // PERFORMANCE is hurt by this too!
                        for (int i = 0; i < MAX_BOXES; ++i)
                        {
                            solution[row][col][i] = NOT_SET;
                        }
                    }
                }

                branchToForce++;
            } while (status != NO_MORE_BRANCHES);
            goto cleanUp;
        }
    } while (res > 0); // number of changes found

    // PERFORMANCE: after branching, this is not neccessary as it's wiped out after each branch
cleanUp:
     // if branch is not set, it had max one solution, keep it so we can print it
    if (branch != NOT_SET)
    {
        for (int row = 0; row < height; ++row)
        {
            for (int col = 0; col < width; ++col)
            {
                solution[row][col][CERTAIN] = cloneCertain[row][col];
                counts[row * MAX_HEIGHT + col] = cloneCounts[row * MAX_HEIGHT + col];
            }
        }
    }

    return solutionCount;
}

int main()
{
    #ifndef __PROGTEST__
        clock_t begin, end;
        double time_spent;
        begin = clock();
    #endif

    // MEMORY: dynamically allocate sizes
    // MEMORY: dynamically allocate solution
    int board[MAX_HEIGHT][MAX_WIDTH];
    int (*p_board)[MAX_WIDTH] = board;
    char sizes[MAX_HEIGHT * MAX_WIDTH];

    int solution[MAX_HEIGHT][MAX_WIDTH][MATRIX_SIZE] = {{{0}}};
    for (int row = 0; row < MAX_HEIGHT; ++row)
    {
        for (int col = 0; col < MAX_WIDTH; ++col)
        {
            board[row][col] = 0;
            for (int i = 0; i < MATRIX_SIZE; ++i)
            {
                solution[row][col][i] = NOT_SET;
            }
        }
    }

    printf("Zadejte puzzle:\n");
    int width, height, sum;
    loadPuzzle(p_board, &width, &height, &sum);

    if (sum != width * height) // optimization
    {
        printf("Reseni neexistuje.\n");
        return 0;
        // else does not mean it does have a solution
    }

    for (int row = 0; row < height; ++row)
    {
        for (int col = 0; col < width; ++col)
        {
            const int id = row * MAX_WIDTH + col;
            // set unique id for this box
            if (board[row][col] != 0)
            {
                solution[row][col][CERTAIN] = id;
                solution[row][col][ORIGINAL] = 1;
                counts[id] = 1;
                sizes[id] = board[row][col];
            }
            else
            {
                solution[row][col][CERTAIN] = NOT_SET;
                solution[row][col][ORIGINAL] = 0;
                sizes[id] = 0;
            }
        }
    }

    // MEMORY: might as well free $board now.

    int result = solve(solution, sizes, width, height, NOT_SET, 0);
    if (result == 0)
    {
        printf("Reseni neexistuje.\n");
    }
    else if (result == 1)
    {
        printf("Jedno reseni:\n");
        printSolution(solution, sizes, width, height, 1, 0);
    }
    else
    {
        printf("Celkem reseni: %d\n", result);
    }

    #ifndef __PROGTEST__
        end = clock();
        time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
        printf("~~~%f~~~\n", time_spent);
    #endif
}

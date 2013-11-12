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
#define MATRIX_SIZE (MAX_BOXES + 1) // for CERTAIN
#define NOT_SET -1 // solutions[][][CERTAIN]
#define NOT_FOUND -1 // fillCertainBoxes return value
#define NO_SOLUTION -2 // fillCertainBoxes return value
#define SOLUTION_FOUND -3 // fillCertainBoxes return value

// a is current sount, max 99
// b is expected count, max 99
// TODO use another data types! char
// TODO 2 remove it altogether as only b is needed
struct Pair
{
    int a;
    int b;
};

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
int counts[MATRIX_SIZE];

/**
 * Should converge faster with sizes sorted by size desc
 * @return int index of last filled box or NOT_FOUND
 */
int fillCertainBoxes(int solution[][MAX_WIDTH][MATRIX_SIZE], struct Pair sizes[], const int width, const int height)
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
     // TODO if there are more number of the same id, each must must contain all of them

    // TODO MAJOR if there count(id) == sizes, skip it, so it does not create new possible paths! !!!
    int done = 1;
    for (int row = 0; row < height; ++row)
    {
        for (int col = 0; col < width; ++col)
        {
            if (solution[row][col][CERTAIN] == NOT_SET)
            {
                continue;
            }

            const int id = row * MAX_WIDTH + col;

            // if this rectangle is complete
            if (counts[id] == sizes[id].b)
                continue;
            const int size = sizes[id].b; // if sort is not active, which indeed isn't
            done = 0;

            int box_width = 1;
            int box_height;
            // PERFORMANCE: profile this
            printf("id=%d\n",id);
            int currentRectangleIsPossible = 0;

            while (box_width <= size)
            {
                if (size % box_width == 0) {
                    box_height = size / box_width;
                    // TODO test if this rectangle fits, starting from any point containg current row col
                    // move it all the way up and test all moves down
                    for (int y = row + (-box_height + 1); y <= row; ++y)
                    {
                        if (y < 0 || y + box_height > height) continue; // outside of board

                        // move it all the way to the left and test all moves to right
                        for (int x = col + (-box_width + 1); x <= col; ++x)
                        {
                            if (x < 0 || x + box_width > width) continue; // outside of board

                            // test if [x,y] to [x+width, y+height] is empty
                            for (int test_y = y; test_y < y + box_height; ++test_y)
                            {
                                for (int test_x = x; test_x < x + box_width; ++test_x)
                                {
                                    if (solution[test_y][test_x][CERTAIN] != NOT_SET
                                        && solution[test_y][test_x][CERTAIN] != id)
                                    goto next; // break;
                                }
                            }
                            // if we got here, the box fits: write to solution as possible number
                            currentRectangleIsPossible = 1;
                            for (int test_y = y; test_y < y + box_height; ++test_y)
                            {
                                for (int test_x = x; test_x < x + box_width; ++test_x)
                                {
                                    if (test_y == row && test_x == col) continue; // already in CERTAIN

                                    for (int index = 0; /* must break */; ++index)
                                    {
                                        if (solution[test_y][test_x][index] == NOT_SET)
                                        {
                                            printf("[%d][%d] could be id %d\n", test_y, test_x, id);
                                            solution[test_y][test_x][index] = id;
                                            break;
                                        }
                                        else if (solution[test_y][test_x][index] == id)
                                        {
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
            if (!currentRectangleIsPossible)
            {
                return NO_SOLUTION;
            }
        }
    }

    if (done)
    {
        return SOLUTION_FOUND;
    }

    /**
     * For all boxes look if there is only one way to fill a box. If so, move it to certain and start again.
     */
    int changeFound = 0;
    for (int row = 0; row < height; ++row)
    {
        for (int col = 0; col < width; ++col)
        {
            // only one id is tracked as possible
            if (solution[row][col][0] != NOT_SET && solution[row][col][1] == NOT_SET)
            {
                changeFound++;
                if (solution[row][col][CERTAIN] != NOT_SET)
                {
                    return NO_SOLUTION;
                }
                const int id = solution[row][col][0];
                solution[row][col][CERTAIN] = id;
                counts[id] += 1;
            }
            // clear possible values
            // PERFORMANCE: could check, if value is zero break, but the check might be slow
            for (int i = 0; i < MAX_BOXES; ++i)
            {
                solution[row][col][i] = NOT_SET;
            }
        }
    }

    return changeFound ? 1 : NOT_FOUND;
}

// int compareSizesDesc(const void * first, const void * second)
// {
//  return ( ((struct Pair*) second)->b - ((struct Pair*) first)->b );
// }

void printRawSolution(int solution[][MAX_WIDTH][MATRIX_SIZE], struct Pair sizes[], const int width, const int height)
{
    for (int row = 0; row < height; ++row)
    {
        for (int col = 0; col < width; ++col)
        {
            const int id = solution[row][col][CERTAIN];
            printf("%d ", id);
        }
        printf("\n");
    }
}

void printSolution(int solution[][MAX_WIDTH][MATRIX_SIZE], struct Pair sizes[], const int width, const int height)
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
            if (id == NOT_SET)
            {
                printf("  ");
            }
            else
            {
                const int size = sizes[id].b;
                if (size < 10) // two digits
                    printf(" %d", size);
                else
                    printf("%d", size);
            }
            if (col != width - 1)
            {
                if (col + 1 >= width || (
                    solution[row][col][CERTAIN] != solution[row][col + 1][CERTAIN]
                    && solution[row][col][CERTAIN] != NOT_SET
                    && solution[row][col + 1][CERTAIN] != NOT_SET
                ))
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
                if (solution[row][col][CERTAIN] != solution[row + 1][col][CERTAIN]
                    && solution[row][col][CERTAIN] != NOT_SET
                    && solution[row + 1][col][CERTAIN] != NOT_SET
                    )
                    printf("--+");
                else
                    printf("  +");
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

int main()
{
    #ifndef __PROGTEST__
        clock_t begin, end;
        double time_spent;
        begin = clock();
    #endif

    // MEMORY: dynamically allocate sizes
    // MEMORY: dynamically allocate solution
    int board[MAX_HEIGHT][MAX_WIDTH] = {{0}}; // TODO check this works
    int (*p_board)[MAX_WIDTH] = board;
    struct Pair sizes[MAX_HEIGHT * MAX_WIDTH]; // could be mapped to MAX_BOXES instead but this is faster
    int solution[MAX_HEIGHT][MAX_WIDTH][MATRIX_SIZE] = {{{0}}};
    for (int row = 0; row < MAX_HEIGHT; ++row)
    {
        for (int col = 0; col < MAX_WIDTH; ++col)
        {
            for (int i = 0; i < MATRIX_SIZE; ++i)
            {
                solution[row][col][i] = NOT_SET;
            }
        }
    }

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
        return 0;
        // TODO else ale to jeste neznamena, ze existuje
    }

    int blanks = width * height;
    for (int row = 0; row < height; ++row)
    {
        for (int col = 0; col < width; ++col)
        {
            int id = row * MAX_WIDTH + col;
            // set unique id for this box
            if (board[row][col] != 0)
            {
                solution[row][col][CERTAIN] = id;
                counts[id] = 1; // there is exactly one number with this id on the board at this point
                sizes[id].b = board[row][col];
                blanks--;
            }
            else
            {
                solution[row][col][CERTAIN] = NOT_SET;
                sizes[id].a = 0;
                sizes[id].b = 0;
            }
        }
    }

    if (blanks == 0)
    {
        // done
    }

    // no need to sort full length of sizes, only used are better.
    //qsort(sizes, height * width, sizeof(struct Pair), compareSizesDesc);
    // printf("after qsort\n");
    // for (int i = 0; i < height; ++i)
    // {
    //  for (int k = 0; k < width; ++k)
    //  {
    //      int id = i * MAX_WIDTH + k;
    //      printf("%d %d\n", sizes[id].a, sizes[id].b);
    //  }
    // }

    // Make deterministic partial solutions.
    // PERFORMANCE: Worst case first number is changed and then the
    // puzzle is reiterated all over again => break after change and
    // loop only once, or continue and break on the changed index.

    // MEMORY: might as well free $board now.
    int res;
    do
    {
        res = fillCertainBoxes(solution, sizes, width, height);
        //printRawSolution(solution, sizes, width, height);
        printSolution(solution, sizes, width, height);
        if (res == SOLUTION_FOUND)
        {
            printf("Jedno reseni:\n");
            printSolution(solution, sizes, width, height);
            break;
        }
        else if (res == NO_SOLUTION)
        {
            printf("Reseni neexistuje.\n");
            break;
        }
        else if (res == NOT_FOUND)
        {
            printf("Reseni asi existuje ale algo se zasek (konec nebo branching).\n");
            printSolution(solution, sizes, width, height);
        }
    } while (res > 0); // number of changes found
    // split paths, pick one number, continue, then pick other paths


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

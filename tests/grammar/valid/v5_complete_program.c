#include <stdio.h>

/*
 * The is a multi line
 *
 * comment
 */

int32_t main(int32_t, int32_t[]) {
    int32_t i;
    int64_t j;;
    char a;;;
// Comment here
    int32_t d, e;// Other comment here

    char c = 'c';
    char d = 'dsdqsd';
    int32_t k = 1;
    int32_t l = -25;
    int64_t m = 10;

    int32_t espaceChar = '\n';
    int32_t espaceChar = '\n';
    int32_t espaceChar = '\r';
    int32_t espaceChar = '\t';
    int32_t espaceChar = '\\';
    int32_t espaceChar = '\'';
    int32_t espaceChar = '\"';

    int32_t line
            = 4 + 4 - (5 * 6);

    int32_t p = (
            1 + 2
    );

    void f() {}

    int32_t func(int32_t a, int32_t) {}

    int32_t bar(int32_t a, int32_t[], char comment[]) {}


    int32_t add = 3 + 2 - 1;

    int32_t mult = 1 * 2 / 3 % 5;

    int32_t tab[] = {1, 2};
    int32_t tab[] = {};
    int32_t tab[] = {1, 2};
    int32_t tab[14];
    int32_t tab[14] = {};
    int32_t tab[3] = {1, 2, 3};
    int32_t tab[0] = {a = 6, 1 > 2, f(g(1, 2))};
    int32_t tab[5]
            =
            {
                    1,
                    2,
                    3
            };

    puts('c');

    while (i < 10) {
        if (i % 2 == 0) {
            put('a' + i);
        } else if (0 != 1 || 0 < 1 && 0 <= 1) {
            m--;
        } else {
            i = i + 2;
        }
        i++;
    }
    return 0;
}

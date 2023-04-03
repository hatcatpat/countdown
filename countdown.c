#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#define ABS(x) ((x) < 0 ? -(x) : (x))

#define DEBUG 0

#define NUM_SMALL 20
#define NUM_LARGE 4
#define NUM_CARDS 6

int small[NUM_SMALL] = {
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
};
int large[NUM_LARGE] = {
    25, 50, 75, 100,
};

typedef int (*binop)(int, int);

int add(int a, int b) {
    return a + b;
}
int mult(int a, int b) {
    return a * b;
}
int sub(int a, int b) {
    return a - b;
}
int subrev(int a, int b) {
    return b - a;
}
int divi(int a, int b) {
    if (b == 0)
        return 0;
    return a / b;
}
int divirev(int a, int b) {
    if (a == 0)
        return 0;
    return b / a;
}

bool can_divide(int a, int b) {
    if(b == 0)
        return false;

    a = ABS(a), b = ABS(b);
    if(a == b)
        return true;
    else
        return (a % b) == 0;
}

bool can_subtract(int a, int b) {
    return a > b;
}

enum E_OP {
    ADD,
    MULT,
    SUB,
    SUBREV,
    DIVI,
    DIVIREV,
    NUM_BINOPS
};
binop binops[NUM_BINOPS] = {
    add,
    mult,
    sub,
    subrev,
    divi,
    divirev
};
const char *binop_names[NUM_BINOPS] = {
    "+",
    "*",
    "-",
    "rev-",
    "/",
    "rev/"
};

const char* get_binop_name(binop op) {
    for(int i = 0; i < NUM_BINOPS; ++i) {
        if(binops[i] == op)
            return binop_names[i];
    }
    return "unknown";
}

void pretty_binop(binop op, int a, int b) {
    const char *str = NULL;
    bool rev = false;
    if(op == subrev) {
        rev = true;
        str = binop_names[SUB];
    } else if(op == divirev) {
        rev = true;
        str = binop_names[DIVI];
    } else {
        for(int i = 0; i < NUM_BINOPS; ++i) {
            if(binops[i] == op)
                str = binop_names[i];
        }
    }

    if(rev)
        printf("%i %s %i", b, str, a);
    else
        printf("%i %s %i", a, str, b);
}

binop choose_binop(int a, int b) {
    binop valid_binops[NUM_BINOPS] = {0};
    int p = 0;

    valid_binops[p++] = add;
    if(can_subtract(a,b))
        valid_binops[p++] = sub;
    if(can_subtract(b,a))
        valid_binops[p++] = subrev;
    if(can_divide(a,b))
        valid_binops[p++] = divi;
    if(can_divide(b,a))
        valid_binops[p++] = divirev;

    return valid_binops[rand() % p];
}

int get_random(int lo, int hi) {
    return lo + rand() % ((hi + 1) - lo);
}

void shuffle(int data[], size_t size, size_t take) {
    for(int i = 0; i < take - 1; ++i) {
        int j = (i + rand()) % (size - i);
        int t = data[i];
        data[i] = data[j];
        data[j] = t;
    }
}

struct op_pair {
    int a, b, x;
    binop op;
};

void countdown(int cards[2][NUM_CARDS]) {
    struct op_pair solution[NUM_CARDS];

    int p = 0;
    for(int i = 0; i < NUM_CARDS - 1; ++i) {
        int n = NUM_CARDS - i;
        int ra = get_random(0, n - 1);
        int rb = (ra + get_random(1, n - 1)) % n;
        int a = cards[p][ra];
        int b = cards[p][rb];
        binop op = choose_binop(a, b);
        int x = op(a, b);

        solution[i] = (struct op_pair) {
            a, b, x, op
        };

#if DEBUG
        printf("[%i] ", i);
        pretty_binop(op, a, b);
        printf(" = %i\n", x);
#endif

        cards[!p][0] = x;
        int k = 1;
        for(int j = 0; j < n; ++j) {
            if(j == ra || j == rb)
                continue;
            cards[!p][k++] = cards[p][j];
        }

        p = !p;
    }

    printf("target: %i\n", cards[p][0]);
    printf("press enter for the solution...\n");
    fflush(stdin);
    getchar();

    for(int i = 0; i < NUM_CARDS - 1; ++i) {
        struct op_pair *pair = &solution[i];
        printf("[%i] ", i);
        pretty_binop(pair->op, pair->a, pair->b);
        printf(" = %i\n", pair->x);
    }
}

void countdown_random() {
    static int cards[2][NUM_CARDS];
    static int small_shuffle[NUM_SMALL];
    static int large_shuffle[NUM_LARGE];

    memcpy(large_shuffle, large, sizeof(int) * NUM_LARGE);
    memcpy(small_shuffle, small, sizeof(int) * NUM_SMALL);

    int num_large = get_random(0, NUM_LARGE);
    int num_small = NUM_CARDS - num_large;

    shuffle(small_shuffle, NUM_SMALL, num_small);

    if(num_large)
        shuffle(large_shuffle, NUM_LARGE, num_large);

    for(int i = 0; i < num_large; ++i)
        cards[0][i] = large_shuffle[i];
    for(int i = 0; i < num_small; ++i)
        cards[0][num_large + i] = small_shuffle[i];

    printf("cards: ");
    for(int i = 0; i < NUM_CARDS; ++i)
        printf("%i ", cards[0][i]);
    printf("\n");

    countdown(cards);
}

void countdown_choose() {
    static int cards[2][NUM_CARDS];
    static char ch;

    int i = 0;
    int num_large = 0;
    int num_small = 0;
    while (i < NUM_CARDS) {
        printf("[l]arge (%i/%i) or [s]mall (%i/%i): \n", num_large, NUM_LARGE, num_small, NUM_SMALL);
        fflush(stdin);

        if((ch = getchar()) == 'q')
            break;

        if(ch == 'l' && num_large < NUM_LARGE) {
            cards[0][i++] = large[get_random(0, NUM_LARGE - 1)];
            num_large++;
        }
        else if(ch == 's' && num_small < NUM_SMALL) {
            cards[0][i++] = small[get_random(0, NUM_SMALL - 1)];
            num_small++;
        }

        printf("cards: ");
        for(int j = 0; j < i; j++)
            printf("%i ", cards[0][j]);
        printf("\n");
    }

    if(i == NUM_CARDS)
        countdown(cards);
}

int main(int argc, char *argv[]) {
    srand(time(NULL));

    int opt;
    bool use_random = false;
    int num_small = 0;
    int num_large = 0;
    while((opt = getopt(argc, argv, "rl:s:")) != -1) {
        switch(opt) {
        case 'r':
            use_random = true;
            break;
        case 's':
            if(num_small < NUM_SMALL)
                small[num_small++] = ABS(atoi(optarg));
            else
                printf("[error] too many small cards given\n");
            break;
        case 'l':
            if(num_large < NUM_LARGE)
                large[num_large++] = ABS(atoi(optarg));
            else
                printf("[error] too many large cards given\n");
            break;
        }
    }

    if(num_large) {
        printf("large: ");
        for(int i = 0; i < NUM_LARGE; ++i)
            printf("%i ", large[i]);
        printf("\n");
    }
    if(num_small) {
        printf("small: ");
        for(int i = 0; i < NUM_SMALL; ++i)
            printf("%i ", small[i]);
        printf("\n");
    }

    if(use_random)
        countdown_random();
    else
        countdown_choose();

    return 0;
}

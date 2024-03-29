#include <stdio.h>

#include "api.h"

#define MAX 1000

int main() {
    char flag[MAX], s[8];

    for (int i = 0; i < MAX; i++) {
        flag[i] = 0;
    }

    for (int i = 2; i < MAX; i++) {
        if (flag[i] == 0) {
            /*没有标记的为质数*/
            sprintf(s, "%d ", i);
            api_putstr(s);
            for (int j = 1 * 2; j < MAX; j += i) {
                flag[j] = 1;
            }
        }
    }

    api_end();

    return 0;
}
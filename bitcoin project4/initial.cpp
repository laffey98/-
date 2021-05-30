#include <memory.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include"ecc.c"
#include "block struct.h"

block a;

int main() {
    block* as=&a;
    FILE* fp = fopen("block0.dat", "wb+");
    fwrite(as, sizeof(a), 1, fp);
    fclose(fp);
    return 0;
}
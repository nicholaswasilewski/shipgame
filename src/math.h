#ifndef _MATH_H__
#define _MATH_H__

#include <ctime>
#include <cstdlib>

#define PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062


void SeedRandom() {
    srand(time(NULL));
}

int Random(int Min, int Max) {
    return Min + rand()%(Max-Min);
}

int Random(int Max) {
    return Random(0, Max);
}

float Random()
{
    return ((float)rand()) / (float)RAND_MAX;
}

float Random(float Min, float Max)
{
    return Min + (((float)rand()) / ((float)RAND_MAX / (Max - Min)));
}

#endif

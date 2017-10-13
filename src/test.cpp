//Alternate entry point for testing

#include "matrixMath.cpp"
#include "loadFBX.cpp"

int Test(int argc, char** argv)
{
    printf("Testing\n");


    FILE* monkeyFile =  fopen("monkey.fbx", "r");
    ParseFBX(monkeyFile);
    
/*
    mat4 M4 = { 1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f };
    PrintMatrix(M4);
*/

    return 0;
}

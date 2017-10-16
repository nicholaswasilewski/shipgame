#ifndef _FBXHELPERS_CPP__
#define _FBXHELPERS_CPP__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "loadFBX.cpp"

FBX_Node* FBX_GetChildByName(FBX_Node* node, char* name)
{
    for(int i = 0; i < node->ChildCount; i++)
    {
        FBX_Node* child = node->Children[i];
        if (strcmp(name, child->Name) == 0)
        {
            return child;
        }
    }

    return NULL;
}

#endif

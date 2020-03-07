#ifndef _NAMES_H__
#define _NAMES_H__

#include "hash.cpp"
#include "entity.cpp"
#include "memory.h"

// Name system, used to test hash table
struct names
{
    hashtable EntityToNameTable;
	
    int Capacity;
    int Count;
    // Array of names sequentially added.
    char** Names;
	
    void AddEntity(entity Entity)
	{
		if (Count < Capacity)
		{
			int NameIndex = Count++;
			Names[NameIndex] = 0;
			printf("Array inserting entity %d at index %d.\n", Entity.Index, NameIndex); 
			Insert(EntityToNameTable, Entity.Index, NameIndex);
		}
    }
	
    void AddEntity(entity Entity, const char* Name)
	{
		AddEntity(Entity);
		SetName(Entity, Name);
	}
	
    void RemoveEntity(entity Entity)
    {
	
    }
    
    void SetName(entity Entity, const char* Name)
    {
		find_result Result = Find(EntityToNameTable, Entity.Index);
		if (Result.Found)
		{
			Names[Result.Value] = (char *)Name;
		}
    }

    const char* GetName(entity Entity)
	{
		find_result Result = Find(EntityToNameTable, Entity.Index);
		if (Result.Found)
		{
			return Names[Result.Value];
		}
		else
		{
			return 0;
		}
    }
};

names CreateNameSystem(memory_arena Arena, int Capacity)
{
    names NameSystem = {
		.EntityToNameTable = CreateHashTable(Capacity, &Arena),
		.Capacity = Capacity,
		.Count = 0,
		.Names = PushArray(&Arena, 10, char*)
    };
    return NameSystem;
}

#endif

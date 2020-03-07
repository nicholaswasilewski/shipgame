#ifndef _HASH_CPP__
#define _HASH_CPP__

#include "assert.h"
#include "memory.h"
#include "numerical_types.h"

// hashtable holds a sparse array of nodes. each node chains to a next node in the event of a collision.

// the hashtable requires external storage and cooperation if you want it to do anything but map numbers to other numbers.

struct hashnode {
    int Key;
    int Value;
    hashnode* Next;
};

struct hashtable {
    memory_arena Arena;
    int Capacity;
    hashnode* Nodes;
};



constexpr uint64 kHashOffsetBasis = 0x84222325;
constexpr uint64 kHashPrimeMultiplier = 0x000001b3;

uint64 Hash(uint8* str, size_t len=SIZE_MAX) {
	if (str == nullptr || *str == 0 || len == 0) {
		return 0;
	}

	// A quick good hash, from:
	// https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
	// Specifically, the FNV-1a function.
	size_t count = 0;
	uint64 value = kHashOffsetBasis;
	while (*str && count < len) {
		value = (value ^ *str++) * kHashPrimeMultiplier;
		++count;
	}

	return value;
}

// Returns an unnormalized value. Mod to get back into range.
uint64 HashInt(uint64 Value)
{
	constexpr int ArrayLength = sizeof(Value);
	uint8 bytes[ArrayLength];
	memcpy(&bytes, &Value, ArrayLength);
	return Hash(&bytes[0], ArrayLength);
}

hashtable CreateHashTable(int Capacity, memory_arena *Arena)
{
    hashtable Hashtable;
    Hashtable.Capacity = Capacity;
    Hashtable.Nodes = PushArray(Arena, Capacity, hashnode);
    Hashtable.Arena = PushArena(Arena, 512);
    memset(Hashtable.Nodes, -1, Capacity*sizeof(hashnode));
    return Hashtable;
}

void Insert(hashtable Hashtable, int Key, int Value)
{
	printf("Inserting %d in %d.\n", Key, Value);
	int Hash = HashInt(Key)%Hashtable.Capacity;
    hashnode* Node = Hashtable.Nodes+Hash;
    // Next vaue of -1 == uninitialized hash node
    if (Node->Next == (hashnode*)-1)
    {
		Node->Key = Key;
		Node->Value = Value;
		Node->Next = 0;
		printf("Hash inserting KV:%d:%d at %d\n", Key, Value, Hash);
    }
    else 
    {
		while (true)
		{
			if (Node->Key == Key)
			{
				Node->Value = Value;
				printf("Key exists, changing value");
			}
			else if (Node->Next == 0)
			{
				Node->Next = PushObject(&Hashtable.Arena, hashnode);
				Node = Node->Next;
				Node->Key = Key;
				Node->Value = Value;
				Node->Next = 0;
				printf("Hash collision. Pushing new node.\n");
			}
			else
			{
				Node = Node->Next;
				continue;
			}
			break;
		}
    }
}

void Remove(hashtable Hashtable, int Key)
{
    int Hash = HashInt(Key)%Hashtable.Capacity;
    hashnode* Node = Hashtable.Nodes+Hash;
    
}

struct find_result
{
    bool Found;
    int Value;
};

find_result Find(hashtable Hashtable, int Key)
{
    int Hash = HashInt(Key)%Hashtable.Capacity;
    printf("Looking for Key:%d with hash: %d.\n", Key, Hash);
    hashnode *Node = Hashtable.Nodes+Hash;
    bool Found = false;
    find_result Result = {0};
    while (Node != 0)
    {
		if (Node->Key == Key)
		{
			Result.Found = true;
			Result.Value = Node->Value;
			break;
		}
		else
		{
			Node = Node->Next;
		}
    }
	
    return Result;
}

#endif

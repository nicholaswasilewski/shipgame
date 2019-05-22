#include "memory.h"

// hashtable holds a sparse array of nodes. each node chains to a next node in the event of a collision.

// the hashtable requires external storage and cooperation if you want it to do anything but map numbers to other numbers.

struct hashnode {
    int Value;
    int Index;
    hashnode* Next;
}

struct hashtable {
    int Capacity;
    hashnode* Nodes;
};

hashtable CreateHashTable(int Capacity, memory_arena Arena)
{
    hashtable Hashtable;
    Hashtable.Capacity = Capacity;
    hashtable.Nodes = PushArray(Arena, Capacity, hashnode);
    memset(hashTable.Nodes, -1, Capacity*sizeof(hashnode));
}

void Insert(hashtable Hashtable, int Value)
{
    int hash = Hash(Value)%Hashtable.Capacity;
    hashnode Node = Hashtable.Nodes[hash];
}

int Find(hashtable Hashtable, int Value)
{
    int hash = Hash(Value)%Hashtable.Capacity;
    
}

// Returns an unnormalized value. Mod to get back into range.
int Hash(int Value)
{
    //TODO: make this something other than...return value.
    return abs(Value);
}

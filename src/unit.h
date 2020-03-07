#ifndef _UNIT_H__
#define _UNIT_H__

#include "hash.cpp"

struct unit {
	float SwingTime;
	entity Target;

	float SwingSpeed;
	int MinDamage;
	int MaxDamage;

	int Armor;
	int Health;
};

struct system {
	hashtable EntityToIndex;
	int Capacity;
	int Count;
	void* Buffer;
};

struct units
{
    hashtable EntityIndexToUnitIndex;

    int Capacity;
    int Count;
    unit* UnitBuffer;
};


#endif

#include "math.h"
#include "unit.h"

void Initialize(units* Units, memory_arena* Arena)
{
	Units->Capacity = 10;
	Units->EntityIndexToUnitIndex = CreateHashTable(Units->Capacity, Arena),
	Units->Count = 0;
	Units->UnitBuffer = PushArray(Arena, Units->Capacity, unit);
}

unit* AddEntity(units* Units, entity Entity)
{
	if (Units->Count < Units->Capacity)
	{
		int Index = Units->Count;
		Units->UnitBuffer[Index] = {0};
		Insert(Units->EntityIndexToUnitIndex, Entity.Index, Index);
		Units->Count += 1;
		return &Units->UnitBuffer[Index];
	}
	else
	{
		printf("Unit capacity reached.");
		return 0;
	}
}

unit* GetComponent(units* Units, int Index)
{
	return &Units->UnitBuffer[Index];
}

unit* GetComponent(units* Units, entity Entity)
{
	find_result Result = Find(Units->EntityIndexToUnitIndex, Entity.Index);
	if (Result.Found)
	{
		return GetComponent(Units, Result.Value);
	}
	else
	{
		return 0;
	}
}

void SetTarget(units* Units, entity Targeter, entity Target) {
	unit* TargeterUnit = GetComponent(Units, Targeter.Index);
	TargeterUnit->Target = Target;
}

float GetCritChance(unit* Unit)
{
	return 5.0;
}

float GetMissChance(unit* Unit)
{
	return 5.0;
}

float GetSwingSpeed(unit* Unit)
{
	return Unit->SwingSpeed;
}

float GetArmor(unit* Unit)
{
	return Unit->Armor;
}

float GetHealth(unit* Unit)
{
	return Unit->Health;
}

typedef enum {
	AttackResultUnknown = -1,
	AttackResultMiss,
	AttackResultCrit,
	AttackResultHit,
	AttackResultCount
} attack_result;

typedef struct {
	float Table[AttackResultCount];
} attack_table;

attack_table GetAttackTable(unit* Unit)
{
	attack_table Table;
	Table.Table[AttackResultMiss] = GetMissChance(Unit);
	Table.Table[AttackResultCrit] = GetCritChance(Unit);
	Table.Table[AttackResultHit] = 100.0f; // Rest of table is normal hits.
	return Table;
}

// Damage source can be 
void LogWeaponSwingEvent(attack_result Result) {
	// These logging and string generation for these events should occur away from this file so dom't think too hard about it right now.
	const char* ResultText = "";
	switch (Result) {
		case AttackResultMiss:
			ResultText = "Miss!";
			break;
		case AttackResultCrit:
			ResultText = "Crit!";
			break;
		case AttackResultHit:
			ResultText = "Hit.";
			break;
		default:
			ResultText = "";
	}
	printf("%s\n", ResultText);
}


// Attack roll should be a randomly determined value between 0 and 100.
attack_result ComputeAttackResult(attack_table AttackTable,  float AttackRoll)
{
	float Accumulator = 0.0f;
	for(int i = 0; i < AttackResultCount; i++)
	{
		Accumulator += AttackTable.Table[i];
		if (Accumulator >= AttackRoll) {
			return (attack_result)i;
		} 
	}
	return AttackResultUnknown;
}

void Update(units* Units, float dt) {
	for (int i = 0; i < Units->Count; i++) 
	{
		unit* Unit = GetComponent(Units, i);
		float SwingSpeed = GetSwingSpeed(Unit);
		if (SwingSpeed > 0.0f) {
			Unit->SwingTime += dt;
			if (Unit->SwingTime > SwingSpeed) {
				printf("Swing!\n");
				float HitRoll = Random(0.0f, 100.0f);
				attack_result Result = ComputeAttackResult(GetAttackTable(Unit), HitRoll);
				LogWeaponSwingEvent(Result);
				Unit->SwingTime -= SwingSpeed;
			}
		}
	}
}

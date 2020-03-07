#ifndef _ENTITY_CPP__
#define _ENTITY_CPP__

struct entity {
    int Index;
};

struct entity_factory
{
    const int NullEntityIndex = 0;
    // The plan right now is to never reuse an entity index but that might not be reasonable.
    int NextEntityIndex = NullEntityIndex + 1;
    
    entity CreateEntity() {
	return entity { .Index = NextEntityIndex++ };
    }
};

#endif

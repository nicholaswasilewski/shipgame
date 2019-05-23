#ifndef ASSERT_H__
#define ASSERT_H__

#include <assert.h>

#ifndef RELEASE
#define Assert(Expression)		    \
if(!(Expression))                           \
{					    \
    assert(Expression);			    \
}


#define DebugLog(Format, ...) { \
    const char* fname =  __FILE__; \
    const char* baseName = strrchr(fname, '\\'); \
    fname = baseName ? baseName+1:fname; \
    printf("%s:%d: " Format, fname, __LINE__, __VA_ARGS__);    \
}

#else
#define Assert(Expression, Message) {};
#define DebugLog(Format, ...) {};
#endif


#endif

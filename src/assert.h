#ifndef ASSERT_H__
#define ASSERT_H__

#ifndef RELEASE
#define Assert(Expression)                      \
if(!(Expression))                           \
{                                           \
    (*(int*)0 = 0);                         \
}


#define DebugLog(Format, ...) { \
    char* fname =  __FILE__; \
    char* baseName = strrchr(fname, '\\'); \
    fname = baseName ? baseName+1:fname; \
    printf("%s:%d: " Format, fname, __LINE__, __VA_ARGS__);    \
}

#else
#define Assert(Expression, Message) {};
#define DebugLog(Format, ...) {};
#endif


#endif
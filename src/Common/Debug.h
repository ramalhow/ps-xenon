#pragma once

#ifdef _DEBUG

#define Crash() {__debugbreak();}

#define HRCHECK( hr_ ) do{ \
    if( FAILED(hr_) ) { \
        printf("ERROR: failed call to %s (%s:%d), hr=0x%x\n", #hr_, __FILE__, __LINE__,hr_); \
        Crash(); \
    } } while(0);

#define ASSERT(expr) if(!(expr)) \
    { printf("[ASSERT ERROR]: expression %s failed at (%s:%d) ", ##expr_, __FILE__, __LINE__); }  

#else

#define Crash()

#define HRCHECK(X)

#define ASSERT(expr)

#endif

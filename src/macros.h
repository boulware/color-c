#define PASTE_(x,y) (x ## y)
#define PASTE(x,y)  PASTE_(x,y)
#define Assert(x) if(!(x)) {*((volatile int*)0) = 0;}
#define ArrayCount(x) (sizeof(x)/sizeof(x[0]))
#define Introspect
#define PASTE_(x,y) (x ## y)
#define PASTE(x,y)  PASTE_(x,y)
#define Assert(x) if(!(x)) {*((volatile int*)0) = 0;}
#define ArrayCount(x) (sizeof(x)/sizeof(x[0]))
#define Introspect
#define NoIntrospect
#define MetaBreakHere
#define mStringify(s) #s

#define Kilobyte(n) n*1024
#define Megabyte(n) n*1024*1024

// Creates a temporary string and formats it according to printf-style formatting rules.
// Use ONLY in a function with variadic arguments (e.g., void foo(int a, int b, ...);)
//
// Usage example:
//
// void foo(char *string, ...) {
// 		char *formatted_string;
// 		mFormatString(formatted_string, string);
// }
#define mFormatString(dst, fmt) \
	va_list args; \
	va_start(args, fmt); \
	dst = TempFormatString(fmt, args); \
	va_end(args);
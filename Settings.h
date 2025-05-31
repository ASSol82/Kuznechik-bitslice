#ifndef __SETTINGS__
#define __SETTINGS__


//#define _TERNARY_LOGIC 1 // включить троичную логику, т.е. использование 3-х входовых LUT-таблиц
#define MaxCountMessage 256 // необязательно здесь задавать этот параметр, можно перед началом компиляции


#ifndef MaxCountMessage
#define MaxCountMessage 64 // длина базового вектора, здесь задается значение 8,16,32,64,128,256
#endif


#if MaxCountMessage!=512 && MaxCountMessage!=256 && MaxCountMessage!=128 && MaxCountMessage!=64 && MaxCountMessage!=32 && MaxCountMessage!=16 && MaxCountMessage!=8
#error MaxCountMessage = 256,128,64,32,16,8
#endif


#define VERIFY_SIZE_TYPE(type, size) { char a[sizeof(type)==(size)?1:-1]; a[0]=0; }


#if defined _MSC_VER
#define _ALIGN(x) __declspec(align(x))
#else
#define _ALIGN(x) __attribute__ ((__aligned__(x)))
#endif


#endif // __SETTINGS__

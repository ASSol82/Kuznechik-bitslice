#ifndef __SETTINGS__
#define __SETTINGS__


#define MaxCountMessage 256 // необязательно здесь задавать этот параметр, можно перед началом компиляции


#ifndef MaxCountMessage
#define MaxCountMessage 256 // в этой реализации поддерживается лишь 256, для других значений требуется определить функции транспонирования данных
#endif


#if MaxCountMessage!=512 && MaxCountMessage!=256 && MaxCountMessage!=128 && MaxCountMessage!=64 && MaxCountMessage!=32 && MaxCountMessage!=16 && MaxCountMessage!=8
#error MaxCountMessage = 256,128,64,32,16,8
#endif


#if defined _MSC_VER
#define _ALIGN(x) __declspec(align(x))
#else
#define _ALIGN(x) __attribute__ ((__aligned__(x)))
#endif


#endif // __SETTINGS__

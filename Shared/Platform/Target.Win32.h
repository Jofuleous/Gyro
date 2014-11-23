#ifndef _TARGET_WIN32_H_
#define _TARGET_WIN32_H_

#pragma warning(disable: 4996)

//SHOULD EVER BE EXPLICITLY INCLUDED
//WIN32 TYPES BASED ON:
////http://msdn.microsoft.com/en-us/library/s3f49ktz(v=vs.80).aspx

#define CACHE_LINE				64 //64 byte cache line
#define CACHE_ALIGNED			__declspec(align(CACHE_LINE))

typedef unsigned __int8			u8;
typedef unsigned __int16		u16;
typedef unsigned __int32		u32;
typedef unsigned __int64		u64;

static const u64				MAX_U64	= 0xFFFFFFFFFFFFFFFF;
static const u32				MAX_U32	= 0xFFFFFFFF;
static const u16				MAX_U16	= 0xFFFF;
//static const u32				NULL = 0;



#endif
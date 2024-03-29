#ifndef EXT_TYPES_H
#define EXT_TYPES_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define FLT_MAX __FLT_MAX__
#define FLT_MIN __FLT_MIN__

#pragma GCC diagnostic ignored "-Wmultichar"
#pragma GCC diagnostic ignored "-Wscalar-storage-order"
#define StructBE     __attribute__((scalar_storage_order("big-endian")))
#define StructPacked __attribute__ ((packed))
#define thread_local _Thread_local
#define StructAligned(x) __attribute__((aligned(x)))

typedef signed char            s8;
typedef unsigned char          u8;
typedef signed short           s16;
typedef unsigned short         u16;
typedef signed int             s32;
typedef unsigned int           u32;
typedef signed long long int   s64;
typedef unsigned long long int u64;
typedef float                  f32;
typedef double                 f64;
typedef uintptr_t              uaddr_t;
typedef intptr_t               addr_t;
typedef time_t                 time_t;
typedef size_t                 size_t;
typedef wchar_t                wchar;
#define var_t __auto_type

#ifdef __clang__
#define onlaunch_func_t __attribute__ ((constructor)) void
#define onexit_func_t   __attribute__ ((destructor)) void
#else
#define onlaunch_func_t __attribute__ ((constructor)) static void
#define onexit_func_t   __attribute__ ((destructor)) static void
#endif

typedef volatile signed char            vs8;
typedef volatile unsigned char          vu8;
typedef volatile signed short           vs16;
typedef volatile unsigned short         vu16;
typedef volatile signed int             vs32;
typedef volatile unsigned int           vu32;
typedef volatile signed long long int   vs64;
typedef volatile unsigned long long int vu64;
typedef volatile bool                   vbool;

#ifndef EXT_BAREBONES
#include <pthread.h>
typedef pthread_t       thread_t;
typedef pthread_mutex_t mutex_t;
#endif

typedef struct {
	u8   hash[32];
	bool hashed;
} Hash;

typedef struct {
	f32 h;
	f32 s;
	f32 l;
} hsl_t;

typedef struct {
	f32 h;
	f32 s;
	f32 l;
	union {
		u8 alpha;
		u8 a;
	};
} hsla_t;

typedef union {
	struct {
		u8 r;
		u8 g;
		u8 b;
	};
	u8 c[3];
} rgb8_t;

typedef union {
	struct {
		u8 r;
		u8 g;
		u8 b;
		u8 a;
	};
	u8 c[4];
} rgba8_t;

typedef struct {
	f32 r;
	f32 g;
	f32 b;
} rgbf_t;

typedef struct {
	f32 r;
	f32 g;
	f32 b;
	f32 a;
} rgbaf_t;

enum IOLevel {
	PSL_NONE,
	PSL_NO_INFO,
	PSL_NO_WARNING,
	PSL_NO_ERROR,
};

typedef enum {
	SWAP_U8  = 1,
	SWAP_U16 = 2,
	SWAP_U32 = 4,
	SWAP_U64 = 8,
	SWAP_F80 = 10
} SwapSize;

typedef union {
	void* p;
	u8*   u8;
	u16*  u16;
	u32*  u32;
	u64*  u64;
	s8*   s8;
	s16*  s16;
	s32*  s32;
	s64*  s64;
	f32*  f32;
	f64*  f64;
} PointerCast;

typedef struct Node {
	struct Node* prev;
	struct Node* next;
} Node;

typedef enum {
	MEM_CRC32       = 1 << 17,
	MEM_ALIGN       = 1 << 18,
	MEM_REALLOC     = 1 << 19,
	MEM_FILENAME    = 1 << 20,
	MEM_THROW_ERROR = 1 << 21,
	
	MEM_CLEAR       = 1 << 30,
	MEM_END         = 1 << 31,
} MemInit;

typedef struct Sym Sym;

enum SymSize {
	SYM_16,
	SYM_32,
	SYM_64
};

enum SymOFlag {
	SYM_O_NONE,
	SYM_O_MERGE_IDENTICAL = 1 << 0, // merge with other if similar
};

typedef struct Memfile {
	union {
		void*       data;
		PointerCast cast;
		char*       str;
	};
	u32 memSize;
	u32 size;
	u32 seekPoint;
	struct {
		time_t age;
		char*  name;
		u32    crc32;
	} info;
	struct {
		u32  align;
		bool realloc    : 1;
		bool getCrc     : 1;
		bool throwError : 1;
		u64  initKey;
	} param;
	
	struct {
		const char* name;
		const char* type;
		Sym* head;
		Sym* refHead;
		u8   align;
		
		struct Memfile** child;
		int numChild;
	} sym;
} Memfile;

typedef enum {
	LIST_FILES    = 0x0,
	LIST_FOLDERS  = 0x1,
	
	LIST_RELATIVE = (1) << 4,
	LIST_NO_DOT   = (1) << 5,
} ListFlag;

typedef enum {
	// Files and folders
	FILTER_SEARCH = 0,
	FILTER_START,
	FILTER_END,
	FILTER_WORD,
	
	// Files
	CONTAIN_SEARCH,
	CONTAIN_START,
	CONTAIN_END,
	CONTAIN_WORD,
} ListFilter;

typedef struct FilterNode {
	struct FilterNode* prev;
	struct FilterNode* next;
	ListFilter type;
	char*      txt;
} FilterNode;

typedef struct List {
	char**      item;
	u32         num;
	FilterNode* filterNode;
	u64         initKey;
	struct {
		s32 alnum;
		int i;
	} p;
} List;

typedef struct {
	u32 size;
	u8  data[];
} DataFile;

typedef struct {
	s32 year;
	s32 month;
	s32 day;
	s32 hour;
	s32 minute;
	s32 second;
} date_t;

enum {
	SORT_NO        = 0,
	SORT_NUMERICAL = 1,
	IS_FILE        = 0,
	IS_DIR         = 1,
	RELATIVE       = 0,
	ABSOLUTE       = 1
};

typedef struct strnode_t {
	struct strnode_t* next;
	char* txt;
} strnode_t;

typedef struct ptr_node_t {
	struct ptr_node_t* next;
	void* ptr;
} ptr_node_t;

enum RegexFlag {
	REGFLAG_START     = 1 << 24,
	REGFLAG_END       = 1 << 25,
	REGFLAG_COPY      = 1 << 26,
	REGFLAG_MATCH_NUM = 1 << 27,
	
	REGFLAG_FLAGMASK  = 0xFF000000,
	REGFLAG_NUMMASK   = 0x00FFFFFF,
};

typedef enum {
	ENV_USERNAME,
	ENV_APPDATA,
	ENV_HOME,
	ENV_TEMP,
} env_index_t;

typedef enum Type {
	TYPE_NONE,
	TYPE_INT,
	TYPE_FLOAT,
	TYPE_BOOL,
	TYPE_STRING,
} Type;

typedef struct Toml {
	struct {
		bool changed : 1;
		bool silence : 1;
		bool success : 1;
		bool write   : 1;
	};
	union {
#ifdef EXT_TOML_C
		toml_table_t* root;
#endif
		void* data;
	};
} Toml;

typedef struct Arli {
	size_t elemSize;
	size_t num;
	size_t max;
	u8*    begin;
	u8*    end;
	u8*    rbegin;
	u8*    rend;
	
	const char* (*elemName)(struct Arli*, size_t);
	u8*   copybuf;
	int64_t cur;
	
	char type[64];
} Arli;

typedef struct Kval {
	char** key;
	char** val;
	
	size_t vsize;
	
	size_t num;
	size_t capacity;
} Kval;

typedef struct {
	f64  start;
	f64  sec;
	bool ongoing;
} Timer;

Timer TimerSet(f32 seconds);
f32 TimerElapsed(Timer* this);
bool TimerDecr(Timer* this);

#endif

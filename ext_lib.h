#ifndef EXT_LIB_H
#define EXT_LIB_H

#ifndef NDEBUG
#define NDEBUG
#endif

#define THIS_EXTLIB_VERSION 220

/**
 * Do not get impression that this library is clang compatible.
 * Far from it.
 *
 * I just use clang IDE, that's all.
 */

#ifdef __clang__
#ifdef _WIN32
#undef _WIN32
#endif
#endif

#ifndef EXTLIB_PERMISSIVE
#ifndef EXTLIB
#error EXTLIB not defined
#else
#if EXTLIB > THIS_EXTLIB_VERSION
#error ExtLib copy is older than the project its used with
#endif
#endif
#endif

#define _GNU_SOURCE
#ifndef __CRT__NO_INLINE
#define __CRT__NO_INLINE
#endif

#ifdef __clang__
#define free __hidden_free
#endif

#include "ext_type.h"
#include "ext_macros.h"
#include "ext_math.h"
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>

#undef free

void profilog(const char* msg);
void profilogdiv(const char* msg, f32 div);

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

void* qxf(const void* ptr);
void* FreeList_Que(void* ptr);
void* FreeList_QueCall(void* callback, void* ptr);
void FreeList_Free(void);

void bswap(void* src, int size);
void* alloc(int size);

Hash HashMem(const void* data, size_t size);
bool HashCmp(Hash* a, Hash* b);

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

void _log_print();

#ifdef __clang__
void _assert(bool);
void _log(const char* fmt, ...);
#else
void __log__(const char* func, u32 line, const char* txt, ...);
#define _log(...) __log__(__FUNCTION__, __LINE__, __VA_ARGS__)

#define _assert(v) do {                                         \
        if (!(v)) {                                             \
            _log("_assert( "PRNT_YELW "%s"PRNT_GRAY " )", # v); \
            _log_print();                                       \
            exit(1);                                            \
        }                                                       \
} while (0)

#endif

char* Ini_Var(const char* str, const char* name);
char* Ini_GetVar(const char* str, const char* name);
void Ini_ParseIncludes(Memfile* mem);
s32 Ini_GetError(void);
void Ini_GetArr(Memfile* mem, const char* variable, List* list);
s32 Ini_GetBool(Memfile* mem, const char* variable);
s32 Ini_GetOpt(Memfile* mem, const char* variable, char* strList[]);
s32 Ini_GetInt(Memfile* mem, const char* variable);
char* Ini_GetStr(Memfile* mem, const char* variable);
f32 Ini_GetFloat(Memfile* mem, const char* variable);
void Ini_GotoTab(const char* section);
void Ini_ListVars(Memfile* mem, List* list, const char* section);
void Ini_ListTabs(Memfile* cfg, List* list);
s32 Ini_RepVar(Memfile* mem, const char* variable, const char* fmt, ...);
void Ini_WriteComment(Memfile* mem, const char* comment);
void Ini_WriteArr(Memfile* mem, const char* variable, List* list, bool quote, const char* comment);
void Ini_WriteInt(Memfile* mem, const char* variable, s64 integer, const char* comment);
void Ini_WriteHex(Memfile* mem, const char* variable, s64 integer, const char* comment);
void Ini_WriteStr(Memfile* mem, const char* variable, const char* str, bool quote, const char* comment);
void Ini_WriteFloat(Memfile* mem, const char* variable, f64 flo, const char* comment);
void Ini_WriteBool(Memfile* mem, const char* variable, s32 val, const char* comment);
void Ini_WriteTab(Memfile* mem, const char* variable, const char* comment);
#define Ini_Fmt(mem, ...) Memfile_Fmt(mem, __VA_ARGS__)
#define NO_COMMENT NULL
#define QUOTES     1
#define NO_QUOTES  0

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

char* regex(const char* str, const char* pattern, enum RegexFlag flag);

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

void IO_SetLevel(enum IOLevel lvl);
void IO_KillBuf(FILE* output);
void IO_FixWin32(void);

void info_title(const char* toolname, const char* fmt, ...);
void warn(const char* fmt, ...);
void warn_align(const char* info, const char* fmt, ...);
void errr(const char* fmt, ...);
void errr_align(const char* info, const char* fmt, ...);
void info(const char* fmt, ...);
void info_align(const char* info, const char* fmt, ...);
void info_progff(const char* info, u32 a, u32 b);
void info_prog(const char* info, u32 a, u32 b);
void info_getc(const char* txt);
void info_volatile(const char* fmt, ...);
void info_hex(const char* txt, const void* data, u32 size, u32 dispOffset);
void info_bit(const char* txt, const void* data, u32 size, u32 dispOffset);
void info_nl(void);

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

List List_New(void);
void List_SetFilters(List* list, u32 filterNum, ...);
void List_FreeFilters(List* this);
void List_Walk(List* this, const char* path, s32 depth, ListFlag flags);
char* List_Concat(List* this, const char* separator);
void List_Tokenize2(List* list, const char* str, const char separator);
void List_Print(List* target);
time_t List_StatMax(List* list);
time_t List_StatMin(List* list);
s32 List_SortSlot(List* this, bool checkOverlaps);
void List_FreeItems(List* this);
void List_Free(List* this);
void List_Alloc(List* this, u32 num);
void List_Add(List* this, const char* item);
void List_Combine(List* out, List* a, List* b);
void List_Tokenize(List* this, const char* s, char r);
void List_Sort(List* this);

#ifndef __clang__
#define List_SetFilters(list, ...) List_SetFilters(list, NARGS(__VA_ARGS__), __VA_ARGS__)
#endif

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

Toml Toml_New();
void Toml_SetVar(Toml* this, const char* item, const char* fmt, ...);
void Toml_SetTab(Toml* this, const char* item, ...);
bool Toml_RmVar(Toml* this, const char* fmt, ...);
bool Toml_RmArr(Toml* this, const char* fmt, ...);
bool Toml_RmTab(Toml* this, const char* fmt, ...);
void Toml_Load(Toml* this, const char* file);
void Toml_Free(Toml* this);
void Toml_Print(Toml* this, void* d, void (*PRINT)(void*, const char*, ...));
void Toml_SaveMem(Toml* this, Memfile* mem);
void Toml_Save(Toml* this, const char* file);
int Toml_GetInt(Toml* this, const char* item, ...);
f32 Toml_GetFloat(Toml* this, const char* item, ...);
bool Toml_GetBool(Toml* this, const char* item, ...);
char* Toml_GetStr(Toml* this, const char* item, ...);

char* Toml_Var(Toml* this, const char* item, ...);
int Toml_ArrItemNum(Toml* this, const char* arr, ...);
int Toml_TabItemNum(Toml* this, const char* item, ...);

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

Memfile Memfile_New();
void Memfile_Set(Memfile* this, ...);
void Memfile_Alloc(Memfile* this, size_t size);
void Memfile_Realloc(Memfile* this, size_t size);
void Memfile_Rewind(Memfile* this);
int Memfile_Write(Memfile* this, const void* src, size_t size);
int Memfile_WriteFile(Memfile* this, const char* source);
int Memfile_Insert(Memfile* this, const void* src, size_t size);
int Memfile_Append(Memfile* this, Memfile* src);
void Memfile_Align(Memfile* this, size_t align);
int Memfile_Fmt(Memfile* this, const char* fmt, ...);
int Memfile_Read(Memfile* this, void* dest, size_t size);
void* Memfile_Seek(Memfile* this, size_t seek);
void Memfile_LoadMem(Memfile* this, void* data, size_t size);
int Memfile_LoadBin(Memfile* this, const char* filepath);
int Memfile_LoadStr(Memfile* this, const char* filepath);
int Memfile_SaveBin(Memfile* this, const char* filepath);
int Memfile_SaveStr(Memfile* this, const char* filepath);
void Memfile_Free(Memfile* this);
void Memfile_Null(Memfile* this);
void Memfile_Clear(Memfile* this);

#ifndef __clang__
#define Memfile_Set(this, ...) Memfile_Set(this, __VA_ARGS__, MEM_END)
#endif

#define MEMFILE_SEEK_END 0xDEFEBABE

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

extern vbool gThreadMode;
extern mutex_t gThreadMutex;
extern const char* gParallel_ProgMsg;
void* Parallel_Add(void* function, void* arg);
void Parallel_Exec(u32 max);
void Parallel_SetID(void* __this, int id);
void Parallel_SetDepID(void* __this, int id);

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

void SegmentSet(const u8 id, void* segment);
void* SegmentToVirtual(const u8 id, u32 ptr);
u32 VirtualToSegment(const u8 id, void* ptr);

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

void* x_alloc(size_t size);

char* x_strdup(const char* s);
char* x_strndup(const char* s, size_t n);
void* x_memdup(const void* d, size_t n);
char* x_fmt(const char* fmt, ...);
char* x_rep(const char* s, const char* a, const char* b);
char* x_cpyline(const char* s, size_t i);
char* x_cpyword(const char* s, size_t i);
char* x_path(const char* s);
char* x_pathslot(const char* s, int i);
char* x_basename(const char* s);
char* x_filename(const char* s);
char* x_randstr(size_t size, const char* charset);
char* x_strunq(const char* s);
char* x_enumify(const char* s);
char* x_canitize(const char* s);
char* x_dirrel_f(const char* from, const char* item);
char* x_dirabs_f(const char* from, const char* item);
char* x_dirrel( const char* item);
char* x_dirabs(const char* item);

char* strdup(const char* s);
char* strndup(const char* s, size_t n);
void* memdup(const void* d, size_t n);
char* fmt(const char* fmt, ...);
char* rep(const char* s, const char* a, const char* b);
char* cpyline(const char* s, size_t i);
char* cpyword(const char* s, size_t i);
char* path(const char* s);
char* pathslot(const char* s, int i);
char* basename(const char* s);
char* filename(const char* s);
char* randstr(size_t size, const char* charset);
char* strunq(const char* s);
char* enumify(const char* s);
char* canitize(const char* s);
char* dirrel_f(const char* from, const char* item);
char* dirabs_f(const char* from, const char* item);
char* dirrel( const char* item);
char* dirabs(const char* item);

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

#define calloc(size) calloc(1, size)

#ifdef __clang__
void* free(const void*, ...);

#else

void* __free(const void* data);
#define __impl_for_each_free(a) a = __free(a);
#define free(...)               ({                      \
        VA_ARG_MANIP(__impl_for_each_free, __VA_ARGS__) \
        NULL;                                           \
    })

#endif

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

int shex(const char* string);
int sint(const char* string);
f32 sfloat(const char* string);
int sbool(const char* string);
int vldt_hex(const char* str);
int vldt_int(const char* str);
int vldt_float(const char* str);
int digint(int i);
int valdig(int val, int digid);
int dighex(int i);
int valhex(int val, int digid);

hsl_t Color_hsl(u8 r, u8 g, u8 b);
rgb8_t Color_rgb8(f32 h, f32 s, f32 l);
rgba8_t Color_rgba8(f32 h, f32 s, f32 l);
void Color_Convert2hsl(hsl_t* dest, rgb8_t* src);
void Color_Convert2rgb(rgb8_t* dest, hsl_t* src);

int Note_Index(const char* note);
const char* Note_Name(int note);

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

f32 randf();
f32 Math_SmoothStepToF(f32* pValue, f32 target, f32 fraction, f32 step, f32 minStep);
f32 Math_Spline(f32 k, f32 xm1, f32 x0, f32 x1, f32 x2);
void Math_ApproachF(f32* pValue, f32 target, f32 fraction, f32 step);
void Math_ApproachS(s16* pValue, s16 target, s16 scale, s16 step);

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

void* memmem(const void* hay, size_t haylen, const void* nee, size_t neelen);
void* memmem_align(u32 val, const void* haystack, size_t haystacklen, const void* needle, size_t needlelen);
char* stristr(const char* haystack, const char* needle);
char* strwstr(const char* hay, const char* nee);
char* strnstr(const char* hay, const char* nee, size_t n);
char* strend(const char* src, const char* ext);
char* striend(const char* src, const char* ext);
char* strstart(const char* src, const char* ext);
char* stristart(const char* src, const char* ext);
int strarg(const char* args[], const char* arg);
char* strracpt(const char* str, const char* c);
char* strlnhead(const char* str, const char* head);
char* strline(const char* str, int line);
char* strword(const char* str, int word);
size_t linelen(const char* str);
size_t wordlen(const char* str);
bool chrspn(int c, const char* accept);
int strnocc(const char* s, size_t n, const char* accept);
int strocc(const char* s, const char* accept);
int strnins(char* dst, const char* src, size_t pos, int n);
int strins(char* dst, const char* src, size_t pos);
int strinsat(char* str, const char* ins);
int strrep(char* src, const char* mtch, const char* rep);
int strnrep(char* src, int len, const char* mtch, const char* rep);
int strwrep(char* src, const char* mtch, const char* rep);
int strwnrep(char* src, int len, const char* mtch, const char* rep);
char* strfext(const char* str);
char* strtoup(char* str);
char* strtolo(char* str);
void strntolo(char* s, int i);
void strntoup(char* s, int i);
void strrem(char* point, int amount);
char* strflipslash(char* t);
char* strfssani(char* t);
int strstrlen(const char* a, const char* b);
char* String_GetSpacedArg(const char** args, int cur);
void strswapext(char* dest, const char* src, const char* ext);
char* strarrcat(const char** list, const char* separator);
size_t strwlen(const wchar* s);
char* strto8(char* dst, const wchar* src);
wchar* strto16(wchar* dst, const char* src);
int linenum(const char* str);
int dirnum(const char* src);
int dir_isabs(const char* item);
int dir_isrel(const char* item);

#ifndef _WIN32
#ifndef __clang__
#define stricmp(a, b)        strcasecmp(a, b)
#define strnicmp(a, b, size) strncasecmp(a, b, size)
#endif
#endif

int stricmp(const char* a, const char* b);
int strnicmp(const char* a, const char* b, size_t size);

bool sys_isdir(const char* path);
time_t sys_stat(const char* item);
const char* sys_app(void);
const char* sys_appdata(void);
time_t sys_time(void);
void sys_sleep(f64 sec);
void sys_mkdir(const char* dir, ...);
const char* sys_workdir(void);
const char* sys_appdir(void);
int sys_mv(const char* input, const char* output);
int sys_rm(const char* item);
int sys_rmdir(const char* item);
void sys_setworkpath(const char* txt);
int sys_touch(const char* file);
int sys_cp(const char* src, const char* dest);
date_t sys_timedate(time_t time);
int sys_getcorenum(void);
size_t sys_statsize(const char* file);
const char* sys_env(env_index_t env);

int sys_exe(const char* cmd);
void sys_exed(const char* cmd);
int sys_exel(const char* cmd, int (*callback)(void*, const char*), void* arg);
void sys_exes_noerr();
int sys_exes_return();
char* sys_exes(const char* cmd);

void fs_mkflag(bool flag);
void fs_set(const char* fmt, ...);
char* fs_item(const char* str, ...);
char* fs_find(const char* str);

bool cli_yesno(void);
void cli_clear(void);
void cli_clearln(int i);
void cli_gotoprevln(void);
const char* cli_gets(void);
char cli_getc();
void cli_hide(void);
void cli_show(void);
void cli_getSize(int* r);
void cli_getPos(int* r);

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

int qsort_numhex(const void* ptrA, const void* ptrB);

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"

static inline void thd_lock(void) {
    pthread_mutex_lock(&gThreadMutex);
}

static inline void thd_unlock(void) {
    
    pthread_mutex_unlock(&gThreadMutex);
}

static inline int thd_create(thread_t* thread, void* func, void* arg) {
    return pthread_create(thread, NULL, (void*)func, (void*)(arg));
}

static inline int thd_join(thread_t* thread) {
    return pthread_join(*thread, NULL);
}

#pragma GCC diagnostic pop

#ifndef __clang__
#define strncat(str, cat, size) strncat(str, cat, (size) - 1)
#define strncpy(str, cpy, size) strncpy(str, cpy, (size) - 1)
#endif

#endif /* EXT_LIB_H */

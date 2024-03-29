#ifndef EXT_MACROS_H
#define EXT_MACROS_H

#define PATH_BUFFER_SIZE 261

#define PRNT_DGRY "\e[90;2m"
#define PRNT_DRED "\e[91;2m"
#define PRNT_GRAY "\e[90m"
#define PRNT_REDD "\e[91m"
#define PRNT_GREN "\e[92m"
#define PRNT_YELW "\e[93m"
#define PRNT_BLUE "\e[94m"
#define PRNT_PRPL "\e[95m"
#define PRNT_CYAN "\e[96m"
#define PRNT_DARK "\e[2m"
#define PRNT_ITLN "\e[3m"
#define PRNT_UNDL "\e[4m"
#define PRNT_BLNK "\e[5m"
#define PRNT_RSET "\e[m"
#define PRNT_NL   "\n"
#define PRNT_RNL  PRNT_RSET PRNT_NL
#define PRNT_TODO "\e[91;2m" "TODO"

#define asm_alias(sym, alias) \
		__asm__ (".equ " #alias ", " #sym)

#define FpsLock(ptime, fps) ({ \
		f64 ctime = sys_ftime(); \
		int r = true; \
		if ((ctime - ptime) < (1.0 / (fps))) { \
			r = false; \
		} else { \
			ptime = ctime; \
		} \
		r; \
	})

#define catprintf(dest, ...) sprintf(dest + strlen(dest), __VA_ARGS__)

#define Node_Add(head, node) do { \
			typeof(node) * __n__ = &head; \
			while (*__n__) __n__ = &(*__n__)->next; \
			*__n__ = node; \
} while (0)

#define Node_Remove(head, node) do { \
			typeof(node) * __n__ = &head; \
			while (*__n__ != node) __n__ = &(*__n__)->next; \
			*__n__ = node->next; \
} while (0)

#define Node_Kill(head, node) do { \
			typeof(node) killNode = node; \
			Node_Remove(head, node); \
			delete(killNode); \
} while (0)

#define Swap(a, b) do { \
			var_t y = a; \
			a = b; \
			b = y; \
} while (0)

void* membswap(void* v, size_t size);
// Checks endianess with tst & tstP
#define ReadBE(in) ({ \
		var_t out = in; \
		switch (sizeof(out)) { \
			case 2: out = __builtin_bswap16(out); break; \
			case 4: out = __builtin_bswap32(out); break; \
			case 8: out = __builtin_bswap64(out); break; \
			default: membswap(&out, sizeof(out)); break; \
		} \
		out; \
	})

#define WriteBE(dest, set) { \
			typeof(dest) get = set; \
			dest = ReadBE(get); \
}

#define SwapBE(in) WriteBE(in, in)

#define Decr(x) (x -= (x > 0) ? 1 : 0)
#define Incr(x) (x += (x < 0) ? 1 : 0)

#define Intersect(a, aend, b, bend) ((Max(a, b) < Min(aend, bend)))
#define IsBetween(a, x, y)          ((a) >= (x) && (a) <= (y))

#define absmax(a, b)         (abs(a) > abs(b) ? (a) : (b))
#define absmin(a, b)         (abs(a) <= abs(b) ? (a) : (b))
#define abs(val)             ((val) < 0 ? -(val) : (val))
#define clamp(val, min, max) ((val) < (min) ? (min) : (val) > (max) ? (max) : (val))
#define clamp_min(val, min)  ((val) < (min) ? (min) : (val))
#define clamp_max(val, max)  ((val) > (max) ? (max) : (val))

#define Max(a, b) ((a) > (b) ? (a) : (b))
#define Min(a, b) ((a) < (b) ? (a) : (b))

#define clamp_s8(val)  (s8)clamp(((f32)val), (-__INT8_MAX__ - 1), __INT8_MAX__)
#define clamp_s16(val) (s16)clamp(((f32)val), (-__INT16_MAX__ - 1), __INT16_MAX__)
#define clamp_s32(val) (s32)clamp(((f32)val), (-__INT32_MAX__ - 1), (f32)__INT32_MAX__)

#define ArrCount(arr) (u32)(sizeof(arr) / sizeof(arr[0]))

#define BinToMb(x)           ((f32)(x) / (f32)0x100000)
#define BinToKb(x)           ((f32)(x) / (f32)0x400)
#define MbToBin(x)           (int)(0x100000 * (x))
#define KbToBin(x)           (int)(0x400 * (x))
#define alignvar(val, align) ((((val) % (align)) != 0) ? (val) + (align) - ((val) % (align)) : (val))

#define VA1(NAME, ...)                                 NAME
#define VA2(_1, NAME, ...)                             NAME
#define VA3(_1, _2, NAME, ...)                         NAME
#define VA4(_1, _2, _3, NAME, ...)                     NAME
#define VA5(_1, _2, _3, _4, NAME, ...)                 NAME
#define VA6(_1, _2, _3, _4, _5, NAME, ...)             NAME
#define VA7(_1, _2, _3, _4, _5, _6, NAME, ...)         NAME
#define VA8(_1, _2, _3, _4, _5, _6, _7, NAME, ...)     NAME
#define VA9(_1, _2, _3, _4, _5, _6, _7, _8, NAME, ...) NAME

#define NARGS_SEQ( \
			_1, _2, _3, _4, _5, _6, _7, _8, \
			_9, _10, _11, _12, _13, _14, _15, _16, \
			_17, _18, _19, _20, _21, _22, _23, _24, \
			_25, _26, _27, _28, _29, _30, _31, _32, \
			_33, _34, _35, _36, _37, _38, _39, _40, \
			_41, _42, _43, _44, _45, _46, _47, _48, \
			_49, _50, _51, _52, _53, _54, _55, _56, \
			_57, _58, _59, _60, _61, _62, _63, _64, \
			_65, _66, _67, _68, _69, _70, _71, _72, \
			_73, _74, _75, _76, _77, _78, _79, _80, \
			_81, _82, _83, _84, _85, _86, _87, _88, \
			_89, _90, _91, _92, _93, _94, _95, _96, \
			_97, _98, _99, _100, _101, _102, _103, _104, \
			_105, _106, _107, _108, _109, _110, _111, _112, \
			_113, _114, _115, _116, _117, _118, _119, _120, \
			_121, _122, _123, _124, _125, _126, _127, _128, N, ...) N
#define NARGS(...) \
		NARGS_SEQ( \
			__VA_ARGS__ \
			, 128, 127, 126, 125, 124, 123, 122, 121 \
			, 120, 119, 118, 117, 116, 115, 114, 113 \
			, 112, 111, 110, 109, 108, 107, 106, 105 \
			, 104, 103, 102, 101, 100, 99, 98, 97 \
			, 96, 95, 94, 93, 92, 91, 90, 89 \
			, 88, 87, 86, 85, 84, 83, 82, 81 \
			, 80, 79, 78, 77, 76, 75, 74, 73 \
			, 72, 71, 70, 69, 68, 67, 66, 65 \
			, 64, 63, 62, 61, 60, 59, 58, 57 \
			, 56, 55, 54, 53, 52, 51, 50, 49 \
			, 48, 47, 46, 45, 44, 43, 42, 41 \
			, 40, 39, 38, 37, 36, 35, 34, 33 \
			, 32, 31, 30, 29, 28, 27, 26, 25 \
			, 24, 23, 22, 21, 20, 19, 18, 17 \
			, 16, 15, 14, 13, 12, 11, 10, 9 \
			, 8, 7, 6, 5, 4, 3, 2, 1 \
		)

#define SEG_FAULT ((u32*)0)[0] = 0

#if defined(_WIN32) && defined(UNICODE)
#define uni_main(count, args) \
		__x_main(int count, const char** args); \
		int wmain(int count, const wchar * *args) { \
			char** nargv = qxf(calloc(sizeof(char*) * (count + 1))); \
			for (int i = 0; i < count; i++) { \
				nargv[i] = qxf(calloc(strwlen(args[i]))); \
				strto8(nargv[i], args[i]); \
			} \
			osLog("run " PRNT_YELW "main"); \
			return __x_main(count, (void*)nargv); \
		} \
		int __x_main(int count, const char** args)
#else
#define uni_main(count, args) main(int count, const char** args)
#endif

#define UnfoldRGB(color)  (color).r, (color).g, (color).b
#define UnfoldRGBA(color) (color).r, (color).g, (color).b, (color).a
#define UnfoldHSL(color)  (color).h, (color).s, (color).l

#define stalloc(type)          ({ void* data = alloca(sizeof(type)); memset(data, 0, sizeof(type)); data; })
#define new(type)              ({ osLog("" PRNT_GREN "new" PRNT_RSET "( %s )", #type); calloc(sizeof(type)); })
#define renew(addr, type)      addr = realloc(addr, sizeof(type))
#define x_new(type)            x_alloc(sizeof(type))
#define EXT_INFO_TITLE(xtitle) PRNT_YELW xtitle PRNT_RNL
#define EXT_INFO(A, indent, B) PRNT_GRAY "[>]: " PRNT_RSET A "\r\033[" #indent "C" PRNT_GRAY "# " B PRNT_NL

#define foreach(val, arr) for (int val = 0; val < ArrCount(arr); val++)
// #define for (int val = 0; val < list.num; val++) for (int val = 0; val < (list).num; val++)
#define fornode(val, head) for (typeof(head) val = head; val != NULL; val = val->next)
#define forstr(val, str)   for (int val = 0; val < strlen(str); val++)
#define forline(val, str)  for (const char* val = str; val; val = strline(val, 1))
#define forlist(val, list) \
		for (char** __p ## val = (list).item; __p ## val && __p ## val < (list).item + (list).num; __p ## val++) \
		for (char* val = *__p ## val; val; val = NULL)

#define indexof(head, elem, type) \
		((int64_t)((u8*)(elem) - (u8*)(head)) / sizeof(type))

#define arrmove_r(arr, start, count) do { \
			var_t v = (arr)[(start) + (count) - 1]; \
			for (int I = (count) + (start) - 1; I > (start); I--) \
			(arr)[I] = (arr)[I - 1]; \
			(arr)[(start)] = v; \
} while (0)

#define arrmove_l(arr, start, count) do { \
			var_t v = (arr)[(start)]; \
			for (int I = (start); I < (count) + (start) - 1; I++) \
			(arr)[I] = (arr)[I + 1]; \
			(arr)[(count) + (start) - 1] = v; \
} while (0)

#define arrzero(arr) memset((arr), 0, sizeof((arr)))
#define memzero(ptr) memset((ptr), 0, sizeof(*(ptr)))

/**
 * These are only to satisfy clang IDE. These won't work
 * as expected if compiled with clang.
 */
#ifdef __clang__
#define nested(type, name, args) \
		type (^ name) args = NULL; \
		name = ^ type args
#define nested_var(v)  typeof(v) v = (typeof(v)) 0;
#define nested_stru(v) typeof(v) v = {}

#else
#define nested(type, name, args) \
		type name args
#define nested_var(v)  (void)0
#define nested_stru(v) (void)0
#endif

#define FOPEN(file, mode) ({ \
		FILE* f = fopen(file, mode); \
		if (f == NULL) { \
			warn("" PRNT_YELW "%s" PRNT_GRAY "();", __FUNCTION__); \
			errr("fopen error: [%s]", file); \
		} \
		f; \
	})

#define __impl_paste(a, b) a ## b
#define __impl_ctassert(predicate, line) \
		typedef char (__impl_paste (ctassert_line_, line))[2 * !!(predicate) - 1]
#define ctassert(predicate) __impl_ctassert(predicate, __LINE__)

#define PP_NARG(...)  PP_NARG_(__VA_ARGS__, PP_RSEQ_N())
#define PP_NARG_(...) PP_ARG_N(__VA_ARGS__)

#define PP_ARG_N( \
			_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
			_11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
			_21, _22, _23, _24, _25, _26, _27, _28, _29, _30, \
			_31, _32, _33, _34, _35, _36, _37, _38, _39, _40, \
			_41, _42, _43, _44, _45, _46, _47, _48, _49, _50, \
			_51, _52, _53, _54, _55, _56, _57, _58, _59, _60, \
			_61, _62, _63, N, ...) N

#define PP_RSEQ_N() \
		63, 62, 61, 60, \
		59, 58, 57, 56, 55, 54, 53, 52, 51, 50, \
		49, 48, 47, 46, 45, 44, 43, 42, 41, 40, \
		39, 38, 37, 36, 35, 34, 33, 32, 31, 30, \
		29, 28, 27, 26, 25, 24, 23, 22, 21, 20, \
		19, 18, 17, 16, 15, 14, 13, 12, 11, 10, \
		9, 8, 7, 6, 5, 4, 3, 2, 1, 0

#define __JOIN_IMPL(a, b)                                                a ## b
#define PP_JOIN(a, b)                                                    __JOIN_IMPL(a, b)
#define __PP_APPLY1(macro, a)                                            macro(a)
#define __PP_APPLY2(macro, a, b)                                         macro(a) macro(b)
#define __PP_APPLY3(macro, a, b, c)                                      macro(a) macro(b) macro(c)
#define __PP_APPLY4(macro, a, b, c, d)                                   macro(a) macro(b) macro(c) macro(d)
#define __PP_APPLY5(macro, a, b, c, d, e)                                macro(a) macro(b) macro(c) macro(d) macro(e)
#define __PP_APPLY6(macro, a, b, c, d, e, f)                             macro(a) macro(b) macro(c) macro(d) macro(e) macro(f)
#define __PP_APPLY7(macro, a, b, c, d, e, f, g)                          macro(a) macro(b) macro(c) macro(d) macro(e) macro(f) macro(g)
#define __PP_APPLY8(macro, a, b, c, d, e, f, g, h)                       macro(a) macro(b) macro(c) macro(d) macro(e) macro(f) macro(g) macro(h)
#define __PP_APPLY9(macro, a, b, c, d, e, f, g, h, i)                    macro(a) macro(b) macro(c) macro(d) macro(e) macro(f) macro(g) macro(h) macro(i)
#define __PP_APPLY10(macro, a, b, c, d, e, f, g, h, i, j)                macro(a) macro(b) macro(c) macro(d) macro(e) macro(f) macro(g) macro(h) macro(i) macro(j)
#define __PP_APPLY11(macro, a, b, c, d, e, f, g, h, i, j, k)             macro(a) macro(b) macro(c) macro(d) macro(e) macro(f) macro(g) macro(h) macro(i) macro(j) macro(k)
#define __PP_APPLY12(macro, a, b, c, d, e, f, g, h, i, j, k, l)          macro(a) macro(b) macro(c) macro(d) macro(e) macro(f) macro(g) macro(h) macro(i) macro(j) macro(k) macro(l)
#define __PP_APPLY13(macro, a, b, c, d, e, f, g, h, i, j, k, l, m)       macro(a) macro(b) macro(c) macro(d) macro(e) macro(f) macro(g) macro(h) macro(i) macro(j) macro(k) macro(l) macro(m)
#define __PP_APPLY14(macro, a, b, c, d, e, f, g, h, i, j, k, l, m, n)    macro(a) macro(b) macro(c) macro(d) macro(e) macro(f) macro(g) macro(h) macro(i) macro(j) macro(k) macro(l) macro(m) macro(n)
#define __PP_APPLY15(macro, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o) macro(a) macro(b) macro(c) macro(d) macro(e) macro(f) macro(g) macro(h) macro(i) macro(j) macro(k) macro(l) macro(m) macro(n) macro(o)
#define __PP_APPLY_(macro, M, ...)                                       M(macro, __VA_ARGS__)
#define PP_APPLY(macro, ...)                                             __PP_APPLY_(macro, PP_JOIN(__PP_APPLY, PP_NARG(__VA_ARGS__)), __VA_ARGS__)
#define VA_ARG_MANIP(macro, ...)                                         PP_APPLY(macro, __VA_ARGS__)

#define pval8(x)  ({ u8* v = x_alloc(1); *v = x; v; })
#define pval16(x) ({ u16* v = x_alloc(2); *v = x; v; })
#define pval32(x) ({ u32* v = x_alloc(4); *v = x; v; })

#endif

#define EXT_TOML_C

#include "xtoml/x0.h"
#include "xtoml/x0impl.h"
#include <ext_lib.h>

enum Remove {
	RM_NONE,
	RM_VAR,
	RM_ARR,
	RM_TAB
};

typedef struct {
	toml_table_t* tbl;
	toml_array_t* arr;
	toml_table_t* par;
	const char*   item;
	s32 idx;
} TravelResult;

#define BUFFER_SIZE 1024

Toml Toml_New() {
	Toml this = {};
	
	return this;
}

static const char* Toml_GetPathStr(const char* path, const char* elem, const char* next) {
	return x_fmt("" PRNT_GRAY "[ %s"PRNT_YELW "%s"PRNT_GRAY "%s ]",
			   path ? path : "", elem ? elem : "", next ? next : "");
}

static TravelResult Toml_Travel(Toml* this, const char* field, toml_table_t* tbl, char* path) {
	const char* elem = x_strcdup(field, ".");
	const char* next = elem ? &field[strlen(elem)] : NULL;
	TravelResult travel = {};
	
	osLog("elem: " PRNT_REDD "%s" PRNT_RSET "\n\tnext: " PRNT_YELW "%s", elem, next);
	
	if (!tbl) return travel;
	
	strcat(path, elem);
	
	nested(void, NewTable, (toml_table_t * tbl, const char* key)) {
		osLog("NewTable: %s", key);
		
		tbl->tab = realloc(tbl->tab, sizeof(void*) * ++tbl->ntab);
		osAssert(tbl->tab != NULL);
		tbl->tab[tbl->ntab - 1] = new(toml_table_t);
		osAssert(tbl->tab[tbl->ntab - 1] != NULL);
		tbl->tab[tbl->ntab - 1]->key = strdup(key);
		osAssert(tbl->tab[tbl->ntab - 1]->key != NULL);
	};
	
	nested(void, NewTblArray, (toml_table_t * tbl, const char* key)) {
		osLog("NewTableArray: %s", key);
		
		tbl->arr = realloc(tbl->arr, sizeof(void*) * ++tbl->narr);
		tbl->arr[tbl->narr - 1] = new(toml_array_t);
		tbl->arr[tbl->narr - 1]->key = strdup(key);
		tbl->arr[tbl->narr - 1]->kind = 't';
		tbl->arr[tbl->narr - 1]->type = 'm';
	};
	
	nested(void, NewTblArrayIndex, (toml_array_t * arr, const char* key, s32 idx)) {
		idx += 1;
		arr->item = realloc(arr->item, sizeof(toml_arritem_t) * idx);
		
		for (var_t i = arr->nitem; i < idx; i++) {
			osLog("NewTableArrayIdx: %s [%d]", key, i);
			arr->item[i] = (toml_arritem_t) { .tab = new(toml_table_t) };
		}
		
		arr->nitem = idx;
	};
	
	nested(void, NewArray, (toml_table_t * tbl, const char* key)) {
		osLog("NewArray: %s", key);
		tbl->arr = realloc(tbl->arr, sizeof(void*) * ++tbl->narr);
		tbl->arr[tbl->narr - 1] = new(toml_array_t);
		tbl->arr[tbl->narr - 1]->key = strdup(key);
		tbl->arr[tbl->narr - 1]->kind = 'a';
		tbl->arr[tbl->narr - 1]->type = 'm';
	};
	
	nested(void, ExpandArray, (toml_array_t * arr, const char* key, int new_max)) {
		osLog("ExpandArray: %s %d->%d", key, arr->nitem, new_max);
		int num = arr->nitem;
		
		arr->item = realloc(arr->item, sizeof(toml_arritem_t) * new_max);
		for (int n = num; n < new_max; n++) {
			arr->item[n] = (toml_arritem_t) { 0 };
			arr->item[n].arr = new(toml_array_t);
			arr->item[n].arr->key = strdup(key);
			arr->item[n].arr->kind = 'a';
			arr->item[n].arr->type = 'm';
		}
		arr->nitem = new_max;
	};
	
	nested(TravelResult, Error, (const char* msg)) {
		TravelResult null = {};
		
		if (!this->silence) {
			warn("" PRNT_REDD "%s" PRNT_RSET " does not exist!", msg);
			errr("%s", Toml_GetPathStr(path, elem, next));
		}
		this->success = false;
		
		return null;
	};
	
	if (!next || *next == '\0') {
		if (strend(elem, "]")) {
			int cspn = strcspn(elem, "[");
			char* key = x_strndup(elem, cspn);
			char* a = x_strdup(elem + cspn);
			int num = strocc(a, "[");
			int* idx = x_alloc(sizeof(int[num]));
			
			for (int i = 0; i < num; i++) {
				while (*a && *a != '[') a++;
				if (!*a) Error("WAT!");
				a++;
				
				if (isalnum(*a))
					idx[i] = sint(a);
				else idx[i] = -1;
			}
			
			toml_array_t* arr = toml_array_in(tbl, key);
			
			if (!arr) {
				if (this->write) {
					NewArray(tbl, key);
					arr = toml_array_in(tbl, key);
				}
				
				if (!arr)
					return Error("Array key");
			}
			
			char* dbg = x_alloc(8 * num);
			
			for (int i = 0; i < num - 1; i++) {
				strcat(dbg, x_fmt("[%d]", idx[i]));
				osLog("%s", dbg);
				
				if (this->write) {
					if (!toml_array_at(arr, idx[i])) {
						ExpandArray(arr, key, idx[i] + 1);
					}
				}
				
				arr = toml_array_at(arr, idx[i]);
				
				if (!arr)
					return Error("Sub Array");
			}
			
			if (this->write)
				arr->kind = 'v';
			
			return (TravelResult) {
					   .arr = arr,
					   .idx = idx[num - 1]
			};
		}
		
		travel =  (TravelResult) {
			.tbl = tbl,
			.item = elem,
		};
		
		if (!travel.tbl)
			return Error("Variable");
		
		return travel;
	} else next++;
	
	strcat(path, ".");
	toml_table_t* new_tbl;
	
	if (strend(elem, "]")) {
		s32 idx = sint(strstr(elem, "[") + 1);
		strstr(elem, "[")[0] = '\0';
		toml_array_t* arr = toml_array_in(tbl, elem);
		
		if (!arr) {
			if (this->write) {
				NewTblArray(tbl, elem);
				arr = toml_array_in(tbl, elem);
			}
			
			if (!arr)
				return Error("Table Array");
		}
		
		new_tbl = toml_table_at(arr, idx);
		
		if (!new_tbl) {
			if (this->write) {
				NewTblArrayIndex(arr, elem, idx);
				
				new_tbl = toml_table_at(arr, idx);
			}
			
			if (!new_tbl)
				return Error("Table Array Index");
		}
		
		return Toml_Travel(this, next, new_tbl, path);
	}
	
	new_tbl = toml_table_in(tbl, elem);
	
	if (!new_tbl) {
		if (this->write) {
			NewTable(tbl, elem);
			new_tbl = toml_table_in(tbl, elem);
		}
		
		if (!new_tbl)
			return Error("Table");
	}
	
	return Toml_Travel(this, next, new_tbl, path);
}

static toml_datum_t Toml_GetValue(Toml* this, const char* item, enum Type type) {
	this->success = true;
	toml_datum_t (*getVar[])(const toml_table_t*, const char*) = {
		[TYPE_INT] = toml_int_in,
		[TYPE_FLOAT] = toml_double_in,
		[TYPE_BOOL] = toml_bool_in,
		[TYPE_STRING] = toml_string_in,
	};
	toml_datum_t (*getArr[])(const toml_array_t*, int) = {
		[TYPE_INT] = toml_int_at,
		[TYPE_FLOAT] = toml_double_at,
		[TYPE_BOOL] = toml_bool_at,
		[TYPE_STRING] = toml_string_at,
	};
	char path[BUFFER_SIZE] = {};
	TravelResult t = Toml_Travel(this, item, this->root, path);
	
	if (t.tbl)
		return getVar[type](t.tbl, t.item);
	else if (t.arr)
		return getArr[type](t.arr, t.idx);
	
	return (toml_datum_t) {};
}

static void Toml_SetVarImpl(Toml* this, const char* item, const char* value) {
	char path[BUFFER_SIZE] = {};
	
	if (this->root == NULL)
		this->root = new(toml_table_t);
	
	this->write = true;
	TravelResult t = Toml_Travel(this, item, this->root, path);
	this->write = false;
	
	if (t.tbl) {
		toml_table_t* tbl = t.tbl;
		
		osLog("SetValue(Tbl) (%08X): %s = %s", t.tbl, item, value);
		
		if (!toml_key_exists(t.tbl, t.item)) {
			tbl->kval = realloc(tbl->kval, sizeof(void*) * ++tbl->nkval);
			tbl->kval[tbl->nkval - 1] = calloc(sizeof(toml_keyval_t));
			
			tbl->kval[tbl->nkval - 1]->key = strdup(t.item);
			this->changed = true;
		}
		
		for (var_t i = 0; i < tbl->nkval; i++) {
			if (!strcmp(tbl->kval[i]->key, t.item)) {
				if (!tbl->kval[i]->val || strcmp(tbl->kval[i]->val, value)) {
					delete(tbl->kval[i]->val);
					tbl->kval[i]->val = strdup(value);
					this->changed = true;
				}
			}
		}
	}
	
	if (t.arr) {
		toml_array_t* arr = t.arr;
		
		osLog("SetValue(Arr[%d]): %s = %s", t.idx, item, value);
		osLog("ArrKey: %s", arr->key);
		
		while (arr->nitem <= t.idx) {
			renew(arr->item, toml_arritem_t[arr->nitem + 1]);
			arr->item[arr->nitem] = (toml_arritem_t) {};
			arr->item[arr->nitem].val = strdup("0");
			arr->item[arr->nitem].valtype = valtype("0");
			arr->item[arr->nitem].tab = 0;
			arr->item[arr->nitem].arr = 0;
			this->changed = true;
			osLog("new arr: %d", arr->nitem);
			arr->nitem++;
		}
		osAssert(arr->nitem > t.idx);
		
		if (!arr->item[t.idx].val || strcmp(arr->item[t.idx].val, value)) {
			delete(arr->item[t.idx].val);
			arr->item[t.idx].val = strdup(value);
			arr->item[t.idx].valtype = valtype(value);
			arr->item[t.idx].tab = 0;
			arr->item[t.idx].arr = 0;
			this->changed = true;
			osLog("arr value: %s", value);
		}
		
		osLog("nitem: %d", arr->nitem);
	}
}

void Toml_SetVar(Toml* this, const char* item, const char* fmt, ...) {
	char buffer[BUFFER_SIZE];
	va_list va;
	
	va_start(va, fmt);
	xl_vsnprintf(buffer, sizeof(buffer), fmt, va);
	va_end(va);
	
	Toml_SetVarImpl(this, item, buffer);
}

void Toml_SetTab(Toml* this, const char* item, ...) {
	char path[BUFFER_SIZE] = {};
	char table[BUFFER_SIZE];
	va_list va;
	
	va_start(va, item);
	xl_vsnprintf(table, BUFFER_SIZE, item, va);
	strncat(table, ".temp", BUFFER_SIZE);
	va_end(va);
	
	if (this->root == NULL)
		this->root = new(toml_table_t);
	
	this->write = true;
	Toml_Travel(this, table, this->root, path);
	this->write = false;
	this->changed = true;
}

static bool Toml_Remove(Toml* this, enum Remove rem, const char* item, va_list va) {
	char path[BUFFER_SIZE] = {};
	char value[BUFFER_SIZE];
	
	xl_vsnprintf(value, BUFFER_SIZE, item, va);
	
	if (this->root == NULL)
		this->root = new(toml_table_t);
	
	switch (rem) {
		case RM_ARR:
			if (!strend(value, "]"))
				strcat(value, "[]");
			break;
			
		case RM_TAB:
			strcat(value, ".t");
			break;
			
		default:
			break;
	}
	
	this->silence = true;
	TravelResult t = Toml_Travel(this, value, this->root, path);
	this->silence = false;
	
	toml_table_t* tbl = t.tbl;
	toml_array_t* arr = t.arr;
	
	#define TOML_REMOVE(TYPE) \
			for (var_t i = 0; i < tbl->n ## TYPE; i++) { \
				if (!strcmp(tbl->TYPE[i]->key, t.item)) { \
					xfree_ ## TYPE(tbl->TYPE[i]); \
					arrmove_l(tbl->TYPE, i, (tbl->n ## TYPE) - i); \
					tbl->n ## TYPE--; \
					if (!tbl->n ## TYPE) { delete(tbl->TYPE); } \
					this->changed = true; \
					return true; \
				} \
			}
	
	if (tbl) {
		switch (rem) {
			case RM_VAR:
				TOML_REMOVE(kval);
				break;
				
			case RM_TAB:
				TOML_REMOVE(arr);
				break;
				
			default:
				break;
		}
	}
	
	if (arr) {
		if (rem == RM_ARR) {
			int i = 0;
			tbl = t.par;
			
			for (; i < tbl->narr; i++)
				if (tbl->arr[i] == arr)
					break;
			
			osLog("%s", arr->key);
			osAssert(i < tbl->narr);
			
			arrmove_l(tbl->arr, i, tbl->narr);
			tbl->narr--;
			
			xfree_arr(arr);
			
			return true;
		}
	}
	
	return false;
}

bool Toml_RmVar(Toml* this, const char* fmt, ...) {
	va_list va;
	
	va_start(va, fmt);
	bool r = Toml_Remove(this, RM_VAR, fmt, va);
	va_end(va);
	return r;
}

bool Toml_RmArr(Toml* this, const char* fmt, ...) {
	va_list va;
	
	va_start(va, fmt);
	bool r = Toml_Remove(this, RM_ARR, fmt, va);
	va_end(va);
	return r;
}

bool Toml_RmTab(Toml* this, const char* fmt, ...) {
	va_list va;
	
	va_start(va, fmt);
	bool r = Toml_Remove(this, RM_TAB, fmt, va);
	va_end(va);
	return r;
}

// # # # # # # # # # # # # # # # # # # # #
// # Base                                #
// # # # # # # # # # # # # # # # # # # # #

bool Toml_Load(Toml* this, const char* file) {
	Memfile mem = Memfile_New();
	char errbuf[200];
	
	this->success = true;
	
	osLog("Parse File: [%s]", file);
	Memfile_LoadStr(&mem, file);
	if (!(this->root = toml_parse(mem.str, errbuf, 200))) {
		if (!this->silence) {
			warn("[Toml Praser Error!]");
			warn("File: %s", file);
			errr("%s", errbuf);
		}
		return EXIT_FAILURE;
	}
	Memfile_Free(&mem);
	
	return EXIT_SUCCESS;
}

bool Toml_LoadMem(Toml* this, const char* str) {
	char errbuf[200];
	
	if (!(this->root = toml_parse((char*)str, errbuf, 200))) {
		if (!this->silence) {
			warn("[Toml Praser Error!]");
			warn("File: LoadMem");
			osLogPrint();
			errr("%s", errbuf);
		}
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}

void Toml_Free(Toml* this) {
	if (!this) return;
	__toml_free(this->root);
	memset(this, 0, sizeof(*this));
}

void Toml_Print(Toml* this, void* d, void (*PRINT)(void*, const char*, ...)) {
	nested(void, Parse, (toml_table_t * tbl, char* path, u32 indent)) {
		osLog("[%s%s]:\n\tnkval: %d\n\tnarr: %d\n\tntab: %d\n", path, tbl->key, tbl->nkval, tbl->narr, tbl->ntab);
		
		nested(char*, Indent, ()) {
			return memset(x_alloc(indent + 1), '\t', indent);
		};
		
		for (var_t i = 0; i < tbl->nkval; i++) {
			toml_keyval_t* kval = tbl->kval[i];
			osLog("kval: %s", kval->key);
			
			PRINT(d, "%s%s = %s\n", Indent(), kval->key, kval->val);
		}
		
		for (var_t i = 0; i < tbl->narr; i++) {
			toml_array_t* arr = tbl->arr[i];
			osLog("arr: %s", arr->key);
			
			if (arr->kind == 't')
				continue;
			
			nested(void, ParseArray, (toml_array_t * arr, bool brk)) {
				nested_var(indent);
				
				int f = 0;
				
				for (var_t j = 0; j < arr->nitem; j++) {
					osLog("%d: %d / %d (%c)", indent, j + 1, arr->nitem, arr->kind);
					
					if (arr->kind == 'v') {
						indent++;
						if (brk) {
							if (f == 0)
								PRINT(d, "%s", Indent());
							else if (f && (f % 4) == 0)
								PRINT(d, ",\n%s", Indent());
							else if (f)
								PRINT(d, ", ");
						} else if (f)
							PRINT(d, ", ");
						
						PRINT(d, "%s", arr->item[j].val);
						f++;
						indent--;
					}
					
					if (arr->kind == 'a') {
						toml_array_t* next = arr->item[j].arr;
						
						indent++;
						
						if (next->kind == 'v') {
							PRINT(d, "%s[ ", Indent(), Indent());
							ParseArray(next, false);
							PRINT(d, " ],\n", Indent());
						} else {
							PRINT(d, "%s[\n", Indent(), Indent());
							ParseArray(next, false);
							PRINT(d, "%s],\n", Indent());
						}
						indent--;
						f++;
					}
				}
				
				if (arr->kind == 'v')
					if (f && brk)
						PRINT(d, ",\n");
				
			};
			
			PRINT(d, "%s%s = [\n", Indent(), arr->key);
			ParseArray(arr, true);
			PRINT(d, "%s]\n", Indent(), arr->key);
		}
		
		for (var_t i = 0; i < tbl->narr; i++) {
			toml_array_t* arr = tbl->arr[i];
			osLog("arr: %s", arr->key);
			
			if (arr->kind != 't')
				continue;
			
			for (var_t j = 0; j < arr->nitem; j++) {
				switch (arr->kind) {
					case 't':
						PRINT(d, "%s[[%s%s]]\n", Indent(), path, arr->key);
						Parse(arr->item[j].tab, x_fmt("%s%s.", path, arr->key), indent + 1);
						
						break;
				}
			}
		}
		
		for (var_t i = 0; i < tbl->ntab; i++) {
			toml_table_t* tab = tbl->tab[i];
			osLog("Table: %s", tab->key);
			
			PRINT(d, "%s[%s%s]\n", Indent(), path, tab->key);
			Parse(tab, x_fmt("%s%s.", path, tab->key), indent + 1);
		}
	};
	
	if (this->root)
		Parse(this->root, "", 0);
	PRINT(d, "\n");
}

void Toml_SaveMem(Toml* this, Memfile* mem) {
	Memfile_Free(mem);
	Toml_Print(this, mem, (void*)Memfile_Fmt);
}

bool Toml_Save(Toml* this, const char* file) {
	Memfile mem = Memfile_New();
	
	this->success = true;
	osLog("save toml: %s", file);
	Toml_Print(this, &mem, (void*)Memfile_Fmt);
	
	mem.param.throwError = false;
	if (Memfile_SaveStr(&mem, file))
		this->success = false;
	Memfile_Free(&mem);
	
	return this->success;
}

// # # # # # # # # # # # # # # # # # # # #
// # GetValue                            #
// # # # # # # # # # # # # # # # # # # # #

int Toml_GetInt(Toml* this, const char* item, ...) {
	char buffer[BUFFER_SIZE];
	va_list va;
	
	va_start(va, item);
	xl_vsnprintf(buffer, BUFFER_SIZE, item, va);
	toml_datum_t t = Toml_GetValue(this, buffer, TYPE_INT);
	va_end(va);
	
	return t.u.i;
}

f32 Toml_GetFloat(Toml* this, const char* item, ...) {
	char buffer[BUFFER_SIZE];
	va_list va;
	
	va_start(va, item);
	xl_vsnprintf(buffer, BUFFER_SIZE, item, va);
	toml_datum_t t = Toml_GetValue(this, buffer, TYPE_FLOAT);
	va_end(va);
	
	return t.u.d;
}

bool Toml_GetBool(Toml* this, const char* item, ...) {
	char buffer[BUFFER_SIZE];
	va_list va;
	
	va_start(va, item);
	xl_vsnprintf(buffer, BUFFER_SIZE, item, va);
	toml_datum_t t = Toml_GetValue(this, buffer, TYPE_BOOL);
	va_end(va);
	
	return t.u.b;
}

char* Toml_GetStr(Toml* this, const char* item, ...) {
	char buffer[BUFFER_SIZE];
	va_list va;
	
	va_start(va, item);
	xl_vsnprintf(buffer, BUFFER_SIZE, item, va);
	toml_datum_t t = Toml_GetValue(this, buffer, TYPE_STRING);
	va_end(va);
	
	if (!t.ok) return NULL;
	
	return t.u.s;
}

char* Toml_Var(Toml* this, const char* item, ...) {
	char buffer[BUFFER_SIZE];
	va_list va;
	
	va_start(va, item);
	xl_vsnprintf(buffer, BUFFER_SIZE, item, va);
	va_end(va);
	
	this->silence = true;
	char path[BUFFER_SIZE] = {};
	TravelResult t = Toml_Travel(this, buffer, this->root, path);
	this->silence = false;
	
	if (t.arr && t.arr->nitem > t.idx)
		return x_strdup(t.arr->item[t.idx].val);
	if (t.tbl && toml_key_exists(t.tbl, t.item))
		for (var_t i = 0; i < t.tbl->nkval; i++)
			if (!strcmp(t.tbl->kval[i]->key, t.item))
				return x_strdup(t.tbl->kval[i]->val);
	return NULL;
}

Type Toml_VarType(Toml* this, const char* item, ...) {
	char buffer[BUFFER_SIZE];
	va_list va;
	
	va_start(va, item);
	xl_vsnprintf(buffer, BUFFER_SIZE, item, va);
	va_end(va);
	
	this->silence = true;
	char path[BUFFER_SIZE] = {};
	TravelResult t = Toml_Travel(this, buffer, this->root, path);
	this->silence = false;
	
	if (t.arr && t.arr->nitem > t.idx) {
		switch (t.arr->item[t.idx].valtype) {
			case 'i':
				return TYPE_INT;
			case 'd':
				return TYPE_FLOAT;
			case 'b':
				return TYPE_BOOL;
			case 's':
				return TYPE_STRING;
		}
	}
	
	if (t.tbl && toml_key_exists(t.tbl, t.item)) {
		for (var_t i = 0; i < t.tbl->nkval; i++) {
			if (!strcmp(t.tbl->kval[i]->key, t.item)) {
				
				switch (valtype(t.arr->item[t.idx].val)) {
					case 'i':
						return TYPE_INT;
					case 'd':
						return TYPE_FLOAT;
					case 'b':
						return TYPE_BOOL;
					case 's':
						return TYPE_STRING;
				}
			}
		}
	}
	
	return TYPE_NONE;
}

// # # # # # # # # # # # # # # # # # # # #
// # NumArray                            #
// # # # # # # # # # # # # # # # # # # # #

static TravelResult Toml_GetTravelImpl(Toml* this, const char* item, const char* cat, va_list va) {
	char buf[BUFFER_SIZE];
	char path[BUFFER_SIZE] = {};
	
	xl_vsnprintf(buf, BUFFER_SIZE, item, va);
	if (!*buf) return (TravelResult) { .tbl = this->root };
	strcat(buf, cat);
	
	this->silence = true;
	TravelResult t = Toml_Travel(this, buf, this->root, path);
	this->silence = false;
	
	return t;
}

void Toml_ListTabs(Toml* this, List* list, const char* item, ...) {
	va_list va;
	
	va_start(va, item);
	TravelResult t = Toml_GetTravelImpl(this, item, ".__get_tbl", va);
	va_end(va);
	
	List_Free(list);
	if (t.tbl) {
		List_Alloc(list, t.tbl->ntab);
		
		for (var_t i = 0; i < t.tbl->ntab; i++)
			List_Add(list, t.tbl->tab[i]->key);
	}
}

void Toml_ListVars(Toml* this, List* list, const char* item, ...) {
	va_list va;
	
	va_start(va, item);
	TravelResult t = Toml_GetTravelImpl(this, item, ".__get_tbl", va);
	va_end(va);
	
	List_Free(list);
	if (t.tbl) {
		List_Alloc(list, t.tbl->nkval);
		
		for (var_t i = 0; i < t.tbl->nkval; i++)
			List_Add(list, t.tbl->kval[i]->key);
	}
}

int Toml_ArrCount(Toml* this, const char* arr, ...) {
	va_list va;
	
	va_start(va, arr);
	TravelResult t = Toml_GetTravelImpl(this, arr, "[]", va);
	va_end(va);
	
	if (t.arr) return t.arr->nitem;
	
	return 0;
}

int Toml_TabItemNum(Toml* this, const char* item, ...) {
	va_list va;
	
	va_start(va, item);
	TravelResult t = Toml_GetTravelImpl(this, item, ".__get_tbl", va);
	va_end(va);
	
	if (t.tbl)
		return toml_table_narr(t.tbl) + toml_table_nkval(t.tbl) + toml_table_ntab(t.tbl);
	
	return 0;
}

int Toml_TabNum(Toml* this, const char* item, ...) {
	va_list va;
	
	va_start(va, item);
	TravelResult t = Toml_GetTravelImpl(this, item, ".__get_tbl", va);
	va_end(va);
	
	if (t.tbl)
		return t.tbl->ntab;
	
	return 0;
}

int Toml_ArrNum(Toml* this, const char* item, ...) {
	va_list va;
	
	va_start(va, item);
	TravelResult t = Toml_GetTravelImpl(this, item, ".__get_tbl", va);
	va_end(va);
	
	if (t.tbl)
		return t.tbl->narr;
	
	return 0;
}

int Toml_VarNum(Toml* this, const char* item, ...) {
	va_list va;
	
	va_start(va, item);
	TravelResult t = Toml_GetTravelImpl(this, item, ".__get_tbl", va);
	va_end(va);
	
	if (t.tbl)
		return t.tbl->nkval;
	
	return 0;
}

const char* Toml_VarKey(Toml* this, int index, const char* item, ...) {
	va_list va;
	
	va_start(va, item);
	TravelResult t = Toml_GetTravelImpl(this, item, ".__get_tbl", va);
	va_end(va);
	
	if (t.tbl && t.tbl->nkval > index)
		return t.tbl->kval[index]->key;
	
	return 0;
}

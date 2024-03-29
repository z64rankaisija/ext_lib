#include <ext_lib.h>
#include <dirent.h>

#undef ItemList_GetWildItem
#undef List_SetFilters

#undef List_Walk
#undef List_Tokenize2
#undef List_Free
#undef List_Alloc
#undef List_Add

// # # # # # # # # # # # # # # # # # # # #
// # ITEM LIST                           #
// # # # # # # # # # # # # # # # # # # # #

List gList_SortError;

static void List_Validate(List* itemList) {
	if (itemList->initKey == 0xDEFABEBACECAFAFF)
		return;
	
	*itemList = List_New();
}

List List_New(void) {
	return (List) { .initKey = 0xDEFABEBACECAFAFF, .p.alnum = 0 };
}

void List_SetFilters(List* list, u32 filterNum, ...) {
	va_list va;
	
	List_Validate(list);
	List_FreeFilters(list);
	
	if (filterNum % 2 == 1)
		warn("Odd filterNum! [%d]", --filterNum);
	
	va_start(va, filterNum);
	
	for (int i = 0; i < filterNum; i += 2) {
		FilterNode* node;
		
		node = calloc(sizeof(FilterNode));
		node->type = va_arg(va, ListFilter);
		node->txt = strdup(va_arg(va, char*));
		osAssert(node->txt != NULL);
		
		Node_Add(list->filterNode, node);
	}
	
	va_end(va);
}

void List_FreeFilters(List* this) {
	if (this->initKey == 0xDEFABEBACECAFAFF) {
		while (this->filterNode) {
			delete(this->filterNode->txt);
			
			Node_Kill(this->filterNode, this->filterNode);
		}
	} else
		*this = List_New();
}

typedef struct {
	strnode_t* node;
	u32 num;
	const ListFlag flags;
} WalkInfo;

static void List_WalkPath(const List* list, const char* base, const char* parent, const s32 level, const s32 max, WalkInfo* info) {
	const char* entryPath = parent;
	
	if (info->flags & LIST_RELATIVE)
		entryPath = parent + strlen(base);
	
	DIR* dir;
	if (strlen(parent) == 0) dir = opendir("./");
	else dir = opendir(parent);
	
	if (dir) {
		const struct dirent* entry;
		
		while ((entry = readdir(dir))) {
			strnode_t* node;
			char path[PATH_BUFFER_SIZE];
			bool skip = 0;
			bool filterContainUse = false;
			bool filterContainMatch = false;
			
			if (!memcmp(".\0", entry->d_name, 2) || !memcmp("..\0", entry->d_name, 3))
				continue;
			
			for (const FilterNode* filterNode = list->filterNode; filterNode != NULL; filterNode = filterNode->next) {
				switch (filterNode->type) {
					case FILTER_SEARCH:
						if (strstr(entry->d_name, filterNode->txt))
							skip = true;
						break;
					case FILTER_START:
						if (!memcmp(entry->d_name, filterNode->txt,
							strlen(filterNode->txt)))
							skip = true;
						break;
					case FILTER_END:
						if (strend(entry->d_name, filterNode->txt))
							skip = true;
						break;
					case FILTER_WORD:
						if (!strcmp(entry->d_name, filterNode->txt))
							skip = true;
						break;
						
					case CONTAIN_SEARCH:
						filterContainUse = true;
						if (strstr(entry->d_name, filterNode->txt))
							filterContainMatch = true;
						break;
					case CONTAIN_START:
						filterContainUse = true;
						if (!memcmp(entry->d_name, filterNode->txt,
							strlen(filterNode->txt)))
							filterContainMatch = true;
						break;
					case CONTAIN_END:
						filterContainUse = true;
						if (strend(entry->d_name, filterNode->txt))
							filterContainMatch = true;
						break;
					case CONTAIN_WORD:
						filterContainUse = true;
						if (!strcmp(entry->d_name, filterNode->txt))
							filterContainMatch = true;
						break;
				}
				
				if (skip)
					break;
			}
			
			if (skip) continue;
			if (((info->flags & LIST_NO_DOT) && entry->d_name[0] == '.')) continue;
			
			snprintf(path, PATH_BUFFER_SIZE, "%s%s", parent, entry->d_name);
			
			if (sys_isdir(path)) {
				strncat(path, "/", PATH_BUFFER_SIZE);
				
				if (max == -1 || level < max)
					List_WalkPath(list, base, path, level + 1, max, info);
				
				if ((info->flags & 0xF) == LIST_FOLDERS) {
					node = new(strnode_t);
					node->txt = fmt("%s%s/", entryPath, entry->d_name);
					info->num++;
					
					Node_Add(info->node, node);
				}
			} else {
				if (!filterContainUse || (filterContainUse && filterContainMatch)) {
					if ((info->flags & 0xF) == LIST_FILES) {
						node = new(strnode_t);
						node->txt = fmt("%s%s", entryPath, entry->d_name);
						info->num++;
						
						Node_Add(info->node, node);
					}
				}
			}
		}
		
		closedir(dir);
	} else
		errr("Could not open dir [%s]", dir);
}

void List_Walk(List* this, const char* path, s32 depth, ListFlag flags) {
	char buf[PATH_BUFFER_SIZE] = "";
	
	List_Validate(this);
	
	osAssert(path != NULL);
	if (strlen(path) > 0 ) {
		
		if (!sys_isdir(path))
			errr("Can't walk path that does not exist! [%s]", path);
		
		strcpy(buf, path);
		if (!strend(buf, "/")) strcat(buf, "/");
	}
	
	WalkInfo info = { .flags = flags };
	
	osLog("Walk: %s", buf);
	List_WalkPath(this, buf, buf, 0, depth, &info);
	
	if (info.num) {
		this->num = info.num;
		this->item = new(char*[info.num]);
		
		for (var_t i = 0; i < info.num; i++) {
			this->item[i] = info.node->txt;
			Node_Kill(info.node, info.node);
		}
	}
}

char* List_Concat(List* this, const char* separator) {
	u32 len = 0;
	u32 seplen = separator ? strlen(separator) : 0;
	
	for (var_t i = 0; i < this->num; i++) {
		if (!this->item[i]) continue;
		len += strlen(this->item[i]) + seplen;
	}
	
	if (!len) return NULL;
	
	char* r = new(char[len + 1]);
	
	for (var_t i = 0; i < this->num; i++) {
		if (!this->item[i]) continue;
		
		strcat(r, this->item[i]);
		if (seplen) strcat(r, separator);
	}
	
	if (separator)
		strend(r, separator)[0] = '\0';
	
	return r;
}

char* List_ConcatNumd(List* this, const char* separator) {
	u32 len = 0;
	u32 seplen = separator ? strlen(separator) : 0;
	
	for (var_t i = 0; i < this->num; i++) {
		if (!this->item[i]) continue;
		len += strlen(this->item[i]) + seplen + digint(this->num) + 8;
	}
	
	if (!len) return NULL;
	
	char* r = new(char[len + 1]);
	
	for (var_t i = 0; i < this->num; i++) {
		if (!this->item[i]) continue;
		
		strcat(r, x_fmt("%-*d: ", digint(this->num), i));
		strcat(r, this->item[i]);
		if (seplen) strcat(r, separator);
	}
	
	if (separator)
		strend(r, separator)[0] = '\0';
	
	return r;
}

void List_Tokenize2(List* list, const char* str, const char separator) {
	s32 a = 0;
	s32 b = 0;
	strnode_t* nodeHead = NULL;
	
	List_Free(list);
	
	while (str[a] == ' ' || str[a] == '\t' || str[a] == '\n' || str[a] == '\r') a++;
	
	while (true) {
		strnode_t* node = NULL;
		s32 isString = 0;
		s32 strcompns = 0;
		s32 brk = true;
		
		if (str[a] == '\"' || str[a] == '\'') {
			isString = 1;
			a++;
		}
		
		b = a;
		
		if (isString && (str[b] == '\"' || str[b] == '\'')) {
			node = calloc(sizeof(strnode_t));
			node->txt = calloc(2);
			Node_Add(nodeHead, node);
			
			b++;
			
			goto write;
		}
		
		while (isString || (str[b] != separator && str[b] != '\0')) {
			b++;
			if (isString && (str[b] == '\"' || str[b] == '\'')) {
				isString = 0;
				strcompns = -1;
			}
		}
		while (!isString && a != b && separator != ' ' && str[b - 1] == ' ') b--;
		
		if (b == a)
			break;
		
		for (s32 v = 0; v < b - a + strcompns + 1; v++) {
			if (isgraph(str[a + v]))
				brk = false;
		}
		
		if (brk)
			break;
		
		node = calloc(sizeof(strnode_t));
		node->txt = calloc(b - a + strcompns + 1);
		memcpy(node->txt, &str[a], b - a + strcompns);
		Node_Add(nodeHead, node);
		
		write:
		list->num++;
		
		a = b;
		
		while (str[a] == separator || str[a] == ' ' || str[a] == '\t' || str[a] == '\n' || str[a] == '\r') a++;
		if (str[a] == '\0')
			break;
	}
	
	list->item = new(char*[list->num]);
	
	for (int i = 0; i < list->num; i++) {
		list->item[i] = nodeHead->txt;
		Node_Kill(nodeHead, nodeHead);
	}
}

void List_Print(List* target) {
	const char* cat = List_ConcatNumd(target, "\n");
	
	info("" PRNT_BLUE "list content:" PRNT_RSET "\n%s", cat);
	delete(cat);
}

time_t List_StatMax(List* list) {
	time_t val = 0;
	
	for (int i = 0; i < list->num; i++)
		val = Max(val, sys_stat(list->item[i]));
	
	return val;
}

time_t List_StatMin(List* list) {
	time_t val = List_StatMax(list);
	
	for (int i = 0; i < list->num; i++)
		val = Min(val, sys_stat(list->item[i]));
	
	return val;
}

s32 List_SortSlot(List* this, bool checkOverlaps) {
	bool error = false;
	s32 max = -1;
	List new = List_New();
	
	if (this->num == 0)
		return 0;
	
	List_Sort(this);
	
	for (int i = this->num - 1; i >= 0; i--) {
		if (!this->item[i]) continue;
		if (!isdigit(this->item[i][0])) continue;
		
		max = sint(this->item[i]) + 1;
		break;
	}
	if (max <= 0) {
		errr("List with [%s] item didn't contain max!", this->item[0]);
	}
	osLog("max Num: %d", max);
	
	new.item = new(char*[max]);
	new.num = max;
	
	u32 j = 0;
	
	for (; j < this->num; j++) {
		if (!this->item[j]) continue;
		if (isdigit(this->item[j][0]))
			break;
	}
	osLog("J: %d / %d", j, this->num - 1);
	
	if (checkOverlaps) {
		osLog("Check Overlap");
		for (var_t i = 0; i < this->num - 1; i++) {
			if (isdigit(this->item[i][0]) && isdigit(this->item[i + 1][0])) {
				if (sint(this->item[i]) == sint(this->item[i + 1])) {
					osLog("" PRNT_REDD "![%s]!", this->item[i]);
					List_Add(&gList_SortError, this->item[i]);
					List_Add(&gList_SortError, this->item[i + 1]);
					error = true;
				}
			}
		}
	}
	
	for (var_t i = 0; i < max; i++) {
		while (j < this->num && (!this->item[j] || !isdigit(this->item[j][0])))
			j++;
		
		if (j < this->num && i == sint(this->item[j])) {
			osLog("Insert Entry: [%s]", this->item[j]);
			new.item[i] = this->item[j];
			this->item[j++] = NULL;
		} else
			new.item[i] = NULL;
	}
	
	osLog("Swap Tables");
	List_Free(this);
	*this = new;
	
	osLog("OK");
	
	if (checkOverlaps && gList_SortError.num) {
		warn("Index overlap");
		for (var_t i = 0; i < gList_SortError.num; i += 2)
			warn("[" PRNT_YELW "%s" PRNT_RSET "] vs "
				"[" PRNT_YELW "%s" PRNT_RSET "]",
				gList_SortError.item[i], gList_SortError.item[i + 1]);
		
		List_Free(&gList_SortError);
	}
	
	return error;
}

void List_FreeItems(List* this) {
	if (this->initKey == 0xDEFABEBACECAFAFF) {
		if (this->item) {
			for (; this->num > 0; this->num--)
				delete(this->item[this->num - 1]);
			delete(this->item);
		}
		this->p.alnum = 0;
	} else
		*this = List_New();
}

void List_Free(List* this) {
	osAssert(this != NULL);
	
	List_FreeItems(this);
	List_FreeFilters(this);
	
	*this = List_New();
}

void List_Alloc(List* this, u32 num) {
	List_Free(this);
	
	this->p.alnum = num;
	this->item = new(char*[num]);
}

static void list_realloc(List* this, u32 num) {
	this->p.alnum = num;
	this->item = realloc(this->item, sizeof(char*[num]));
}

void List_Add(List* this, const char* item) {
	if (this->p.alnum && this->p.alnum < this->num + 1)
		list_realloc(this, this->p.alnum * 2 + 10);
	else
		this->item = realloc(this->item, sizeof(char*[this->num + 1]));
	this->item[this->num++] = item ? strdup(item) : NULL;
}

void List_Combine(List* out, List* a, List* b) {
	List_Free(out);
	
	List_Alloc(out, a->num + b->num);
	
	for (var_t i = 0; i < a->num; i++)
		List_Add(out, a->item[i]);
	
	for (var_t i = 0; i < b->num; i++)
		List_Add(out, b->item[i]);
}

void List_Tokenize(List* this, const char* s, char r) {
	char* token;
	char sep[2] = { r };
	char* buf = strdup(s);
	
	List_Free(this);
	
	osLog("Tokenize: [%s]", s);
	osLog("Separator: [%c]", r);
	
	token = strtok(buf, sep);
	while (token) {
		this->num++;
		osLog("%d: %s", this->num - 1, token);
		token = strtok(NULL, sep);
	}
	
	if (!this->num) {
		delete(buf);
		return;
	}
	
	this->item = calloc(sizeof(char*) * this->num);
	
	token = buf;
	for (int i = 0; i < this->num; i++) {
		char* end;
		
		this->item[i] = strdup(token + strspn(token, " "));
		token += strlen(token) + 1;
		while (( end = strend(this->item[i], " ") ))
			*end = '\0';
	}
	
	delete(buf);
}

void List_Sort(List* this) {
	qsort(this->item, this->num, sizeof(char*), qsort_numhex);
}

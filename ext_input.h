#ifndef EXT_INPUT_H
#define EXT_INPUT_H
#include <GLFW/glfw3.h>
#include "ext_lib.h"
#include "ext_math.h"
#include "ext_vector.h"

struct Window;

enum {
	KEY_SPACE         = 32,
	KEY_APOSTROPHE    = 39,
	KEY_COMMA         = 44,
	KEY_MINUS         = 45,
	KEY_PERIOD        = 46,
	KEY_SLASH         = 47,
	KEY_0             = 48,
	KEY_1             = 49,
	KEY_2             = 50,
	KEY_3             = 51,
	KEY_4             = 52,
	KEY_5             = 53,
	KEY_6             = 54,
	KEY_7             = 55,
	KEY_8             = 56,
	KEY_9             = 57,
	KEY_SEMICOLON     = 59,
	KEY_EQUAL         = 61,
	KEY_A             = 65,
	KEY_B             = 66,
	KEY_C             = 67,
	KEY_D             = 68,
	KEY_E             = 69,
	KEY_F             = 70,
	KEY_G             = 71,
	KEY_H             = 72,
	KEY_I             = 73,
	KEY_J             = 74,
	KEY_K             = 75,
	KEY_L             = 76,
	KEY_M             = 77,
	KEY_N             = 78,
	KEY_O             = 79,
	KEY_P             = 80,
	KEY_Q             = 81,
	KEY_R             = 82,
	KEY_S             = 83,
	KEY_T             = 84,
	KEY_U             = 85,
	KEY_V             = 86,
	KEY_W             = 87,
	KEY_X             = 88,
	KEY_Y             = 89,
	KEY_Z             = 90,
	KEY_LEFT_BRACKET  = 91,
	KEY_BACKSLASH     = 92,
	KEY_RIGHT_BRACKET = 93,
	KEY_GRAVE_ACCENT  = 96,
	KEY_WORLD_1       = 161,
	KEY_WORLD_2       = 162,
	KEY_ESCAPE        = 256,
	KEY_ENTER         = 257,
	KEY_TAB           = 258,
	KEY_BACKSPACE     = 259,
	KEY_INSERT        = 260,
	KEY_DELETE        = 261,
	KEY_RIGHT         = 262,
	KEY_LEFT          = 263,
	KEY_DOWN          = 264,
	KEY_UP            = 265,
	KEY_PAGE_UP       = 266,
	KEY_PAGE_DOWN     = 267,
	KEY_HOME          = 268,
	KEY_END           = 269,
	KEY_CAPS_LOCK     = 280,
	KEY_SCROLL_LOCK   = 281,
	KEY_NUM_LOCK      = 282,
	KEY_PRINT_SCREEN  = 283,
	KEY_PAUSE         = 284,
	KEY_F1            = 290,
	KEY_F2            = 291,
	KEY_F3            = 292,
	KEY_F4            = 293,
	KEY_F5            = 294,
	KEY_F6            = 295,
	KEY_F7            = 296,
	KEY_F8            = 297,
	KEY_F9            = 298,
	KEY_F10           = 299,
	KEY_F11           = 300,
	KEY_F12           = 301,
	KEY_F13           = 302,
	KEY_F14           = 303,
	KEY_F15           = 304,
	KEY_F16           = 305,
	KEY_F17           = 306,
	KEY_F18           = 307,
	KEY_F19           = 308,
	KEY_F20           = 309,
	KEY_F21           = 310,
	KEY_F22           = 311,
	KEY_F23           = 312,
	KEY_F24           = 313,
	KEY_F25           = 314,
	KEY_KP_0          = 320,
	KEY_KP_1          = 321,
	KEY_KP_2          = 322,
	KEY_KP_3          = 323,
	KEY_KP_4          = 324,
	KEY_KP_5          = 325,
	KEY_KP_6          = 326,
	KEY_KP_7          = 327,
	KEY_KP_8          = 328,
	KEY_KP_9          = 329,
	KEY_KP_DECIMAL    = 330,
	KEY_KP_DIVIDE     = 331,
	KEY_KP_MULTIPLY   = 332,
	KEY_KP_SUBTRACT   = 333,
	KEY_KP_ADD        = 334,
	KEY_KP_ENTER      = 335,
	KEY_KP_EQUAL      = 336,
	KEY_LEFT_SHIFT    = 340,
	KEY_LEFT_CONTROL  = 341,
	KEY_LEFT_ALT      = 342,
	KEY_LEFT_SUPER    = 343,
	KEY_RIGHT_SHIFT   = 344,
	KEY_RIGHT_CONTROL = 345,
	KEY_RIGHT_ALT     = 346,
	KEY_RIGHT_SUPER   = 347,
	KEY_MENU          = 348,
	
	KEY_MAX
};

typedef enum {
	CLICK_L,
	CLICK_R,
	CLICK_M,
	CLICK_ANY,
	CLICK_MAX
} CursorClick;

typedef enum {
	CURSOR_ACTION_PRESS,
	CURSOR_ACTION_HOLD,
	CURSOR_ACTION_RELEASE,
	CURSOR_ACTION_DOUBLE,
	CURSOR_ACTION_MAX,
} CursorAction;

typedef union {
	struct {
		bool press;
		bool hold;
		bool prev;
		bool release;
		bool dual;
		u8   __timer;
	};
	bool __action[CURSOR_ACTION_MAX];
} InputType;

typedef struct {
	union {
		struct {
			InputType clickL;
			InputType clickR;
			InputType clickMid;
			InputType clickAny;
		};
		InputType clickList[CLICK_MAX];
	};
	bool  cursorAction;
	f64   scrollY;
	Vec2s pos;
	Vec2s pressPos;
	f32   dragDist;
	f32   postPressMoveDist;
	Vec2s vel;
} Cursor;

typedef enum {
	INPUT_BLOCK      = 1 << 0,
	INPUT_PERMISSIVE = 1 << 1,
} InputState;

typedef struct {
	struct Window* window;
	Cursor     cursor;
	InputType  key[KEY_MAX];
	bool       keyAction;
	InputState state;
	char       buffer[512];
	
	int tempLock;
	
	struct {
		bool permissive : 1;
	};
} Input;

#define MOUSE_KEEP_AXIS 0xD0D0CAFE

extern int gScrollSpeedMod;

void Input_SetTempLock(Input* this, int frames);
void Input_Init(Input* input, struct Window* window);
void Input_Update(Input* input);
void Input_End(Input* input);
const char* Input_GetClipboardStr(Input* input);
void Input_SetClipboardStr(Input* input, const char* str);
InputType* Input_GetKey(Input* input, int key);
InputType* Input_GetCursor(Input* input, CursorClick type);
f32 Input_GetScrollRaw(Input* this);
f32 Input_GetScroll(Input* this);
s32 Input_GetShortcut(Input* input, int mod, int key);
void Input_SetMousePos(Input* input, s32 x, s32 y);
f32 Input_GetPressPosDist(Input* input);
int Input_SelectClick(Input* this, CursorClick type);

int Input_SetState(Input* this, InputState state);
int Input_ClearState(Input* this, InputState state);

void InputCallback_Key(GLFWwindow* window, s32 key, s32 scancode, s32 action, s32 mods);
void InputCallback_Text(GLFWwindow* window, u32 scancode);
void InputCallback_Mouse(GLFWwindow* window, s32 button, s32 action, s32 mods);
void InputCallback_MousePos(GLFWwindow* window, f64 x, f64 y);
void InputCallback_Scroll(GLFWwindow* window, f64 x, f64 y);

#endif

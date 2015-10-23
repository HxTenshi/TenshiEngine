#pragma once

enum class MouseCoord{
	Left,
	Right,
	Middle,
	Count
};

enum class KeyCoord{
	/*
	DIK_0,
	DIK_1,
	DIK_2,
	DIK_3,
	DIK_4,
	DIK_5,
	DIK_6,
	DIK_7,
	DIK_8,
	DIK_9,
	DIK_A,
	DIK_ABNT_C1,
	DIK_ABNT_C2,
	DIK_ADD,
	DIK_APOSTROPHE,
	DIK_APPS,
	DIK_AT,
	DIK_AX,
	DIK_B,
	DIK_BACK,
	DIK_BACKSLASH,
	DIK_C,
	DIK_CALCULATOR,
	DIK_CAPITAL,
	DIK_COLON,
	DIK_COMMA,
	DIK_CONVERT,
	DIK_D,
	DIK_DECIMAL,
	DIK_DELETE,
	DIK_DIVIDE,
	DIK_DOWN,
	DIK_E,
	DIK_END,
	DIK_EQUALS,
	DIK_ESCAPE,
	DIK_F,
	DIK_F1,
	DIK_F2,
	DIK_F3,
	DIK_F4,
	DIK_F5,
	DIK_F6,
	DIK_F7,
	DIK_F8,
	DIK_F9,
	DIK_F10,
	DIK_F11,
	DIK_F12,
	DIK_F13,
	DIK_F14,
	DIK_F15,
	DIK_G,
	DIK_GRAVE,
	DIK_H,
	DIK_HOME,
	DIK_I,
	DIK_INSERT,
	DIK_J,
	DIK_K,
	DIK_KANA,
	DIK_KANJI,
	DIK_L,
	DIK_LBRACKET,
	DIK_LCONTROL,
	DIK_LEFT,
	DIK_LMENU,
	DIK_LSHIFT,
	DIK_LWIN,
	DIK_M,
	DIK_MAIL,
	DIK_MEDIASELECT,
	DIK_MEDIASTOP,
	DIK_MINUS,
	DIK_MULTIPLY,
	DIK_MUTE,
	DIK_MYCOMPUTER,
	DIK_N,
	DIK_NEXT,
	DIK_NEXTTRACK,
	DIK_NOCONVERT,
	DIK_NUMLOCK,
	DIK_NUMPAD0,
	DIK_NUMPAD1,
	DIK_NUMPAD2,
	DIK_NUMPAD3,
	DIK_NUMPAD4,
	DIK_NUMPAD5,
	DIK_NUMPAD6,
	DIK_NUMPAD7,
	DIK_NUMPAD8,
	DIK_NUMPAD9,
	DIK_NUMPADCOMMA,
	DIK_NUMPADENTER,
	DIK_NUMPADEQUALS,
	DIK_O,
	DIK_OEM_102,
	DIK_P,
	DIK_PAUSE,
	DIK_PERIOD,
	DIK_PLAYPAUSE,
	DIK_POWER,
	DIK_PREVTRACK,
	DIK_PRIOR,
	DIK_Q,
	DIK_R,
	DIK_RBRACKET,
	DIK_RCONTROL,
	DIK_RETURN,
	DIK_RIGHT,
	DIK_RMENU,
	DIK_RSHIFT,
	DIK_RWIN,
	DIK_S,
	DIK_SCROLL,
	DIK_SEMICOLON,
	DIK_SLASH,
	DIK_SLEEP,
	DIK_SPACE,
	DIK_STOP,
	DIK_SUBTRACT,
	DIK_SYSRQ,
	DIK_T,
	DIK_TAB,
	DIK_U,
	DIK_UNDERLINE,
	DIK_UNLABELED,
	DIK_UP,
	DIK_V,
	DIK_VOLUMEDOWN,
	DIK_VOLUMEUP,
	DIK_W,
	DIK_WAKE,
	DIK_WEBBACK,
	DIK_WEBFAVORITES,
	DIK_WEBFORWARD,
	DIK_WEBHOME,
	DIK_WEBREFRESH,
	DIK_WEBSEARCH,
	DIK_WEBSTOP,
	DIK_X,
	DIK_Y,
	DIK_YEN,
	DIK_Z,
	Count
	*/

 Key_ESCAPE=1          //0x01
,Key_1               //0x02
,Key_2               //0x03
,Key_3               //0x04
,Key_4               //0x05
,Key_5               //0x06
,Key_6               //0x07
,Key_7               //0x08
,Key_8               //0x09
,Key_9               //0x0A
,Key_0               //0x0B
,Key_MINUS           //0x0C
,Key_EQUALS          //0x0D
,Key_BACK            //0x0E
,Key_TAB             //0x0F
,Key_Q               //0x10
,Key_W               //0x11
,Key_E               //0x12
,Key_R               //0x13
,Key_T               //0x14
,Key_Y               //0x15
,Key_U               //0x16
,Key_I               //0x17
,Key_O               //0x18
,Key_P               //0x19
,Key_LBRACKET        //0x1A
,Key_RBRACKET        //0x1B
,Key_RETURN          //0x1C
,Key_LCONTROL        //0x1D
,Key_A               //0x1E
,Key_S               //0x1F
,Key_D               //0x20
,Key_F               //0x21
,Key_G               //0x22
,Key_H               //0x23
,Key_J               //0x24
,Key_K               //0x25
,Key_L               //0x26
,Key_SEMICOLON       //0x27
,Key_APOSTROPHE      //0x28
,Key_GRAVE           //0x29
,Key_LSHIFT          //0x2A
,Key_BACKSLASH       //0x2B
,Key_Z               //0x2C
,Key_X               //0x2D
,Key_C               //0x2E
,Key_V               //0x2F
,Key_B               //0x30
,Key_N               //0x31
,Key_M               //0x32
,Key_COMMA           //0x33
,Key_PERIOD          //0x34
,Key_SLASH           //0x35
,Key_RSHIFT          //0x36
,Key_MULTIPLY        //0x37
,Key_LMENU           //0x38
,Key_SPACE           //0x39
,Key_CAPITAL         //0x3A
,Key_F1              //0x3B
,Key_F2              //0x3C
,Key_F3              //0x3D
,Key_F4              //0x3E
,Key_F5              //0x3F
,Key_F6              //0x40
,Key_F7              //0x41
,Key_F8              //0x42
,Key_F9              //0x43
,Key_F10             //0x44
,Key_NUMLOCK         //0x45
,Key_SCROLL          //0x46
,Key_NUMPAD7         //0x47
,Key_NUMPAD8         //0x48
,Key_NUMPAD9         //0x49
,Key_SUBTRACT        //0x4A
,Key_NUMPAD4         //0x4B
,Key_NUMPAD5         //0x4C
,Key_NUMPAD6         //0x4D
,Key_ADD             //0x4E
,Key_NUMPAD1         //0x4F
,Key_NUMPAD2         //0x50
,Key_NUMPAD3         //0x51
,Key_NUMPAD0         //0x52
,Key_DECIMAL         //0x53
,Key_OEM_102         //0x56
,Key_F11             //0x57
,Key_F12             //0x58
,Key_F13             //0x64
,Key_F14             //0x65
,Key_F15             //0x66
,Key_KANA            //0x70
,Key_ABNT_C1         //0x73
,Key_CONVERT         //0x79
,Key_NOCONVERT       //0x7B
,Key_YEN             //0x7D
,Key_ABNT_C2         //0x7E
,Key_NUMPADEQUALS    //0x8D
,Key_PREVTRACK       //0x90
,Key_AT              //0x91
,Key_COLON           //0x92
,Key_UNDERLINE       //0x93
,Key_KANJI           //0x94
,Key_STOP            //0x95
,Key_AX              //0x96
,Key_UNLABELED       //0x97
,Key_NEXTTRACK       //0x99
,Key_NUMPADENTER     //0x9C
,Key_RCONTROL        //0x9D
,Key_MUTE            //0xA0
,Key_CALCULATOR      //0xA1
,Key_PLAYPAUSE       //0xA2
,Key_MEDIASTOP       //0xA4
,Key_VOLUMEDOWN      //0xAE
,Key_VOLUMEUP        //0xB0
,Key_WEBHOME         //0xB2
,Key_NUMPADCOMMA     //0xB3
,Key_DIVIDE          //0xB5
,Key_SYSRQ           //0xB7
,Key_RMENU           //0xB8
,Key_PAUSE           //0xC5
,Key_HOME            //0xC7
,Key_UP              //0xC8
,Key_PRIOR           //0xC9
,Key_LEFT            //0xCB
,Key_RIGHT           //0xCD
,Key_END             //0xCF
,Key_DOWN            //0xD0
,Key_NEXT            //0xD1
,Key_INSERT          //0xD2
,Key_DELETE          //0xD3
,Key_LWIN            //0xDB
,Key_RWIN            //0xDC
,Key_APPS            //0xDD
,Key_POWER           //0xDE
,Key_SLEEP           //0xDF
,Key_WAKE            //0xE3
,Key_WEBSEARCH       //0xE5
,Key_WEBFAVORITES    //0xE6
,Key_WEBREFRESH      //0xE7
,Key_WEBSTOP         //0xE8
,Key_WEBFORWARD      //0xE9
,Key_WEBBACK         //0xEA
,Key_MYCOMPUTER      //0xEB
,Key_MAIL            //0xEC
,Key_MEDIASELECT     //0xED
,Count
};

enum class PAD_DS4_KeyCoord{
	Button_UP,
	Button_DOWN,
	Button_LEFT,
	Button_RIGHT,
	Button_TRIANGLE,
	Button_CIRCLE,
	Button_SQUARE,
	Button_CROSS,
	Button_L1,
	Button_L2,
	Button_L3,
	Button_R1,
	Button_R2,
	Button_R3,
	Button_PS,
	Button_SHARE,
	Button_OPTIONS,
	Button_TOUCHPAD,
	Button_1stTOUCH,
	Button_2ndTOUCH,
	Count,
};
//2
enum class PAD_DS4_LevelCoord{
	Level_L2,
	Level_R2,
	Level_LStickX,
	Level_LStickY,
	Level_RStickX,
	Level_RStickY,
	Velo2_1stTouchX,
	Velo2_1stTouchY,
	Velo2_2ndTouchX,
	Velo2_2ndTouchY,
	Velo3_AngularX,
	Velo3_AngularY,
	Velo3_AngularZ,
	Velo3_AccelerationX,
	Velo3_AccelerationY,
	Velo3_AccelerationZ,
	Count,
};

//8
enum class PAD_DS4_Velo2Coord{
	Velo2_LStick,
	Velo2_RStick,
	Velo2_1stTouch,
	Velo2_2ndTouch,
	Count,
};

//6
enum class PAD_DS4_Velo3Coord{
	Velo3_Angular,
	Velo3_Acceleration,
	Count,
};

enum class PAD_X_KeyCoord{
	Button_UP,
	Button_DOWN,
	Button_LEFT,
	Button_RIGHT,
	Button_A,
	Button_B,
	Button_X,
	Button_Y,
	Button_L1,
	Button_L2,
	Button_L3,
	Button_R1,
	Button_R2,
	Button_R3,
	Button_START,
	Button_BACK,
	Count,
};
enum class PAD_X_LevelCoord{
	Level_L2,
	Level_R2,
	Level_LStickX,
	Level_LStickY,
	Level_RStickX,
	Level_RStickY,
	Count,
};

enum class PAD_X_Velo2Coord{
	Velo2_LStick,
	Velo2_RStick,
	Count,
};
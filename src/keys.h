/*
MX2000DRV - portable driver for the MX-2000 II gaming mouse
Copyright (C) 2015  Dan Panzarella

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef MX_KEYS_H
#define MX_KEYS_H


#define MACRO_DELAY_TIMESCALE 8

/* length of macro command is added to this base */
#define MACRO_LOOP_LEN 0x80

#define MAX_LOOP 0xFF

/* Loop count or fire button are stored in global mem, in a macro setting area
   they index into byte position based on this */
#define MACRO_SETTING_INDEX 0x70

/* the least significant byte seems to determine a class or type of keypress
    this is the mask to get the LSByte, and then the few consistent types mapped */
#define KEYTYPE_MSK 0xFF

#define KEYTYPE_DELAY 0x44
#define KEYTYPE_MOUSE 0x42
#define KEYTYPE_SCROLL 0x43
#define KEYTYPE_MACRO 0x80
#define KEYTYE_INTERNAL 0x4A



/* ---------
	KEYBOARD KEYS FOR BUTTON MAPPING AND MACROS
   --------- */

/* ------------- keys you can press ------------ */
/* 00 - 03 generated no events */
#define KEY_a 0x0400
#define KEY_b 0x0500
#define KEY_c 0x0600
#define KEY_d 0x0700
#define KEY_e 0x0800
#define KEY_f 0x0900
#define KEY_g 0x0A00
#define KEY_h 0x0B00
#define KEY_i 0x0C00
#define KEY_j 0x0D00
#define KEY_k 0x0E00
#define KEY_l 0x0F00
#define KEY_m 0x1000
#define KEY_n 0x1100
#define KEY_o 0x1200
#define KEY_p 0x1300
#define KEY_q 0x1400
#define KEY_r 0x1500
#define KEY_s 0x1600
#define KEY_t 0x1700
#define KEY_u 0x1800
#define KEY_v 0x1900
#define KEY_w 0x1A00
#define KEY_x 0x1B00
#define KEY_y 0x1C00
#define KEY_z 0x1D00
#define KEY_1 0x1E00
#define KEY_2 0x1F00
#define KEY_3 0x2000
#define KEY_4 0x2100
#define KEY_5 0x2200
#define KEY_6 0x2300
#define KEY_7 0x2400
#define KEY_8 0x2500
#define KEY_9 0x2600
#define KEY_0 0x2700
#define KEY_RETURN 0x2800
#define KEY_ESC 0x2900
#define KEY_BACKSPACE 0x2A00
#define KEY_TAB 0x2B00
#define KEY_SPACE 0x2C00
#define KEY_MINUS 0x2D00
#define KEY_EQUAL 0x2E00
#define KEY_OPEN_BRACKET 0x2F00
#define KEY_CLOSE_BRACKET 0x3000
#define KEY_BACKSLASH 0x3100
/* 0x32 is also backslash */
#define KEY_SEMICOLON 0x3300
#define KEY_APOSTROPHE 0x3400
#define KEY_GRAVE 0x3500
#define KEY_COMMA 0x3600
#define KEY_PERIOD 0x3700
#define KEY_SLASH 0x3800
#define KEY_CAPS 0x3900
#define KEY_F7 0x4000
#define KEY_F8 0x4100
#define KEY_F9 0x4200
#define KEY_F10 0x4300
#define KEY_F11 0x4400
#define KEY_F12 0x4500
#define KEY_PRINT 0x4600
#define KEY_SCR_LOCK 0x4700
#define KEY_PAUSE 0x4800 /* keycode 127 */
#define KEY_INSERT 0x4900 /* 118 */
#define KEY_HOME 0x4A00
#define KEY_PRIOR 0x4B00 /* 112 */
#define KEY_DEL 0x4C00
#define KEY_END 0x4D00
#define KEY_NEXT 0x4E00 /* 117, ff56 */
#define KEY_ARROW_RIGHT 0x4F00
#define KEY_ARROW_LEFT 0x5000
#define KEY_ARROW_DOWN 0x5100
#define KEY_ARROW_UP 0x5200
#define KEY_NUM_LOCK 0x5300
#define KEY_KP_DIVIDE 0x5400
#define KEY_KP_MULT 0x5500
#define KEY_KP_MINUS 0x5600
#define KEY_KP_PLUS 0x5700
#define KEY_KP_ENTER 0x5800
#define KEY_KP_1 0x5900
#define KEY_KP_2 0x5A00
#define KEY_KP_3 0x5B00
#define KEY_KP_4 0x5C00
#define KEY_KP_5 0x5D00
#define KEY_KP_6 0x5E00
#define KEY_KP_7 0x5F00
#define KEY_KP_8 0x6000
#define KEY_KP_9 0x6100
#define KEY_KP_0 0x6200
#define KEY_KP_DEL 0x6300 /* period when numlock on */
#define KEY_LESS 0x6400
#define KEY_MENU 0x6500
#define KEY_POWEROFF 0x6600 /* 124 */
#define KEY_KP_EQUAL 0x6700
#define KEY_XF86Tools 0x6800 /* 191 */
#define KEY_XF86Launch5 0x6900 /* 192 */
#define KEY_XF86Launch6 0x6A00 /* 193 */
#define KEY_XF86Launch7 0x6B00 /* 194 */
#define KEY_XF86Launch8 0x6C00 /* 195 */
#define KEY_XF86Launch9 0x6D00 /* 196 */
#define KEY_NOSYMBOL 0x6E00 /* 197 */
#define KEY_XF86AudioMicMute 0x6F00
#define KEY_XF86TouchPadToggle 0x7000
#define KEY_XF86TouchPadOn 0x7100
#define KEY_XF86TouchPadOff 0x7200
#define KEY_NOSYMBOL 0x7300 /* 202 */
#define KEY_XF86Open 0x7400
#define KEY_HELP 0x7500 /* 146 */
#define KEY_SUNPROPS 0x7600 /* 138 */
#define KEY_SUNFRONT 0x7700 /* 140 */
#define KEY_CANCEL 0x7800 /* 136 */
#define KEY_REDO 0x7900 /* 137 */
#define KEY_UNDO 0x7A00 /* 139 */
#define KEY_XF86Cut 0x7B00 /* 145 */
#define KEY_XF86Copy 0x7C00 /* 141 */
#define KEY_XF86Paste 0x7D00 /* 143 */
#define KEY_FIND 0x7E00 /* 144 */
#define KEY_XF86AudioMute 0x7F00 /* 121 */
#define KEY_XF86AudioRaiseVolume 0x8000
#define KEY_XF86AudioLowerVolume 0x8100
#define KEY_NOSYMBOL 0x8200 /* 248 */
#define KEY_NOSYMBOL 0x8300
#define KEY_NOSYMBOL 0x8400
#define KEY_KP_PERIOD 0x8500
#define KEY_NOSYMBOL 0x8600
#define KEY_NOSYMBOL 0x8700
#define KEY_HIRAGANA_KATAKANA 0x8800
#define KEY_NOSYMBOL 0x8900
#define KEY_HENKAN_MODE 0x8A00
#define KEY_MUHENKAN 0x8B00
#define KEY_NOSYMBOL 0x8C00 /* 103 */
#define KEY_NOSYMBOL 0x8D00 /* 248 */
#define KEY_NOSYMBOL 0x8E00
#define KEY_NOSYMBOL 0x8F00
#define KEY_HANGUL 0x9000
#define KEY_HANGUL_HANJA 0x9100
#define KEY_KATAKANA 0x9200
#define KEY_HIRAGANA 0x9300
#define KEY_NOSYMBOL 0x9400 /* 93 */
#define KEY_NOSYMBOL 0x9500
#define KEY_NOSYMBOL 0x9600 /* 248 */
#define KEY_NOSYMBOL 0x9700
#define KEY_NOSYMBOL 0x9800
#define KEY_NOSYMBOL 0x9900
#define KEY_NOSYMBOL 0x9A00
#define KEY_NOSYMBOL 0x9B00
#define KEY_DEL 0x9C00 /* 119 */
#define KEY_NOSYMBOL 0x9D00
#define KEY_NOSYMBOL 0x9E00
#define KEY_NOSYMBOL 0x9F00
#define KEY_NOSYMBOL 0xA000
#define KEY_NOSYMBOL 0xA100
#define KEY_NOSYMBOL 0xA200
#define KEY_NOSYMBOL 0xA300
#define KEY_NOSYMBOL 0xA400
#define KEY_NOSYMBOL 0xA500
#define KEY_NOSYMBOL 0xA600
#define KEY_NOSYMBOL 0xA700
#define KEY_NOSYMBOL 0xA800
#define KEY_NOSYMBOL 0xA900
#define KEY_NOSYMBOL 0xAA00
#define KEY_NOSYMBOL 0xAB00
#define KEY_NOSYMBOL 0xAC00
#define KEY_NOSYMBOL 0xAD00
#define KEY_NOSYMBOL 0xAE00
#define KEY_NOSYMBOL 0xAF00
#define KEY_NOSYMBOL 0xB000
#define KEY_NOSYMBOL 0xB100
#define KEY_NOSYMBOL 0xB200
#define KEY_NOSYMBOL 0xB300
#define KEY_NOSYMBOL 0xB400
#define KEY_NOSYMBOL 0xB500
#define KEY_OPEN_PAREN 0xB600
#define KEY_CLOSE_PAREN 0xB700
#define KEY_NOSYMBOL 0xB800
#define KEY_NOSYMBOL 0xB900
#define KEY_NOSYMBOL 0xBA00
#define KEY_NOSYMBOL 0xBB00
#define KEY_NOSYMBOL 0xBC00
#define KEY_NOSYMBOL 0xBD00
#define KEY_NOSYMBOL 0xBE00
#define KEY_NOSYMBOL 0xBF00
#define KEY_NOSYMBOL 0xC000
#define KEY_NOSYMBOL 0xC100
#define KEY_NOSYMBOL 0xC200
#define KEY_NOSYMBOL 0xC300
#define KEY_NOSYMBOL 0xC400
#define KEY_NOSYMBOL 0xC500
#define KEY_NOSYMBOL 0xC600
#define KEY_NOSYMBOL 0xC700
#define KEY_NOSYMBOL 0xC800
#define KEY_NOSYMBOL 0xC900
#define KEY_NOSYMBOL 0xCA00
#define KEY_NOSYMBOL 0xCB00
#define KEY_NOSYMBOL 0xCC00
#define KEY_NOSYMBOL 0xCD00
#define KEY_NOSYMBOL 0xCE00
#define KEY_NOSYMBOL 0xCF00
#define KEY_NOSYMBOL 0xD000
#define KEY_NOSYMBOL 0xD100
#define KEY_NOSYMBOL 0xD200
#define KEY_NOSYMBOL 0xD300
#define KEY_NOSYMBOL 0xD400
#define KEY_NOSYMBOL 0xD500
#define KEY_NOSYMBOL 0xD600
#define KEY_NOSYMBOL 0xD700
#define KEY_DEL 0xD800
#define KEY_NOSYMBOL 0xD900
#define KEY_NOSYMBOL 0xDA00
#define KEY_NOSYMBOL 0xDB00
#define KEY_NOSYMBOL 0xDC00
#define KEY_NOSYMBOL 0xDD00
#define KEY_NOSYMBOL 0xDE00
#define KEY_NOSYMBOL 0xDF00
#define KEY_L_CTL 0xE000
#define KEY_L_SHIFT 0xE100
#define KEY_L_ALT 0xE200
#define KEY_L_SUPER 0xE300
#define KEY_R_CTL 0xE400
#define KEY_R_SHIFT 0xE500
#define KEY_R_ALT 0xE600
#define KEY_R_SUPER 0xE700
#define KEY_XF86AudioPlay 0xE800
#define KEY_XF86AudioStop 0xE900
#define KEY_XF86AudioPrev 0xEA00
#define KEY_XF86AudioNext 0xEB00
#define KEY_XF86Eject 0xEC00
#define KEY_XF86AudioRaiseVolume 0xED00
#define KEY_XF86AudioLowerVolume 0xEE00
#define KEY_XF86AudioMute 0xEF00
#define KEY_XF86WWW 0xF000
#define KEY_XF86Back 0xF100
#define KEY_XF86Forward 0xF200
#define KEY_CANCEL 0xF300
#define KEY_FIND 0xF400
#define KEY_XF86ScrollUp 0xF500
#define KEY_XF86ScrollDown 0xF600
#define KEY_NOSYMBOL 0xF700
#define KEY_XF86Sleep 0xF800
#define KEY_XF86ScreenSaver 0xF900
#define KEY_XF86Reload 0xFA00
#define KEY_XF86Calculator 0xFB00
#define KEY_NOSYMBOL 0xFC00
#define KEY_NOSYMBOL 0xFD00
#define KEY_NOSYMBOL 0xFE00
#define KEY_NOSYMBOL 0xFF00


/* ---------- clicks ----------- */
#define CLICK_PRIMARY    0x8142
#define CLICK_SECONDARY  0x8242
#define CLICK_WHEEL      0x8442
#define CLICK_NONE       0x0142

/* ---------- weird extra keys ----------- */
#define KEY_NONE 0
#define KEY_DISABLED 0xFF5F

#define SCROLL_UP 0x0143 /* mouse button 4 */
/* 01 - 7F = scroll speed */
#define SCROLL_DN 0xFF43 /* mouse button 5 */
/* 81 - FF = scroll speed */



#define DPI_DOWN 0x004A /* mouse internal change */
#define DPI_UP   0x014A /* mouse internal change */
/* 0x024A is a mystery */
#define DPI_CYCL 0x034A /* mouse internal change */
#define PRO_DOWN 0x044A /* mouse internal change */
#define PRO_UP   0x054A /* mouse internal change */
/* 0x064A, also mysterious */
#define PRO_CYCL 0x074A /* mouse internal change */
/* what happened to 8 and 9? */
#define SNIPER 0x104A /* mouse internal change */

#define SCROLL_LF 0xC04A /* no-op */
#define SCROLL_RT 0xC14A /* no-op */

#define KEY_OTG 0x0046 /* no-op. what is on-the-go? */



#define KEY_OPENPLAYER 0x8321 /* XF86Tools */

#define KEY_EMAIL 0x8A21

#define KEY_CALC 0x9221 /* XF86Mail */

#define KEY_WWW_SRC 0x2122 /* XF86Search */
/* 0x2222 is a no-op. not even nosymbol, but no event generated */
#define KEY_WWW_HME 0x2322 /* XF86HomePage */
#define KEY_WWW_BCK 0x2422 /* XF86Back */
#define KEY_WWW_FWD 0x2522 /* XF86Forward */
#define KEY_WWW_STP 0x2622 /* Cancel */
#define KEY_WWW_RFR 0x2722 /* XF86Reload */

#define KEY_WWW_FAV 0x2A22 /* XF86Favorites */




#define KEY_IE_BACK 0x8842 /* X11 button 8 */
#define KEY_IE_FWD  0x9042 /* X11 button 9 */


#define KEY_MY_COMP 0x9421 /* XF86Explorer */


#define KEY_NEXT_TRACK 0xB520 /* XF86AudioNext */
#define KEY_PREV_TRACK 0xB620 /* XF86AudioPrev */
#define KEY_STOP 0xB720 /* XF86AudioStop */

#define PLAY_PAUSE 0xCD20 /* XF86AudioPlay */

#define MUTE 0xE220 /* XF86AudioMute */

#define VOL_UP 0xE920 /* XF86AudioRaiseVolume */
#define VOL_DN 0xEA20 /* XF86AudioLowerVolume */


/* ----- modifier keys for when LSB is 0x00 ------ */
#define MOD_CTRL  (1 << 0)
#define MOD_SHIFT (1 << 1)
#define MOD_ALT   (1 << 2)
#define MOD_SUP   (1 << 3)

/* --------
	end keys 
   -------- */

#endif

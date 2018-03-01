#include "lib/utf8decode.h"
#include "lib/graphics.h"

#include "terminal-font.h"

static uint16_t char_width     = 9;    /* Width of a cell in pixels */
static uint16_t char_height    = 20;   /* Height of a cell in pixels */

static uint32_t ununicode(uint32_t c) {
	switch (c) {
		case L'☺': return 1;
		case L'☻': return 2;
		case L'♥': return 3;
		case L'♦': return 4;
		case L'♣': return 5;
		case L'♠': return 6;
		case L'•': return 7;
		case L'◘': return 8;
		case L'○': return 9;
		case L'◙': return 10;
		case L'♂': return 11;
		case L'♀': return 12;
		case L'♪': return 13;
		case L'♫': return 14;
		case L'☼': return 15;
		case L'►': return 16;
		case L'◄': return 17;
		case L'↕': return 18;
		case L'‼': return 19;
		case L'¶': return 20;
		case L'§': return 21;
		case L'▬': return 22;
		case L'↨': return 23;
		case L'↑': return 24;
		case L'↓': return 25;
		case L'→': return 26;
		case L'←': return 27;
		case L'∟': return 28;
		case L'↔': return 29;
		case L'▲': return 30;
		case L'▼': return 31;
				   /* ASCII text */
		case L'⌂': return 127;
		case L'Ç': return 128;
		case L'ü': return 129;
		case L'é': return 130;
		case L'â': return 131;
		case L'ä': return 132;
		case L'à': return 133;
		case L'å': return 134;
		case L'ç': return 135;
		case L'ê': return 136;
		case L'ë': return 137;
		case L'è': return 138;
		case L'ï': return 139;
		case L'î': return 140;
		case L'ì': return 141;
		case L'Ä': return 142;
		case L'Å': return 143;
		case L'É': return 144;
		case L'æ': return 145;
		case L'Æ': return 146;
		case L'ô': return 147;
		case L'ö': return 148;
		case L'ò': return 149;
		case L'û': return 150;
		case L'ù': return 151;
		case L'ÿ': return 152;
		case L'Ö': return 153;
		case L'Ü': return 154;
		case L'¢': return 155;
		case L'£': return 156;
		case L'¥': return 157;
		case L'₧': return 158;
		case L'ƒ': return 159;
		case L'á': return 160;
		case L'í': return 161;
		case L'ó': return 162;
		case L'ú': return 163;
		case L'ñ': return 164;
		case L'Ñ': return 165;
		case L'ª': return 166;
		case L'º': return 167;
		case L'¿': return 168;
		case L'⌐': return 169;
		case L'¬': return 170;
		case L'½': return 171;
		case L'¼': return 172;
		case L'¡': return 173;
		case L'«': return 174;
		case L'»': return 175;
		case L'░': return 176;
		case L'▒': return 177;
		case L'▓': return 178;
		case L'│': return 179;
		case L'┤': return 180;
		case L'╡': return 181;
		case L'╢': return 182;
		case L'╖': return 183;
		case L'╕': return 184;
		case L'╣': return 185;
		case L'║': return 186;
		case L'╗': return 187;
		case L'╝': return 188;
		case L'╜': return 189;
		case L'╛': return 190;
		case L'┐': return 191;
		case L'└': return 192;
		case L'┴': return 193;
		case L'┬': return 194;
		case L'├': return 195;
		case L'─': return 196;
		case L'┼': return 197;
		case L'╞': return 198;
		case L'╟': return 199;
		case L'╚': return 200;
		case L'╔': return 201;
		case L'╩': return 202;
		case L'╦': return 203;
		case L'╠': return 204;
		case L'═': return 205;
		case L'╬': return 206;
		case L'╧': return 207;
		case L'╨': return 208;
		case L'╤': return 209;
		case L'╥': return 210;
		case L'╙': return 211;
		case L'╘': return 212;
		case L'╒': return 213;
		case L'╓': return 214;
		case L'╫': return 215;
		case L'╪': return 216;
		case L'┘': return 217;
		case L'┌': return 218;
		case L'█': return 219;
		case L'▄': return 220;
		case L'▌': return 221;
		case L'▐': return 222;
		case L'▀': return 223;
		case L'α': return 224;
		case L'ß': return 225;
		case L'Γ': return 226;
		case L'π': return 227;
		case L'Σ': return 228;
		case L'σ': return 229;
		case L'µ': return 230;
		case L'τ': return 231;
		case L'Φ': return 232;
		case L'Θ': return 233;
		case L'Ω': return 234;
		case L'δ': return 235;
		case L'∞': return 236;
		case L'φ': return 237;
		case L'ε': return 238;
		case L'∩': return 239;
		case L'≡': return 240;
		case L'±': return 241;
		case L'≥': return 242;
		case L'≤': return 243;
		case L'⌠': return 244;
		case L'⌡': return 245;
		case L'÷': return 246;
		case L'≈': return 247;
		case L'°': return 248;
		case L'∙': return 249;
		case L'·': return 250;
		case L'√': return 251;
		case L'ⁿ': return 252;
		case L'²': return 253;
		case L'■': return 254;
	}
	return 4;
}

void draw_string(gfx_context_t * ctx, int x, int y, uint32_t _fg, char * str) {
	static uint32_t codepoint = 0;
	static uint32_t unicode_state = 0;

	while (*str) {
		if (!decode(&unicode_state, &codepoint, (uint8_t)*str)) {
			int val = codepoint;
			if (val > 128) val = ununicode(val);
			uint16_t * c = large_font[val];
			for (uint8_t i = 0; i < char_height; ++i) {
				for (uint8_t j = 0; j < char_width; ++j) {
					if (c[i] & (1 << (15-j))) {
						GFX(ctx, x+j, y+i) = _fg;
					}
				}
			}
			x += char_width;
			unicode_state = 0;
			codepoint = 0;
		} else if (unicode_state == UTF8_REJECT) {
			break;
			unicode_state = 0;
			codepoint = 0;
		}
		str++;
	}
}

int draw_string_width(char * str) {
	uint32_t codepoint = 0;
	uint32_t unicode_state = 0;

	int x = 0;

	while (*str) {
		if (!decode(&unicode_state, &codepoint, (uint8_t)*str)) {
			x += char_width;
			unicode_state = 0;
			codepoint = 0;
		} else if (unicode_state == UTF8_REJECT) {
			break;
			unicode_state = 0;
			codepoint = 0;
		}
		str++;
	}
	return x;
}


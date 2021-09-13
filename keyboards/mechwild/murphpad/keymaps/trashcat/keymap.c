/* Copyright 2021 Kyle McCreery
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include QMK_KEYBOARD_H

extern MidiDevice midi_device;

// Defines names for use in layer keycodes and the keymap
enum layer_names {
  _BASE,
  _FN1,
	_FN2,
	_FN3
};

enum midi_cc_keycodes_LTRM { MIDI_CC1 = SAFE_RANGE, MIDI_CC2, MIDI_CC3, MIDI_CC4, MIDI_CC5, MIDI_CC6, MIDI_CC7, MIDI_CC8, MIDI_CC9, MIDI_CC10, MIDI_CC11, MIDI_CC12, MIDI_CC13, MIDI_CC14, MIDI_CC15, MIDI_CC16, MIDI_CC17, MIDI_CC18, MIDI_CC19, MIDI_CC20, MIDI_CC21, MIDI_CC22, MIDI_CC23, MIDI_CC24, MIDI_CC25, MIDI_CC26 };

static char current_alpha_oled = '\0';
static char current_ltrm_alpha_oled = '\0';

static uint8_t current_MIDI_ccNumber         = 1;
static char    current_MIDI_ccNumber_char[3] = {'0', '1', '\0'};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* Base */
    [_BASE] = LAYOUT(
                  KC_F1,   KC_F2,   KC_F3,   KC_F4,
				          KC_NLCK, KC_PSLS, KC_PAST, KC_PMNS,
                  KC_P7,   KC_P8,   KC_P9,   KC_PPLS,
        KC_MUTE,  KC_P4,   KC_P5,   KC_P6,   _______,
        MO(_FN1), KC_P1,   KC_P2,   KC_P3,   KC_PENT,
        KC_BSPC,  KC_P0,   _______, KC_PDOT, _______,

				          KC_F5,   KC_F6,   KC_F7

    ),
    [_FN1] = LAYOUT(
                 _______,  _______, _______, _______,
                 _______,  _______, _______, _______,
                 RGB_HUD,  RGB_SPI, RGB_HUI, _______,
        _______, RGB_RMOD, RGB_TOG, RGB_MOD, _______,
        _______, RGB_VAD,  RGB_SPD, RGB_VAI, _______,
        _______, RGB_SAD,  _______, RGB_SAI, _______,

                 _______,  _______, _______

    ),
    [_FN2] = LAYOUT(
                  _______, _______, _______, _______,
                  _______, _______, _______, _______,
                  _______, _______, _______, _______,
        _______,  _______, _______, _______, _______,
        _______,  _______, _______, _______, _______,
        _______,  _______, _______, _______, _______,

                  _______, _______, _______

    ),
	  [_FN3] = LAYOUT(
                  _______, _______, _______, _______,
                  _______, _______, _______, _______,
                  _______, _______, _______, _______,
        _______,  _______, _______, _______, _______,
        _______,  _______, _______, _______, _______,
        _______,  _______, _______, _______, _______,

                  _______, _______, _______

    )
};

#ifdef ENCODER_ENABLE
bool encoder_update_user(uint8_t index, bool clockwise) {
    switch (index) {
        case _BASE:
            if (clockwise) {
                tap_code(KC_VOLU);
            } else {
                tap_code(KC_VOLD);
            }
        break;
		case _FN2:
            if (clockwise) {
                midi_send_cc(&midi_device, 0, current_MIDI_ccNumber, 65);
            } else {
                midi_send_cc(&midi_device, 0, current_MIDI_ccNumber, 63);
            }
        break;
    }
    return true;
}
#endif

#ifdef OLED_ENABLE
oled_rotation_t oled_init_user(oled_rotation_t rotation) {
        return OLED_ROTATION_270;       // flips the display 270 degrees
}
	static void render_logo(void) {     // Render MechWild "MW" Logo
		static const char PROGMEM logo_1[] = {0x8A, 0x8B, 0x8C, 0x8D, 0x00};
		static const char PROGMEM logo_2[] = {0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0x00};
		static const char PROGMEM logo_3[] = {0xCA, 0xCB, 0xCC, 0xCD, 0x00};
		static const char PROGMEM logo_4[] = {0x20, 0x8E, 0x8F, 0x90, 0x00};
		oled_set_cursor(0,0);
		oled_write_P(logo_1, false);
		oled_set_cursor(0,1);
		oled_write_P(logo_2, false);
		oled_set_cursor(0,2);
		oled_write_P(logo_3, false);
		oled_set_cursor(0,3);
		oled_write_P(logo_4, false);
	}

	void oled_task_user(void) {
		render_logo();
		oled_set_cursor(0,6);

		oled_write_ln_P(PSTR("Layer"), false);

    switch (get_highest_layer(layer_state)) {
        case _BASE:
            oled_write_ln_P(PSTR("Base"), false);
            break;
        case _FN1:
            oled_write_ln_P(PSTR("FN 1"), false);
            break;
        case _FN2:
            oled_write_ln_P(PSTR("LTRM 1"), false);
            break;
        case _FN3:
            oled_write_ln_P(PSTR("FN 3"), false);
            break;
        default:
            oled_write_ln_P(PSTR("Undef"), false);
    }
	oled_write_ln_P(PSTR(""), false);
    // Host Keyboard LED Status
    led_t led_state = host_keyboard_led_state();
    oled_write_ln_P(led_state.num_lock ? PSTR("NUM ") : PSTR("    "), false);
    oled_write_ln_P(led_state.caps_lock ? PSTR("CAP ") : PSTR("    "), false);
    oled_write_ln_P(led_state.scroll_lock ? PSTR("SCR ") : PSTR("    "), false);
	}
#endif

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case MIDI_CC1:
            if (record->event.pressed) {
                current_MIDI_ccNumber         = 1;
                current_ltrm_alpha_oled       = 'Q';
                current_MIDI_ccNumber_char[0] = '0';
                current_MIDI_ccNumber_char[1] = '1';
            } else {
            }
            return false;
            break;
        case MIDI_CC2:
            if (record->event.pressed) {
                current_MIDI_ccNumber         = 2;
                current_ltrm_alpha_oled       = 'W';
                current_MIDI_ccNumber_char[0] = '0';
                current_MIDI_ccNumber_char[1] = '2';
            } else {
            }
            return false;
            break;
        case MIDI_CC3:
            if (record->event.pressed) {
                current_MIDI_ccNumber         = 3;
                current_ltrm_alpha_oled       = 'E';
                current_MIDI_ccNumber_char[0] = '0';
                current_MIDI_ccNumber_char[1] = '3';
            } else {
            }
            return false;
            break;
        case MIDI_CC4:
            if (record->event.pressed) {
                current_MIDI_ccNumber         = 4;
                current_ltrm_alpha_oled       = 'R';
                current_MIDI_ccNumber_char[0] = '0';
                current_MIDI_ccNumber_char[1] = '4';
            } else {
            }
            return false;
            break;
        case MIDI_CC5:
            if (record->event.pressed) {
                current_MIDI_ccNumber         = 5;
                current_ltrm_alpha_oled       = 'T';
                current_MIDI_ccNumber_char[0] = '0';
                current_MIDI_ccNumber_char[1] = '5';
            } else {
            }
            return false;
            break;
        case MIDI_CC6:
            if (record->event.pressed) {
                current_MIDI_ccNumber         = 6;
                current_ltrm_alpha_oled       = 'Y';
                current_MIDI_ccNumber_char[0] = '0';
                current_MIDI_ccNumber_char[1] = '6';
            } else {
            }
            return false;
            break;
        case MIDI_CC7:
            if (record->event.pressed) {
                current_MIDI_ccNumber         = 7;
                current_ltrm_alpha_oled       = 'U';
                current_MIDI_ccNumber_char[0] = '0';
                current_MIDI_ccNumber_char[1] = '7';
            } else {
            }
            return false;
            break;
        case MIDI_CC8:
            if (record->event.pressed) {
                current_MIDI_ccNumber         = 8;
                current_ltrm_alpha_oled       = 'I';
                current_MIDI_ccNumber_char[0] = '0';
                current_MIDI_ccNumber_char[1] = '8';
            } else {
            }
            return false;
            break;
        case MIDI_CC9:
            if (record->event.pressed) {
                current_MIDI_ccNumber         = 9;
                current_ltrm_alpha_oled       = 'O';
                current_MIDI_ccNumber_char[0] = '0';
                current_MIDI_ccNumber_char[1] = '9';
            } else {
            }
            return false;
            break;
        case MIDI_CC10:
            if (record->event.pressed) {
                current_MIDI_ccNumber         = 10;
                current_ltrm_alpha_oled       = 'P';
                current_MIDI_ccNumber_char[0] = '1';
                current_MIDI_ccNumber_char[1] = '0';
            } else {
            }
            return false;
            break;
        case MIDI_CC11:
            if (record->event.pressed) {
                current_MIDI_ccNumber         = 11;
                current_ltrm_alpha_oled       = 'A';
                current_MIDI_ccNumber_char[0] = '1';
                current_MIDI_ccNumber_char[1] = '1';
            } else {
            }
            return false;
            break;
        case MIDI_CC12:
            if (record->event.pressed) {
                current_MIDI_ccNumber         = 12;
                current_ltrm_alpha_oled       = 'S';
                current_MIDI_ccNumber_char[0] = '1';
                current_MIDI_ccNumber_char[1] = '2';
            } else {
            }
            return false;
            break;
        case MIDI_CC13:
            if (record->event.pressed) {
                current_MIDI_ccNumber         = 13;
                current_ltrm_alpha_oled       = 'D';
                current_MIDI_ccNumber_char[0] = '1';
                current_MIDI_ccNumber_char[1] = '3';
            } else {
            }
            return false;
            break;
        case MIDI_CC14:
            if (record->event.pressed) {
                current_MIDI_ccNumber         = 14;
                current_ltrm_alpha_oled       = 'F';
                current_MIDI_ccNumber_char[0] = '1';
                current_MIDI_ccNumber_char[1] = '4';
            } else {
            }
            return false;
            break;
        case MIDI_CC15:
            if (record->event.pressed) {
                current_MIDI_ccNumber         = 15;
                current_ltrm_alpha_oled       = 'G';
                current_MIDI_ccNumber_char[0] = '1';
                current_MIDI_ccNumber_char[1] = '5';
            } else {
            }
            return false;
            break;
        case MIDI_CC16:
            if (record->event.pressed) {
                current_MIDI_ccNumber         = 16;
                current_ltrm_alpha_oled       = 'H';
                current_MIDI_ccNumber_char[0] = '1';
                current_MIDI_ccNumber_char[1] = '6';
            } else {
            }
            return false;
            break;
        case MIDI_CC17:
            if (record->event.pressed) {
                current_MIDI_ccNumber         = 17;
                current_ltrm_alpha_oled       = 'J';
                current_MIDI_ccNumber_char[0] = '1';
                current_MIDI_ccNumber_char[1] = '7';
            } else {
            }
            return false;
            break;
        case MIDI_CC18:
            if (record->event.pressed) {
                current_MIDI_ccNumber         = 18;
                current_ltrm_alpha_oled       = 'K';
                current_MIDI_ccNumber_char[0] = '1';
                current_MIDI_ccNumber_char[1] = '8';
            } else {
            }
            return false;
            break;
        case MIDI_CC19:
            if (record->event.pressed) {
                current_MIDI_ccNumber         = 19;
                current_ltrm_alpha_oled       = 'L';
                current_MIDI_ccNumber_char[0] = '1';
                current_MIDI_ccNumber_char[1] = '9';
            } else {
            }
            return false;
            break;
        case MIDI_CC20:
            if (record->event.pressed) {
                current_MIDI_ccNumber         = 20;
                current_ltrm_alpha_oled       = 'Z';
                current_MIDI_ccNumber_char[0] = '2';
                current_MIDI_ccNumber_char[1] = '0';
            } else {
            }
            return false;
            break;
        case MIDI_CC21:
            if (record->event.pressed) {
                current_MIDI_ccNumber         = 21;
                current_ltrm_alpha_oled       = 'X';
                current_MIDI_ccNumber_char[0] = '2';
                current_MIDI_ccNumber_char[1] = '1';
            } else {
            }
            return false;
            break;
        case MIDI_CC22:
            if (record->event.pressed) {
                current_MIDI_ccNumber         = 22;
                current_ltrm_alpha_oled       = 'C';
                current_MIDI_ccNumber_char[0] = '2';
                current_MIDI_ccNumber_char[1] = '2';
            } else {
            }
            return false;
            break;
        case MIDI_CC23:
            if (record->event.pressed) {
                current_MIDI_ccNumber         = 23;
                current_ltrm_alpha_oled       = 'V';
                current_MIDI_ccNumber_char[0] = '2';
                current_MIDI_ccNumber_char[1] = '3';
            } else {
            }
            return false;
            break;
        case MIDI_CC24:
            if (record->event.pressed) {
                current_MIDI_ccNumber         = 24;
                current_ltrm_alpha_oled       = 'B';
                current_MIDI_ccNumber_char[0] = '2';
                current_MIDI_ccNumber_char[1] = '4';
            } else {
            }
            return false;
            break;
        case MIDI_CC25:
            if (record->event.pressed) {
                current_MIDI_ccNumber         = 25;
                current_ltrm_alpha_oled       = 'N';
                current_MIDI_ccNumber_char[0] = '2';
                current_MIDI_ccNumber_char[1] = '5';
            } else {
            }
            return false;
            break;
        case MIDI_CC26:
            if (record->event.pressed) {
                current_MIDI_ccNumber         = 26;
                current_ltrm_alpha_oled       = 'M';
                current_MIDI_ccNumber_char[0] = '2';
                current_MIDI_ccNumber_char[1] = '6';
            } else {
            }
            break;
        case KC_A:
            if (record->event.pressed) {
                current_alpha_oled = 'A';
            } else {
            }
            break;
        case KC_B:
            if (record->event.pressed) {
                current_alpha_oled = 'B';
            } else {
            }
            break;
        case KC_C:
            if (record->event.pressed) {
                current_alpha_oled = 'C';
            } else {
            }
            break;
        case KC_D:
            if (record->event.pressed) {
                current_alpha_oled = 'D';
            } else {
            }
            break;
        case KC_E:
            if (record->event.pressed) {
                current_alpha_oled = 'E';
            } else {
            }
            break;
        case KC_F:
            if (record->event.pressed) {
                current_alpha_oled = 'F';
            } else {
            }
            break;
        case KC_G:
            if (record->event.pressed) {
                current_alpha_oled = 'G';
            } else {
            }
            break;
        case KC_H:
            if (record->event.pressed) {
                current_alpha_oled = 'H';
            } else {
            }
            break;
        case KC_I:
            if (record->event.pressed) {
                current_alpha_oled = 'I';
            } else {
            }
            break;
        case KC_J:
            if (record->event.pressed) {
                current_alpha_oled = 'J';
            } else {
            }
            break;
        case KC_K:
            if (record->event.pressed) {
                current_alpha_oled = 'K';
            } else {
            }
            break;
        case KC_L:
            if (record->event.pressed) {
                current_alpha_oled = 'L';
            } else {
            }
            break;
        case KC_M:
            if (record->event.pressed) {
                current_alpha_oled = 'M';
            } else {
            }
            break;
        case KC_N:
            if (record->event.pressed) {
                current_alpha_oled = 'N';
            } else {
            }
            break;
        case KC_O:
            if (record->event.pressed) {
                current_alpha_oled = 'O';
            } else {
            }
            break;
        case KC_P:
            if (record->event.pressed) {
                current_alpha_oled = 'P';
            } else {
            }
            break;
        case KC_Q:
            if (record->event.pressed) {
                current_alpha_oled = 'Q';
            } else {
            }
            break;
        case KC_R:
            if (record->event.pressed) {
                current_alpha_oled = 'R';
            } else {
            }
            break;
        case KC_S:
            if (record->event.pressed) {
                current_alpha_oled = 'S';
            } else {
            }
            break;
        case KC_T:
            if (record->event.pressed) {
                current_alpha_oled = 'T';
            } else {
            }
            break;
        case KC_U:
            if (record->event.pressed) {
                current_alpha_oled = 'U';
            } else {
            }
            break;
        case KC_V:
            if (record->event.pressed) {
                current_alpha_oled = 'V';
            } else {
            }
            break;
        case KC_W:
            if (record->event.pressed) {
                current_alpha_oled = 'W';
            } else {
            }
            break;
        case KC_X:
            if (record->event.pressed) {
                current_alpha_oled = 'X';
            } else {
            }
            break;
        case KC_Y:
            if (record->event.pressed) {
                current_alpha_oled = 'Y';
            } else {
            }
            break;
        case KC_Z:
            if (record->event.pressed) {
                current_alpha_oled = 'Z';
            } else {
            }
            break;
        default:
            break;
    }
    return true;
}

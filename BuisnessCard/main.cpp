#include "Wch_arduino_compat.h"

constexpr uint8_t RowCount = 4;
constexpr uint8_t ColumnCount = 12;

constexpr uint8_t MainRowsCount = 3;   /* rows 0..2 = typed keys / shift */
constexpr uint8_t ModRowIndex = 3;     /* row 3 = modifiers/layer/space/arrows */

constexpr uint8_t ShiftRow = 2;
constexpr uint8_t ShiftColumn = 0;

/* Tunable constants - PLACEHOLDERS, sweep once real ADC data is flowing
 * over Serial */
constexpr uint32_t DischargeTimeUs = 5;
constexpr uint32_t RowSettleTimeUs = 2;
constexpr int16_t  TouchEnterThreshold = 80;
constexpr int16_t  TouchExitThreshold = 40;
constexpr int32_t  DominanceMarginNum = 3; /* winner must be >= 1.5x    */
constexpr int32_t  DominanceMarginDen = 2; /* strongest neighbor delta */
constexpr uint8_t  BaselineAlphaShift = 4; /* baseline update = 1/16 per pass */

constexpr uint8_t rowPins[RowCount] = { PB9, PB8, PB17, PB16 };

constexpr uint8_t columnPins[ColumnCount] = {
    PA4, PA5, PA6, PA0, PA1, PA2, PA3, PA15, PA14, PA13, PA12, PA7
};

constexpr uint8_t columnAdcChannels[ColumnCount] = {
    AIN0, AIN1, AIN10, AIN9, AIN8, AIN7,
    AIN6, AIN5, AIN4,  AIN3, AIN2, AIN11
};

/* ---------------------------------------------------------------------
 * USB HID Usage IDs (standard Keyboard/Keypad usage page

 
 * ------------------------------------------------------------------- */
constexpr uint8_t KEY_A = 0x04, KEY_B = 0x05, KEY_C = 0x06, KEY_D = 0x07,
                   KEY_E = 0x08, KEY_F = 0x09, KEY_G = 0x0A, KEY_H = 0x0B,
                   KEY_I = 0x0C, KEY_J = 0x0D, KEY_K = 0x0E, KEY_L = 0x0F,
                   KEY_M = 0x10, KEY_N = 0x11, KEY_O = 0x12, KEY_P = 0x13,
                   KEY_Q = 0x14, KEY_R = 0x15, KEY_S = 0x16, KEY_T = 0x17,
                   KEY_U = 0x18, KEY_V = 0x19, KEY_W = 0x1A, KEY_X = 0x1B,
                   KEY_Y = 0x1C, KEY_Z = 0x1D;

constexpr uint8_t KEY_1 = 0x1E, KEY_2 = 0x1F, KEY_3 = 0x20, KEY_4 = 0x21,
                   KEY_5 = 0x22, KEY_6 = 0x23, KEY_7 = 0x24, KEY_8 = 0x25,
                   KEY_9 = 0x26, KEY_0 = 0x27;

constexpr uint8_t KEY_ENTER = 0x28, KEY_ESC = 0x29, KEY_BACKSPACE = 0x2A,
                   KEY_TAB = 0x2B, KEY_SPACE = 0x2C, KEY_MINUS = 0x2D,
                   KEY_EQUAL = 0x2E, KEY_LEFTBRACE = 0x2F, KEY_RIGHTBRACE = 0x30,
                   KEY_BACKSLASH = 0x31, KEY_SEMICOLON = 0x33,
                   KEY_APOSTROPHE = 0x34, KEY_GRAVE = 0x35, KEY_COMMA = 0x36,
                   KEY_PERIOD = 0x37, KEY_SLASH = 0x38, KEY_DELETE = 0x4C;

constexpr uint8_t KEY_F1 = 0x3A, KEY_F2 = 0x3B, KEY_F3 = 0x3C, KEY_F4 = 0x3D,
                   KEY_F5 = 0x3E, KEY_F6 = 0x3F, KEY_F7 = 0x40, KEY_F8 = 0x41,
                   KEY_F9 = 0x42, KEY_F10 = 0x43, KEY_F11 = 0x44, KEY_F12 = 0x45;

constexpr uint8_t KEY_HOME = 0x4A, KEY_PAGEUP = 0x4B, KEY_PAGEDOWN = 0x4E,
                   KEY_END = 0x4D, KEY_RIGHT = 0x4F, KEY_LEFT = 0x50,
                   KEY_DOWN = 0x51, KEY_UP = 0x52;

constexpr uint8_t MOD_LCTRL = 0x01, MOD_LSHIFT = 0x02, MOD_LALT = 0x04,
                   MOD_LGUI = 0x08;

/* ---------------------------------------------------------------------
 * Keymap data structures
 * ------------------------------------------------------------------- */
struct KeymapEntry {
    uint8_t keycode;
    bool    forceShift; /* true = OR in MOD_LSHIFT when this key is sent,
                          * because HID has no raw keycode for symbols like
                          * '_' or '+' - they're Shift+Minus / Shift+Equal */
};

enum ActiveLayer { LAYER_BASE, LAYER_RAISE, LAYER_LOWER };

/* Row 2, column 0 is Shift - handled as its own dedicated pad (see
 * CheckShiftPressed), so its keymap slot below is unused ({0,false}). */

constexpr KeymapEntry BaseKeymap[MainRowsCount][ColumnCount] = {
    /* row0: esc  q      w      e      r      t      y      u      i      o      p      backspace */
    { {KEY_ESC,false},{KEY_Q,false},{KEY_W,false},{KEY_E,false},{KEY_R,false},{KEY_T,false},
      {KEY_Y,false},{KEY_U,false},{KEY_I,false},{KEY_O,false},{KEY_P,false},{KEY_BACKSPACE,false} },
    /* row1: tab  a      s      d      f      g      h      j      k      l      ;      ' */
    { {KEY_TAB,false},{KEY_A,false},{KEY_S,false},{KEY_D,false},{KEY_F,false},{KEY_G,false},
      {KEY_H,false},{KEY_J,false},{KEY_K,false},{KEY_L,false},{KEY_SEMICOLON,false},{KEY_APOSTROPHE,false} },
    /* row2: [shift] z      x      c      v      b      n      m      ,      .      up     enter */
    { {0,false},{KEY_Z,false},{KEY_X,false},{KEY_C,false},{KEY_V,false},{KEY_B,false},
      {KEY_N,false},{KEY_M,false},{KEY_COMMA,false},{KEY_PERIOD,false},{KEY_UP,false},{KEY_ENTER,false} },
};

constexpr KeymapEntry RaiseKeymap[MainRowsCount][ColumnCount] = {
    /* row0: `    1    2    3    4    5    6    7    8    9    0    backspace */
    { {KEY_GRAVE,false},{KEY_1,false},{KEY_2,false},{KEY_3,false},{KEY_4,false},{KEY_5,false},
      {KEY_6,false},{KEY_7,false},{KEY_8,false},{KEY_9,false},{KEY_0,false},{KEY_BACKSPACE,false} },
    /* row1: del  f1   f2   f3   f4   f5   f6   _        +         [      ]      \  */
    { {KEY_DELETE,false},{KEY_F1,false},{KEY_F2,false},{KEY_F3,false},{KEY_F4,false},{KEY_F5,false},
      {KEY_F6,false},{KEY_MINUS,true},{KEY_EQUAL,true},{KEY_LEFTBRACE,false},{KEY_RIGHTBRACE,false},{KEY_BACKSLASH,false} },
    /* row2: [shift] f7   f8   f9   f10  f11  f12  \        ?         pgup   pgdn   enter */
    { {0,false},{KEY_F7,false},{KEY_F8,false},{KEY_F9,false},{KEY_F10,false},{KEY_F11,false},
      {KEY_F12,false},{KEY_BACKSLASH,false},{KEY_SLASH,true},{KEY_PAGEUP,false},{KEY_PAGEDOWN,false},{KEY_ENTER,false} },
};

constexpr KeymapEntry LowerKeymap[MainRowsCount][ColumnCount] = {
    /* row0: ~        !        @        #        $        %        ^        &        *        (        )        backspace */
    { {KEY_GRAVE,true},{KEY_1,true},{KEY_2,true},{KEY_3,true},{KEY_4,true},{KEY_5,true},
      {KEY_6,true},{KEY_7,true},{KEY_8,true},{KEY_9,true},{KEY_0,true},{KEY_BACKSPACE,false} },
    /* row1: del  f1   f2   f3   f4   f5   f6   -         =         {         }         | */
    { {KEY_DELETE,false},{KEY_F1,false},{KEY_F2,false},{KEY_F3,false},{KEY_F4,false},{KEY_F5,false},
      {KEY_F6,false},{KEY_MINUS,false},{KEY_EQUAL,false},{KEY_LEFTBRACE,true},{KEY_RIGHTBRACE,true},{KEY_BACKSLASH,true} },
    /* row2: [shift] f7   f8   f9   f10  f11  f12  <         >         home   end    enter */
    { {0,false},{KEY_F7,false},{KEY_F8,false},{KEY_F9,false},{KEY_F10,false},{KEY_F11,false},
      {KEY_F12,false},{KEY_COMMA,true},{KEY_PERIOD,true},{KEY_HOME,false},{KEY_END,false},{KEY_ENTER,false} },
};

/* Row 3 pad classification - evaluated independently per-pad (NOT a
 * single dominant-peak search), since modifier combos (Ctrl+Alt) and
 * layer+space etc are legitimate simultaneous presses on this row. */
enum Row3PadType {
    ROW3_MOD_CTRL, ROW3_FN_RESERVED, ROW3_MOD_GUI, ROW3_MOD_ALT,
    ROW3_LAYER_LOWER, ROW3_KEY_SPACE, ROW3_KEY_SPACE2, ROW3_LAYER_RAISE,
    ROW3_KEY_SLASH, ROW3_KEY_LEFT, ROW3_KEY_DOWN, ROW3_KEY_RIGHT
};
constexpr Row3PadType Row3Types[ColumnCount] = {
    ROW3_MOD_CTRL, ROW3_FN_RESERVED, ROW3_MOD_GUI, ROW3_MOD_ALT,
    ROW3_LAYER_LOWER, ROW3_KEY_SPACE, ROW3_KEY_SPACE2, ROW3_LAYER_RAISE,
    ROW3_KEY_SLASH, ROW3_KEY_LEFT, ROW3_KEY_DOWN, ROW3_KEY_RIGHT
};

/* ---------------------------------------------------------------------
 * Scan state
 * ------------------------------------------------------------------- */
uint16_t RawValue[RowCount][ColumnCount];
float Baseline[RowCount][ColumnCount];
float DeltaValue[RowCount][ColumnCount];
bool  Pressed[RowCount][ColumnCount];

/* Debug-print change tracking */
int lastKeyRow = -1, lastKeyCol = -1;
int lastActiveLayer = -1;
uint8_t lastModifiers = 0;
bool lastRow3Extra[ColumnCount] = { false };

/* ---------------------------------------------------------------------
 * function declarations
 * ------------------------------------------------------------------- */
void InitializeRows();
void InitializeTouchHardware();
void InitializeBaselines();
void ReadValues();
void UpdateBaselineAndDelta();
void DebounceAll();
float MaxNeighborDelta(uint8_t row, uint8_t column);
bool FindDominantTypedKey(uint8_t *outRow, uint8_t *outColumn);
bool CheckDominantSinglePad(uint8_t row, uint8_t column);
void EvaluateAndPrint();

/* ---------------------------------------------------------------------
 * setup / loop
 * ------------------------------------------------------------------- */
void setup() {
    InitializeRows();
    InitializeTouchHardware();

    // eventually:
    // initialize USB
    // initialize bluetooth if there is no USB comm

    InitializeBaselines();
}

void loop() {
    ReadValues();
    UpdateBaselineAndDelta();
    DebounceAll();
    EvaluateAndPrint();
}

/* ---------------------------------------------------------------------
 * setup helpers
 * ------------------------------------------------------------------- */
void InitializeRows() {
    for (uint8_t row = 0; row < RowCount; row++) {
        pinMode(rowPins[row], OUTPUT);
        digitalWrite(rowPins[row], LOW);
    }
}

void InitializeTouchHardware() {
    for (uint8_t column = 0; column < ColumnCount; column++) {
        setFloatingInput(columnPins[column]);
    }
}

void InitializeBaselines() {
    for (uint8_t i = 0; i < 8; i++) {
        ReadValues();
        for (uint8_t row = 0; row < RowCount; row++) {
            for (uint8_t column = 0; column < ColumnCount; column++) {
                Baseline[row][column] = RawValue[row][column];
            }
        }
    }
}

/* ---------------------------------------------------------------------
 * scan
 * ------------------------------------------------------------------- */
void ReadValues() {
    for (uint8_t otherRow = 0; otherRow < RowCount; otherRow++) {
        digitalWrite(rowPins[otherRow], LOW);
    }

    for (uint8_t row = 0; row < RowCount; row++) {
        for (uint8_t column = 0; column < ColumnCount; column++) {
            pinMode(columnPins[column], OUTPUT);
            digitalWrite(columnPins[column], LOW);
            delayMicroseconds(DischargeTimeUs);

            setFloatingInput(columnPins[column]);

            digitalWrite(rowPins[row], HIGH);
            delayMicroseconds(RowSettleTimeUs);

            RawValue[row][column] = analogRead(columnAdcChannels[column]);

            digitalWrite(rowPins[row], LOW);
        }
    }
}

/* ---------------------------------------------------------------------
 * baseline / delta - frozen while pressed so a held key's baseline
 * doesn't creep toward the pressed value over time
 * ------------------------------------------------------------------- */
void UpdateBaselineAndDelta() {
    for (uint8_t row = 0; row < RowCount; row++) {
        for (uint8_t column = 0; column < ColumnCount; column++) {
            if (!Pressed[row][column]) {
                Baseline[row][column] +=
                    (RawValue[row][column] - Baseline[row][column]) / (1 << BaselineAlphaShift);
            }
            DeltaValue[row][column] = Baseline[row][column] - RawValue[row][column];
        }
    }
}

/* ---------------------------------------------------------------------
 * debounce (per-pad enter/exit hysteresis) - applies uniformly to all
 * 48 pads regardless of what region they belong to
 * ------------------------------------------------------------------- */
void DebounceAll() {
    for (uint8_t row = 0; row < RowCount; row++) {
        for (uint8_t column = 0; column < ColumnCount; column++) {
            if (!Pressed[row][column] && DeltaValue[row][column] > TouchEnterThreshold) {
                Pressed[row][column] = true;
            } else if (Pressed[row][column] && DeltaValue[row][column] < TouchExitThreshold) {
                Pressed[row][column] = false;
            }
        }
    }
}

/* ---------------------------------------------------------------------
 * physical-neighbor dominance check - "neighbor" = adjacent in the
 * physical 4x12 grid (single uniform matrix, no row isolation)
 * ------------------------------------------------------------------- */
float MaxNeighborDelta(uint8_t row, uint8_t column) {
    float maxDelta = 0;
    for (int dr = -1; dr <= 1; dr++) {
        for (int dc = -1; dc <= 1; dc++) {
            if (dr == 0 && dc == 0) continue;
            int nr = row + dr, nc = column + dc;
            if (nr < 0 || nr >= RowCount || nc < 0 || nc >= ColumnCount) continue;
            if (DeltaValue[nr][nc] > maxDelta) maxDelta = DeltaValue[nr][nc];
        }
    }
    return maxDelta;
}

bool CheckDominantSinglePad(uint8_t row, uint8_t column) {
    if (!Pressed[row][column]) return false;
    float neighborMax = MaxNeighborDelta(row, column);
    return DeltaValue[row][column] * DominanceMarginDen >= neighborMax * DominanceMarginNum;
}

/* Single dominant peak among rows 0-2, EXCLUDING the Shift pad (which
 * is checked separately via CheckDominantSinglePad so it doesn't
 * compete with letters for this one "typed key" slot). */
bool FindDominantTypedKey(uint8_t *outRow, uint8_t *outColumn) {
    float bestDelta = 0;
    int bestRow = -1, bestColumn = -1;

    for (uint8_t row = 0; row < MainRowsCount; row++) {
        for (uint8_t column = 0; column < ColumnCount; column++) {
            if (row == ShiftRow && column == ShiftColumn) continue; /* excluded */
            if (!Pressed[row][column]) continue;
            if (DeltaValue[row][column] > bestDelta) {
                bestDelta = DeltaValue[row][column];
                bestRow = row;
                bestColumn = column;
            }
        }
    }

    if (bestRow < 0) return false;

    float neighborMax = MaxNeighborDelta(bestRow, bestColumn);
    if (bestDelta * DominanceMarginDen < neighborMax * DominanceMarginNum) {
        return false; /* not dominant enough - reject as ambiguous */
    }

    *outRow = (uint8_t)bestRow;
    *outColumn = (uint8_t)bestColumn;
    return true;
}

/* ---------------------------------------------------------------------
 * Evaluate full state (typed key + shift + row3 mods/layers/keys),
 * resolve against the active layer's keymap, and print over Serial
 * only when something changes.
 * ------------------------------------------------------------------- */
void EvaluateAndPrint() {
    /* --- row 3: evaluate every pad independently (multi-touch OK) --- */
    bool raiseHeld = false, lowerHeld = false;
    uint8_t modifiers = 0;
    uint8_t row3ExtraKeycodes[ColumnCount];
    uint8_t row3ExtraCount = 0;
    bool row3ExtraNow[ColumnCount] = { false };

    for (uint8_t column = 0; column < ColumnCount; column++) {
        if (!Pressed[ModRowIndex][column]) continue;

        switch (Row3Types[column]) {
            case ROW3_MOD_CTRL:      modifiers |= MOD_LCTRL; break;
            case ROW3_MOD_ALT:       modifiers |= MOD_LALT;  break;
            case ROW3_MOD_GUI:       modifiers |= MOD_LGUI;  break;
            case ROW3_LAYER_LOWER:   lowerHeld = true;        break;
            case ROW3_LAYER_RAISE:   raiseHeld = true;        break;
            case ROW3_KEY_SPACE:
            case ROW3_KEY_SPACE2:    row3ExtraKeycodes[row3ExtraCount++] = KEY_SPACE; row3ExtraNow[column] = true; break;
            case ROW3_KEY_SLASH:     row3ExtraKeycodes[row3ExtraCount++] = KEY_SLASH; row3ExtraNow[column] = true; break;
            case ROW3_KEY_LEFT:      row3ExtraKeycodes[row3ExtraCount++] = KEY_LEFT;  row3ExtraNow[column] = true; break;
            case ROW3_KEY_DOWN:      row3ExtraKeycodes[row3ExtraCount++] = KEY_DOWN;  row3ExtraNow[column] = true; break;
            case ROW3_KEY_RIGHT:     row3ExtraKeycodes[row3ExtraCount++] = KEY_RIGHT; row3ExtraNow[column] = true; break;
            case ROW3_FN_RESERVED:   /* no-op for now */ break;
        }
    }

    /* --- shift: its own dedicated pad, independent of the typed-key search --- */
    bool shiftHeld = CheckDominantSinglePad(ShiftRow, ShiftColumn);
    if (shiftHeld) modifiers |= MOD_LSHIFT;

    /* --- active layer: Raise takes priority if both held (undefined combo) --- */
    ActiveLayer layer = raiseHeld ? LAYER_RAISE : (lowerHeld ? LAYER_LOWER : LAYER_BASE);

    /* --- typed key: single dominant peak in rows 0-2, excluding shift --- */
    uint8_t keyRow = 0, keyCol = 0;
    bool keyHit = FindDominantTypedKey(&keyRow, &keyCol);

    KeymapEntry entry = {0, false};
    if (keyHit) {
        const KeymapEntry (*table)[ColumnCount] =
            (layer == LAYER_RAISE) ? RaiseKeymap : (layer == LAYER_LOWER) ? LowerKeymap : BaseKeymap;
        entry = table[keyRow][keyCol];
        if (entry.forceShift) modifiers |= MOD_LSHIFT;
    }

    /* --- print only on change --- */
    bool keyChanged = (keyHit && (keyRow != lastKeyRow || keyCol != lastKeyCol || (int)layer != lastActiveLayer)) ||
                       (!keyHit && lastKeyRow != -1);
    bool modChanged = modifiers != lastModifiers;
    bool row3Changed = false;
    for (uint8_t c = 0; c < ColumnCount; c++) {
        if (row3ExtraNow[c] != lastRow3Extra[c]) { row3Changed = true; break; }
    }

    if (keyChanged) {
        if (keyHit) {
            Serial.print("KEY  row=");   Serial.print((int)keyRow);
            Serial.print(" col=");       Serial.print((int)keyCol);
            Serial.print(" layer=");     Serial.print((int)layer); /* 0=base 1=raise 2=lower */
            Serial.print(" keycode=0x"); Serial.print((int)entry.keycode);
            Serial.print(" forceShift="); Serial.println(entry.forceShift ? 1 : 0);
        } else {
            Serial.println("KEY  released");
        }
        lastKeyRow = keyHit ? keyRow : -1;
        lastKeyCol = keyHit ? keyCol : -1;
        lastActiveLayer = (int)layer;
    }

    if (modChanged) {
        Serial.print("MOD  byte=0x");
        Serial.println((int)modifiers);
        lastModifiers = modifiers;
    }

    if (row3Changed) {
        for (uint8_t c = 0; c < ColumnCount; c++) {
            if (row3ExtraNow[c] && !lastRow3Extra[c]) {
                Serial.print("ROW3 pressed  col="); Serial.println((int)c);
            } else if (!row3ExtraNow[c] && lastRow3Extra[c]) {
                Serial.print("ROW3 released col="); Serial.println((int)c);
            }
            lastRow3Extra[c] = row3ExtraNow[c];
        }
    }
}
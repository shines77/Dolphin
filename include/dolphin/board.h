
#ifndef _BOARD_H_
#define _BOARD_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Symbolic values for the possible contents of a square */
#define BOARD_SIDE                  8
#define BOARD_ROW                   BOARD_SIDE      // row      行
#define BOARD_COL                   BOARD_SIDE      // column   列
#define BOARD_MAX_DISC              (BOARD_ROW * BOARD_COL)

#define EX_BOARD_SIDE               10
#define EX_BOARD_ROW                EX_BOARD_SIDE   // row      行
#define EX_BOARD_COL                EX_BOARD_SIDE   // column   列
#define EX_BOARD_MAX_DISC           (EX_BOARD_ROW * EX_BOARD_COL)

#define BOARD_ROW_MASKS             (1 << BOARD_COL)

/* Coordinates in 8x8 Field */

enum Coord_8x8 {
    A1=0, B1,   C1,   D1,   E1,   F1,   G1,   H1,
    A2,   B2,   C2,   D2,   E2,   F2,   G2,   H2,
    A3,   B3,   C3,   D3,   E3,   F3,   G3,   H3,
    A4,   B4,   C4,   D4,   E4,   F4,   G4,   H4,
    A5,   B5,   C5,   D5,   E5,   F5,   G5,   H5,
    A6,   B6,   C6,   D6,   E6,   F6,   G6,   H6,
    A7,   B7,   C7,   D7,   E7,   F7,   G7,   H7,
    A8,   B8,   C8,   D8,   E8,   F8,   G8,   H8,
    MAX_COORD
};

#define _NULL_MOVE  MAX_COORD

#ifdef __cplusplus
}
#endif

#endif // _BOARD_H_

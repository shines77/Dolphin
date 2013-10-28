
#ifndef _DOL_COLOUR_H_
#define _DOL_COLOUR_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

/* Symbolic values for the possible contents of a square */
#define CHESS_ILLEGAL               (-1)
#define CHESS_EMPTY                 1
#define CHESS_BLACK                 0
#define CHESS_WHITE                 2
#define CHESS_OUTSIDE               3

#define CHESS_COLORS                2
#define CHESS_MAX_COLOR             3
#define CHESS_MAX_COLOR_EXT         4

#define OPP_COLOR( color )          ((CHESS_BLACK + CHESS_WHITE) - (color))

#endif // _DOL_COLOUR_H_

#ifndef _DOL_SEARCH_ENGINE_H_
#define _DOL_SEARCH_ENGINE_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif

#include <dolphin/dol_stddef.h>
#include <dolphin/colour.h>
#include <dolphin/board.h>

namespace dolphin {

class chess_board {
    uint8_t position[BOARD_ROWS + 2][BOARD_COLS + 2];
};

class chess_board;
class game_evaluator;
class move_generator;
class move_fliplist;

typedef uint32_t  move_pos;
typedef uint8_t   move_pos_short;
typedef uint32_t  move_color;
typedef uint8_t   move_color_short;

class search_engine {
public:
    search_engine( void );
    virtual ~search_engine( void );

public:
    virtual int     search_move(chess_board *board) = 0;

    virtual void    set_search_depth(int nDepth) { m_nSearchDepth = nDepth; };
    virtual void    set_curcolor(int nColor) { m_nCurColor = nColor; };
    virtual void    set_endgame(bool bEndGame) { m_bEndGame = bEndGame; };

    virtual void    set_game_evaluator(game_evaluator *pGameEval) { m_pGameEval = pGameEval; };

    virtual void    set_move_generator(move_generator *pMoveGen) { m_pMoveGen = pMoveGen; };

            void    get_piece_counts(int &nBlack, int &nWhite, int &nEmpty);
            void    get_piece_counts(chess_board *board, int &nBlack, int &nWhite, int &nEmpty);

protected:
            void    copy_board(chess_board *dest, chess_board *src);
    virtual void    make_move(move_fliplist *moveflip, move_color color);
    virtual void    make_move(move_pos movepos, move_color color);
    virtual void    unmake_move(move_fliplist *moveflip, move_color color);
    virtual void    unmake_move(move_pos movepos, move_color color);

public:
    chess_board     m_Board;
    move_color      m_nCurColor;
    int             m_nBestMove;
    move_generator  *m_pMoveGen;
    game_evaluator  *m_pGameEval;
    int             m_nSearchDepth;
    int             m_nMaxDepth;
    int             m_nEvalScore;
    bool            m_bEndGame;
    bool            m_bEndGameFirstStep;
};

}  // namespace dolphin

#endif  /* _DOL_SEARCH_ENGINE_H_ */
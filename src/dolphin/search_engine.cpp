
#include <dolphin/search_engine.h>
#include <dolphin/bitboard.h>
#include <dolphin/my_random.h>

namespace dolphin {

search_engine::search_engine( void ) :
    m_nCurColor(CHESS_EMPTY),
    m_nBestMove(-1),
    m_pMoveGen(NULL),
    m_pGameEval(NULL),
    m_nSearchDepth(0),
    m_nMaxDepth(0),
    m_nEvalScore(0),
    m_bEndGame(false),
    m_bEndGameFirstStep(false)
{

}

search_engine::~search_engine( void )
{

}

void search_engine::make_move( move_fliplist *moveflip, move_color color )
{

}

void search_engine::make_move( move_pos movepos, move_color color )
{

}

void search_engine::unmake_move( move_fliplist *moveflip, move_color color )
{

}

void search_engine::unmake_move( move_pos movepos, move_color color )
{

}

void search_engine::copy_board( chess_board *dest, chess_board *src )
{

}

}  // namespace dolphin

use crate::bitboard::Manipulations;

#[allow(dead_code)]
pub struct Board {
    white_pieces: u64,
    white_knights: u64,
    white_rooks: u64,
    white_bishops: u64,
    white_queens: u64,
    white_pawns: u64,

    black_pieces: u64,
    black_knights: u64,
    black_rootks: u64,
    black_bishops: u64,
}

#[derive(Copy,Clone)]
pub struct Move {
    pub from: u64,
    pub to: u64
}

// returns (from, to) as bitboards
pub fn get_move() -> Move {
    choose_move(&get_legal_moves())
}

fn get_legal_moves() -> Vec::<Move> {
    let (mut from, mut to): (u64, u64) = (0, 0);

    // e4
    from.set_square(4, 1);
    to.set_square(4, 3);

    vec![ Move { from, to } ]
}

fn choose_move(moves: &[Move]) -> Move {
    assert!(!moves.is_empty());
    moves[0]
}


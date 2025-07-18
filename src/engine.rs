use crate::bitboard::{Manipulations,Scans};

#[derive(Default)]
pub struct State {
    white_pieces: u64,
    white_knights: u64,
    white_rooks: u64,
    white_bishops: u64,
    white_queens: u64,
    white_pawns: u64,
    white_king: u64,

    black_pieces: u64,
    black_knights: u64,
    black_rooks: u64,
    black_bishops: u64,
    black_queens: u64,
    black_pawns: u64,
    black_king: u64,
}

#[derive(Copy,Clone)]
pub struct Move {
    pub from: u64,
    pub to: u64
}

pub fn move_from_str(input: &str) -> Move {
    let files: Vec<char> = vec!['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'];
    let ranks: Vec<char> = vec!['1', '2', '3', '4', '5', '6', '7', '8'];
    let chars: Vec<char> = input.chars().collect();

    let mut ret = Move { from: 0, to: 0 };
    assert!(chars.len() >= 4);
    let file = files.iter().position(|&c| c == chars[0]).unwrap();
    let rank = ranks.iter().position(|&c| c == chars[1]).unwrap();
    ret.from.set_square(file as isize, rank as isize);

    let file = files.iter().position(|&c| c == chars[2]).unwrap();
    let rank = ranks.iter().position(|&c| c == chars[3]).unwrap();
    ret.to.set_square(file as isize, rank as isize);

    ret
}

#[derive(Clone,PartialEq)]
pub enum Turn {
    White,
    Black,
}

impl State {
    pub fn new() -> Self {
        Self { ..Default::default() }
    }

    pub fn reset_board(&mut self) {
        // I'm sorry.

        self.white_pieces = 0xffff;
        self.white_knights = 0x42;
        self.white_rooks = 0x81;
        self.white_bishops = 0x24;
        self.white_queens = 0x10;
        self.white_pawns = 0xff00;
        self.white_king = 0x8;

        self.black_pieces = 0xffff_0000_0000_0000;
        self.black_knights = 0x4200_0000_0000_0000;
        self.black_rooks = 0x8100_0000_0000_0000;
        self.black_bishops = 0x2400_0000_0000_0000;
        self.black_queens = 0x1000_0000_0000_0000;
        self.black_pawns = 0x00ff_0000_0000_0000;
        self.black_king = 0x0800_0000_0000_0000;
    }

    // returns (from, to) as bitboards
    pub fn get_move(&self, turn: &Turn) -> Move {
        self.choose_move(&self.get_legal_moves(turn), turn)
    }

    fn get_legal_moves(&self, turn: &Turn) -> Vec::<Move> {
        let mut moves: Vec::<Move> = vec![];

        self.get_knight_moves(&mut moves, turn);
        // Todo:
        // self.get_king_moves(&mut moves, turn);
        // self.get_rook_moves(&mut moves, turn);
        // self.get_bishop_moves(&mut moves, turn);
        // self.get_queen_moves(&mut moves, turn);
        // self.get_pawn_moves(&mut moves, turn);
        //
        // the C++ version did have partial pawn support but En Passant and
        // promotion sound complicated to implement

        moves
    }

    fn get_knight_moves(&self, moves: &mut Vec::<Move>, turn: &Turn) {
        let mut current_piece = match turn {
            Turn::White => self.white_knights,
            Turn::Black => self.black_knights,
        };

        while current_piece != 0 {
            let file = current_piece.get_file() as isize;
            let rank = current_piece.get_rank() as isize;

            for i in 0..2 {
                for j in 0..4 {
                    let file_offset = match i {
                        0 => 2,
                        _ => 1,
                    } * match j / 2 {
                        0 => -1,
                        _ => 1,
                    };

                    let rank_offset = match i {
                        0 => 1,
                        _ => 2,
                    } * match j % 2 {
                        0 => -1,
                        _ => 1,
                    };

                    if (*turn == Turn::White &&
                        self.white_pieces
                            .valid_destination(file + file_offset,
                                               rank + rank_offset)) ||
                       (*turn == Turn::Black &&
                        self.black_pieces
                            .valid_destination(file + file_offset,
                                               rank + rank_offset)) {
                        let mut current = Move { from: 0, to: 0 };
                        current.from.set_square(file, rank);
                        current.to.set_square(file + file_offset,
                                              rank + rank_offset);
                        moves.push(current);
                    }
                }
            }
            current_piece.clear_square(file, rank);
        }
    }

    fn choose_move(&self, moves: &[Move], _turn: &Turn) -> Move {
        assert!(!moves.is_empty());
        moves[0]
    }

    pub fn play_move(&mut self, src: u64, dst: u64, turn: &Turn) {
        // "(Crying)" - Anyone reading this code

        self.white_pieces.play_if_exists(src, dst);
        self.white_knights.play_if_exists(src, dst);
        self.white_rooks.play_if_exists(src, dst);
        self.white_bishops.play_if_exists(src, dst);
        self.white_queens.play_if_exists(src, dst);
        self.white_pawns.play_if_exists(src, dst);
        self.white_king.play_if_exists(src, dst);

        self.black_pieces.play_if_exists(src, dst);
        self.black_knights.play_if_exists(src, dst);
        self.black_rooks.play_if_exists(src, dst);
        self.black_bishops.play_if_exists(src, dst);
        self.black_queens.play_if_exists(src, dst);
        self.black_pawns.play_if_exists(src, dst);
        self.black_king.play_if_exists(src, dst);

        match turn {
            Turn::White => {
                self.black_pieces.take(self.white_pieces);
                self.black_knights.take(self.white_pieces);
                self.black_rooks.take(self.white_pieces);
                self.black_bishops.take(self.white_pieces);
                self.black_queens.take(self.white_pieces);
                self.black_pawns.take(self.white_pieces);
                self.black_king.take(self.white_pieces);
            },
            Turn::Black => {
                self.white_pieces.take(self.black_pieces);
                self.white_knights.take(self.black_pieces);
                self.white_rooks.take(self.black_pieces);
                self.white_bishops.take(self.black_pieces);
                self.white_queens.take(self.black_pieces);
                self.white_pawns.take(self.black_pieces);
                self.white_king.take(self.black_pieces);
            }
        };
    }
}


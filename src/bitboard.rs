pub trait Scans {
    fn most_significant(self) -> usize;
    fn get_file(self) -> usize;
    fn get_rank(self) -> usize;
    fn square_occupied(self, file: usize, rank: usize) -> bool;
}

pub trait Manipulations {
    fn play_move(&mut self, src: u64, dst: u64);
    fn set_square(&mut self, file: usize, rank: usize);
    fn clear_square(&mut self, file: usize, rank: usize);
}

impl Manipulations for u64 {
    fn play_move(&mut self, src: u64, dst: u64) {
        assert!(*self & src != 0);
        assert!(*self | dst != *self);
        *self &= !src;
        *self |= dst;
    }

    fn set_square(&mut self, file: usize, rank: usize) {
        assert!(!self.square_occupied(file, rank));
        *self |= get_location_bit(file, rank);
    }

    fn clear_square(&mut self, file: usize, rank: usize) {
        assert!(self.square_occupied(file, rank));
        *self &= !get_location_bit(file, rank);
    }
}

impl Scans for u64 {
    fn most_significant(mut self) -> usize {
        let mut count = 0;
        while self >> 1 != 0 {
            count += 1;
            self >>= 1;
        }
        count
    }

    fn get_file(self) -> usize {
        self.most_significant() % 8
    }

    fn get_rank(self) -> usize {
        self.most_significant() / 8
    }

    fn square_occupied(self, file: usize, rank: usize) -> bool {
        self & get_location_bit(file, rank) != 0
    }
}

pub fn get_location_bit(file: usize, rank: usize) -> u64 {
    assert!(square_exists(file, rank));
    1 << ((rank * 8) + file)
}

pub fn square_exists(file: usize, rank: usize) -> bool {
    file < 8 && rank < 8
}

pub fn to_move(src: u64, dst: u64) -> String {
    // bad?
    let files: Vec<char> = vec!['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'];
    let ranks: Vec<char> = vec!['1', '2', '3', '4', '5', '6', '7', '8'];
    let chars: Vec<char> = vec![ files[src.get_file()],
                                     ranks[src.get_rank()],
                                     files[dst.get_file()],
                                     ranks[dst.get_rank()] ];

    chars.into_iter().collect()
}


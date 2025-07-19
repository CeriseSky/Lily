use chess::uci;
use chess::uci::Command;
use std::thread;
use std::time::Duration;
use std::sync::mpsc;
use chess::bitboard;
use chess::engine::State;
use chess::engine::move_from_str;
use chess::engine::Turn;

fn main() {
    let (tx, rx) = mpsc::channel::<String>();

    // UCI must always be able to handle messages so it will run on a separate
    // thread from the engine. Limited to a bandwidth of 200 commands a second
    // which should be enough to not timeout any GUI out there
    thread::spawn(move || {
        let mut state = uci::Interface::new("Lily", "CeriseSky", tx);
        while state.is_running() {
            uci::get_command().handle(&mut state);
            thread::sleep(Duration::from_millis(50));
        }
    });

    let mut lily = State::new();
    let mut turn = Turn::White;

    loop {
        let message = rx.recv().unwrap();
        let tokens: Vec<&str> = message.split_whitespace().collect();
        assert!(!tokens.is_empty());
        match tokens[0] {
            "quit" => { break; }
            "full_reset" => {},
            "board_reset" => {
                lily.reset_board();
                turn = Turn::White;
            },
            "move" => {
                assert!(tokens.len() > 1);
                let from_gui = move_from_str(tokens[1]);
                lily.play_move(from_gui.from, from_gui.to, &turn);
                turn = match turn {
                    Turn::White => Turn::Black,
                    Turn::Black => Turn::White,
                };
            },
            "think" => {
                let best_move = lily.get_move(&turn);
                let command = format!("bestmove {}", bitboard::to_move(best_move.from,
                                                                       best_move.to));
                uci::send_command(command.as_str());
            }
            _ => {
                println!("Unhandled signal: {message}");
                break;
            }
        }
    };
}

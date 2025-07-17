use chess::uci;
use chess::uci::Command;
use std::thread;
use std::time::Duration;
use std::sync::mpsc;
use chess::bitboard;
use chess::engine;

fn main() {
    // I had no idea what these where supposed to be named:
    // tx, rx: goes from the GUI interface to the Engine
    // ty, ry: goes from the Engine to the GUI interface
    let (tx, rx) = mpsc::channel::<String>();
    let (ty, ry) = mpsc::channel::<String>();

    // UCI must always be able to handle messages so it will run on a separate
    // thread from the engine. Limited to a bandwidth of 200 commands a second
    // which should be enough to not timeout any GUI out there
    thread::spawn(move || {
        let mut state = uci::Interface::new("Lily", "CeriseSky", tx, ry);
        while state.is_running() {
            state.poll();
            uci::get_command().handle(&mut state);
            thread::sleep(Duration::from_millis(50));
        }
    });

    loop {
        let message = rx.recv().unwrap();
        let tokens: Vec<&str> = message.split_whitespace().collect();
        assert!(!tokens.is_empty());
        match tokens[0] {
            "quit" => { break; }
            "full_reset" => {},
            "board_reset" => {},
            "move" => {},
            "think" => {
                let best_move = engine::get_move();
                ty.send(format!("submit {}", bitboard::to_move(best_move.from,
                                                               best_move.to)))
                  .unwrap();
            }
            _ => {
                println!("Unhandled signal: {message}");
                break;
            }
        }
    };
}

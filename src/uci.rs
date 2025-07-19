use std::io::{Write,stdout,stdin};
use std::vec::Vec;
use std::sync::mpsc;

pub fn send_command(command: &str) {
    println!("{command}");
    stdout().flush().unwrap();
}

pub fn get_command() -> String {
    let mut buffer = String::new();
    stdin().read_line(&mut buffer).unwrap();

    buffer
}

pub struct Interface {
    name: String,
    author: String,
    running: bool,
    ready: bool,
    sender: mpsc::Sender::<String>,
}

impl Interface {
    pub fn new(name: &str, author: &str,
               sender: mpsc::Sender::<String>) -> Self {
        Self {
            name: name.to_string(),
            author: author.to_string(),
            running: true,
            ready: true,
            sender,
        }
    }

    pub fn get_name(&self) -> String {
        self.name.clone()
    }

    pub fn get_author(&self) -> String {
        self.author.clone()
    }

    pub fn is_running(&self) -> bool {
        self.running
    }

    pub fn quit(&mut self) {
        self.running = false;
    }

    pub fn is_ready(&self) -> bool {
        self.ready
    }

    pub fn send_signal(&self, message: String) {
        self.sender.send(message).unwrap();
    }
}

mod command_handlers {
    use crate::uci;

    pub fn uci(parameters: &uci::Interface) {
        uci::send_command(
            &format!("id name {}", parameters.get_name())
            );
        uci::send_command(
            &format!("id author {}", parameters.get_author())
            );
        uci::send_command("uciok");
    }

    pub fn isready(state: &uci::Interface) {
        while !state.is_ready() {}
        uci::send_command("readyok");
    }

    pub fn quit(state: &mut uci::Interface) {
        state.quit();
        state.send_signal("quit".to_string());
    }

    pub fn ucinewgame(state: &uci::Interface) {
        state.send_signal("full_reset".to_string());
    }

    pub fn position(state: &uci::Interface, tokens: &[&str]) {
        assert!(tokens.len() > 1);
        assert_eq!(tokens[1], "startpos");

        state.send_signal("board_reset".to_string());

        // tokens[2] is assumed to equal "moves" because that is the only
        // possibility in the UCI spec
        if tokens.len() > 2 {
            for token in &tokens[3..tokens.len()] {
                state.send_signal(format!("move {token}"));
            }
        }
    }

    pub fn go(state: &uci::Interface) {
        state.send_signal("think".to_string());
    }
}

pub trait Command {
    fn handle(&self, state: &mut Interface);
}

impl Command for String {
    fn handle(&self, state: &mut Interface) {
        let tokens: Vec<&str> = self.split_whitespace()
                                    .collect();
        assert!(!tokens.is_empty());

        match tokens[0] {
            "uci" => command_handlers::uci(state),
            "isready" => command_handlers::isready(state),
            "quit" => command_handlers::quit(state),
            "ucinewgame" => command_handlers::ucinewgame(state),
            "position" => command_handlers::position(state, &tokens),
            "go" => command_handlers::go(state),
            _ => {
                println!("Unhandled command: {}", tokens[0]);
                command_handlers::quit(state);
            },
        };
    }
}


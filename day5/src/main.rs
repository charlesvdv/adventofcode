use std::collections::LinkedList;
use std::fs::File;
use std::io::prelude::*;
use std::env;
use std::clone::Clone;

fn main() {
    let args: Vec<_> = env::args().collect();

    if args.len() != 2 {
        panic!("No file has been given");
    }

    let mut file = File::open(&args[1]).expect("file not found");
    let mut content = String::new();
    file.read_to_string(&mut content).expect("unable to read file");
    content.trim();

    let result = reduce(&content);
    println!("Part 1: {}", result);

    let mut best_size = std::u32::MAX;
    for to_remove in b'a'..(b'z' + 1) {
        let mut reduced_content = content.clone();
        reduced_content.retain(|c| c.to_ascii_lowercase() != to_remove as char);
        let size = reduce(&reduced_content) as u32;
        if size < best_size {
            best_size = size;
        }
    }
    println!("Part 2: {}", best_size);
}

fn reduce(content: &str) -> usize {
    let mut stack: LinkedList<char> = LinkedList::new();

    for ch in content.chars() {
        if !ch.is_ascii_alphabetic() {
            continue;
        }
        if let Some(elem) = stack.back() {
            if !are_opposite(elem.clone(), ch) {
                stack.push_back(ch);
            } else {
                stack.pop_back();
            }
        } else {
            stack.push_back(ch);
        }
    }

    stack.len()
}

fn are_opposite(a: char, b: char) -> bool {
    a.to_ascii_lowercase() == b.to_ascii_lowercase() && a != b
}

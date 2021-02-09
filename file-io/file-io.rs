use std::fs::File;
use std::io::prelude::*;
use std::io::BufReader;

const FILENAME: &str = "/tmp/r_test.txt";

fn main() -> std::io::Result<()> {
    write(FILENAME)?;
    read(FILENAME)?;
    Ok(())
}

fn write(filename: &str) -> std::io::Result<()> {
    let mut file = File::create(filename)?;
    file.write_all(b"This is a Rust test file.\n")?;
    for i in 1..11 {
        file.write_all(format!("This is Rust test line {}.\n", i).as_bytes())?;
    }
    Ok(())
}

fn read(filename: &str) -> std::io::Result<()> {
    let file = File::open(filename)?;
    let mut buf_reader = BufReader::new(file);
    let mut contents = String::new();
    buf_reader.read_to_string(&mut contents)?;
    print!("{}", contents);
    Ok(())
}
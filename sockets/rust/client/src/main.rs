use std::env;
use std::io::prelude::*;
use std::net::TcpStream;

fn main() -> std::io::Result<()> {
    let args: Vec<String> = env::args().collect();
    let host = &args[1];
    let port = &args[2];
    let message = &args[3];
    let mut buffer = vec![0; message.len()];
    let mut stream = TcpStream::connect(format!("{}:{}", host, port))?;

    stream.write(&[message.len() as u8])?;
    stream.write(message.as_bytes())?;
    stream.read(&mut buffer)?;
    buffer.resize(buffer[0] as usize, 0);
    let string = std::str::from_utf8(&buffer).unwrap();
    println!("Received message from server: {}", string);
    Ok(())
} // the stream is closed here

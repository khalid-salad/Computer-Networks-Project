use protobuf::Message;
use std::env;
use std::io::prelude::*;
use std::net::TcpStream;
mod person;

fn main() -> std::io::Result<()> {
    let args: Vec<String> = env::args().collect();
    let host = &args[1];
    let port = &args[2];
    let mut person = person::Person::new();
    person.set_name(String::from("Rust Client"));
    person.set_id(1);
    let message = person.write_to_bytes()?;
    let mut buffer = vec![0; 1024];
    let mut stream = TcpStream::connect(format!("{}:{}", host, port))?;
    stream.write(&message)?;
    let len = stream.read(&mut buffer)?;
    let server = person::Person::parse_from_bytes(&buffer[..len])?;
    println!("Received message from server.");
    println!("name: {}", server.get_name());
    println!("id: {}", server.get_id());
    Ok(())
} // the stream is closed here

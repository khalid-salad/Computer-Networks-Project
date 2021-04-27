use protobuf::Message;

use std::{
    env,
    io::{Read, Write},
    net::{TcpListener, TcpStream},
};

mod person;

fn handle_client(mut stream: TcpStream) -> std::io::Result<()> {
    let mut buffer = [0; 1024];
    let len = stream.read(&mut buffer)?;
    let client = person::Person::parse_from_bytes(&buffer[..len])?;
    let mut person = person::Person::new();
    person.set_name(String::from("Rust Server"));
    person.set_id(1);
    let message = person.write_to_bytes()?;
    println!("Received message from client.");
    println!("name: {}", client.get_name());
    println!("id: {}", client.get_id());
    stream.write(&message)?;
    Ok(())
}

fn main() -> std::io::Result<()> {
    let args: Vec<String> = env::args().collect();
    let port = &args[1];
    let listener = TcpListener::bind(format!("localhost:{}", port))?;
    // accept connections and process them serially
    for stream in listener.incoming() {
        handle_client(stream?)?;
    }
    Ok(())
}

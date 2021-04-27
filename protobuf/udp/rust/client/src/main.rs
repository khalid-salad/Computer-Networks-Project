use std::env;
use std::net::UdpSocket;
mod person;
use protobuf::Message;


fn main() -> std::io::Result<()> {
    let args: Vec<String> = env::args().collect();
    let host = &args[1];
    let port = &args[2];
    let mut person = person::Person::new();
    person.set_name(String::from("Rust Client"));
    person.set_id(1);
    let message = person.write_to_bytes()?;
    let mut buffer = [0; 1024];
    let socket = UdpSocket::bind(format!("{}:{}", host, 0))?;
    socket.connect(format!("{}:{}", host, port))?;
    socket.send(&message)?;
    let length = socket.recv(&mut buffer)?;
    let server = person::Person::parse_from_bytes(&buffer[..length])?;
    println!("Received message from server.");
    println!("name: {}", server.get_name());
    println!("id: {}", server.get_id());
    Ok(())
} // the stream is closed here

use std::{env, net::UdpSocket};
mod person;
use protobuf::Message;


fn handle_client(socket: &UdpSocket) -> std::io::Result<()> {
    let mut buffer = [0; 128];
    let (len, source) = socket.recv_from(&mut buffer)?;
    let client = person::Person::parse_from_bytes(&buffer[..len])?;
    let mut person = person::Person::new();
    person.set_name(String::from("Rust Server"));
    person.set_id(1);
    let message = person.write_to_bytes()?;
    println!("Received message from client.");
    println!("name: {}", client.get_name());
    println!("id: {}", client.get_id());
    socket.send_to(&message, &source)?;
    Ok(())
}

fn main() -> std::io::Result<()> {
    let args: Vec<String> = env::args().collect();
    let port = &args[1];
    let socket = UdpSocket::bind(format!("localhost:{}", port))?;
    loop {
        handle_client(&socket)?
    }
}

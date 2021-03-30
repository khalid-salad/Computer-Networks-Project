use std::env;
use std::net::UdpSocket;

fn main() -> std::io::Result<()> {
    let args: Vec<String> = env::args().collect();
    let host = &args[1];
    let port = &args[2];
    let message = &args[3];
    let mut buffer = [0; 128];
    let socket = UdpSocket::bind(format!("{}:{}", host, 0))?;
    socket.connect(format!("{}:{}", host, port))?;
    socket.send(message.as_bytes())?;
    let length = socket.recv(&mut buffer)?;
    let string = std::str::from_utf8(&mut buffer[..length]).unwrap();
    println!("Received message from server: {}", string);
    Ok(())
} // the stream is closed here

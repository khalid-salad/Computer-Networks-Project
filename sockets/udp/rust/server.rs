use std::{env, net::UdpSocket};

fn handle_client(socket: &UdpSocket) -> std::io::Result<()> {
    let mut buffer = [0; 128];
    let (_, source) = socket.recv_from(&mut buffer)?;
    let string = std::str::from_utf8(&buffer).unwrap();
    println!("Received message from client: {}", string);
    socket.send_to(b"Hello from the Rust Server", &source)?;
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

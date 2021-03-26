use std::{
    env,
    io::{Read, Write},
    net::{TcpListener, TcpStream},
};

fn handle_client(mut stream: TcpStream) -> std::io::Result<()> {
    let mut buffer = [0; 128];
    stream.read(&mut buffer)?;
    let string = std::str::from_utf8(&buffer).unwrap();
    println!("Received message from client: {}", string);
    stream.write(b"Hello from the Rust Server")?;
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

use std::{
    env,
    io::Read,
    io::Write,
    net::{TcpListener, TcpStream},
};

fn handle_client(mut stream: TcpStream) -> std::io::Result<()> {
    let mut buf = vec![0; 128];
    stream.read(&mut buf)?;
    buf.resize(buf[0] as usize, 0);
    stream.read(&mut buf)?;
    let string = std::str::from_utf8(&buf).unwrap();
    println!("Received message from client: {}", string);
    stream.write(b"Hello from the Rust Server")?;
    Ok(())
}

fn main() -> std::io::Result<()> {
    let args: Vec<String> = env::args().collect();
    let host = &args[1];
    let port = &args[2];
    println!("{}:{}", host, port);
    let listener = TcpListener::bind(format!("{}:{}", host, port))?;

    // accept connections and process them serially
    for stream in listener.incoming() {
        handle_client(stream?)?;
    }
    Ok(())
}

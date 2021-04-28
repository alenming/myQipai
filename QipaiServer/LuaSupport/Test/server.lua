socket = require("socket");
host = host or "127.0.0.1";
port = port or "8383";
server = assert(socket.bind(host, port));
ack = "ack\n";
while 1 do
    print("server: waiting for client connection...");
    control = assert(server:accept());
    while 1 do 
        command,status = control:receive();
  if status == "closed" then break end
        print(command);
        control:send(ack);
    end
end
local socket = require("socket")

host = "127.0.0.1"
port = 8383

--打开一个TCP连接
c = assert (socket.connect (host, port))

c:send ("GET \n")
while (true) do
 local s, status, partial = c:receive ()
 print(s)
 if status == "closed" then break end
 c:send ("GET \n")
end

c:close ()
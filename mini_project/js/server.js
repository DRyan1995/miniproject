var http = require("http");
var net = require("net");
var dispatch = require("./node_modules/dispatch");
var Socket;
var receivedMsg;

var server = http.createServer(
    dispatch({
        '/about': function(req, res){
          res.end("Fuck you !");
        },
        '/pwd/:id': function(req, res, id){
          console.log('get pwd request, the pwd is:' + id);
          res.end(id);
          if (Socket) {
            var msg = '*PWD' + id + '*';
            sendMsgToA(msg);
          }else {
            console.log("the socket has not been established!");
          }
        },
        '/relay':function(req, res) {
          if (client) {
            client.write('Ryan');
            res.end("relay toggled!");
          }
        },
        '/tem':function(req, res) {
          if (client) {
            client.write('tem');
            res.end(receivedMsg);
          }
        }
    })
);
server.listen(8080);
console.log('HTTP Server running at http://192.168.0.201:8080/');
console.log('HTTP Server running at http://192.168.0.201:8888/');

var tcpServer = net.createServer(function(socket) {
  Socket = socket;
	socket.write('hello from server\r\n');
	socket.pipe(socket);
  console.log("client connected");
});

tcpServer.listen(8888, '0.0.0.0');

function sendMsgToA(msg) {
  Socket.write(msg);
	Socket.pipe(Socket);
}

var client = new net.Socket();
client.connect(9988, '192.168.0.200', function() {
	console.log('De1-Soc Connected');
	client.write('Hello from Server');
});

client.on('data', function(data) {
	console.log('Received: ' + data);
  receivedMsg = data;
	// client.destroy(); // kill client after server's response
});

client.on('close', function() {
	console.log('Connection closed');
});

var http = require("http");
var net = require("net");
var dispatch = require("./node_modules/dispatch");
var Socket;

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
            sendMsg(msg);
          }else {
            console.log("the socket has not been established!");
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

function sendMsg(msg) {
  Socket.write(msg);
	Socket.pipe(Socket);
}

//
// var client = new net.Socket();
// client.connect(8080, '192.168.0.101', function() {
// 	console.log('Connected');
// 	client.write('Hello, server! Love, Client.');
// });
//
// client.on('data', function(data) {
// 	console.log('Received: ' + data);
// 	client.destroy(); // kill client after server's response
// });
//
// client.on('close', function() {
// 	console.log('Connection closed');
// });

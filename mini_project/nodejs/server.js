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

var tcpServer = net.createServer(function(socket) {
  Socket = socket;
	socket.write('Hello from NODEJS server!\r\n');
	socket.pipe(socket);
  console.log("Arduino connected!");
  socket.on('data', function(data){
    console.log("NODEJS tcpServer receive from Arduino:" + socket.remoteAddress + ':' + data);
  });
  socket.on('close', function(data) {
    console.log("NODEJS tcpServer closed connection to Arduino" + socket.remoteAddress + ':' + socket.remotePort);
  });
});

tcpServer.listen(8888, '0.0.0.0');

function sendMsgToA(msg) {
  Socket.write(msg);
	Socket.pipe(Socket);
}

console.log('TCP Server running at http://192.168.0.201:8888/');

var temperatureData = "Initializing, Please Try again";

var client = new net.Socket();
  client.connect(9988, '192.168.0.200', function() {
	console.log('DE1-SOC Connected');
	client.write('Hello from NODEJS Server');
});

client.on('data', function(data) {
  data = data.toString();
	console.log('Msg From DE1-SOC: ' + data);
  if (data.substring(0, 3) == 'The') {
    temperatureData = data;
  }else if (data == 'CLOSE') {
    client.destroy(); // kill client after server's response
  }//todo

});

client.on('close', function() {
	console.log('DE1-SOC Connection closed');
});

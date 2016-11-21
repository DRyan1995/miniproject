var http = require("http");
var net = require("net");
var dispatch = require("./node_modules/dispatch");
var ServerSocketList = [];

var server = http.createServer(
    dispatch({

        '/about': function(req, res){
          res.end("Fuck you !");
        },

        '/pwd/:id': function(req, res, id){
          console.log('nodejs http get pwd request, the pwd is:' + id);
          res.end(id);
          var ins = '*PWD' + id + '*';
          SendInstructions(ins);
        },

        '/delay_time/:delayTime':function(req, res, delayTime) {
          console.log('nodejs http get delay_time reset request, the delay_time is:' + delayTime);
          res.end(delayTime);
          delayTime = delayTime.toString();
          while (delayTime.length < 10) {
            delayTime = '0' + delayTime;
          }
          var ins = '*DELAY' + delayTime + '*';
          SendInstructions(ins);
        },

        '/relay':function(req, res) {
          if (client) {
            client.write('Ryan');
            res.end("relay toggle sent from NODEJS!");
          }
        },

        '/tem':function(req, res) {
          if (client) {
            client.write('tem');
            res.end(temperatureData);
          }
        }
    })
);
server.listen(8080);
console.log('HTTP Server running at http://192.168.0.201:8080/');

var tcpServer = net.createServer(function(socket) {
  // Socket = socket;
  ServerSocketList.push(socket);
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

function SendInstructions(instructions) {
  if (ServerSocketList.length > 0) {
    ServerSocketList.forEach(function(Socket){
      Socket.write(instructions);
    	Socket.pipe(Socket);
      console.log("The request " +  instructions + " sent to TCP clients: " + ServerSocketList.length);
    });
  }else {
    console.log("TCP Server does not have any clients!");
  }
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

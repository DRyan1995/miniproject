var http = require("http");
var net = require("net");
var dispatch = require("./node_modules/dispatch");

var server = http.createServer(
    dispatch({
        '/about': function(req, res){
          res.end("Fuck you !");
        },
        '/pwd/:id': function(req, res, id){
          console.log('get pwd request');
          res.end(id);
        }
    })
);
server.listen(8080);
console.log('Server running at http://127.0.0.1:8080/');

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

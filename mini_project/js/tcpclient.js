var net = require("net");

var client = new net.Socket();
client.connect(9988, '192.168.0.200', function() {
	console.log('Connected');
	client.write('Ryan');
});

client.on('data', function(data) {
	console.log('Received: ' + data);
	// client.destroy(); // kill client after server's response
});

client.on('close', function() {
	console.log('Connection closed');
});

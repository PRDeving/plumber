var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);

app.get('/', function(req, res){
  res.sendfile('index.html');
});

io.on('connection', function(socket){
  console.log('a user connected');
});

http.listen(3000, function(){
  console.log('listening on *:3000');
});


// var net = require('net');
// const http = require('http')
// const io = require('socket.io')(http);
// const port = 3000
//
// const clients = {};
//
// io.on('connection', (socket) => {
//   console.log('hi');
// });
//
// const requestHandler = (request, response) => {
//   console.log(request.url)
//   response.end('Hello Node.js Server!\n' + Object.keys(clients).join());
// }
//
// const httpserver = http.createServer(requestHandler);
// httpserver.listen(port, (err) => {
//   if (err) return console.log('something bad happened', err)
//   console.log(`server is listening on ${port}`)
// });
//
//
//
//
// var buffer = "";
// var server = net.createServer(function(socket) {
//   console.log('connection');
//
//   socket.on('data', function(chunk) {
//     buffer += chunk;
//     socket.write("halloo\0");
//     setTimeout(() => socket.write('close\0'), 2000);
//   });
// });
//
// function digest() {
//   setTimeout(function() {
//     var sindex = buffer.indexOf('/0');
//     if (sindex != -1) {
//       got(buffer.slice(0, sindex));
//       buffer = buffer.slice(sindex + 2);
//     }
//     digest();
//   }, 300);
// }
// digest();
//
// server.listen(1337, '127.0.0.1');
//
//
// function got(msg) {
//   msg = msg.split('~~');
//   console.log(msg);
//   clients[msg[0]] = {};
// }

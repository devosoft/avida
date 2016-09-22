/*
  The purpose of this javascript prepended code is to:
  (1) keep consistant state information about avida in order to...
  (2) handle "file system" access rights and
  (3) make sure that worker-spawners have appropriate state information, also
  (4) provide message passing features such as
  (5) queuing messages when Avida is actively running
  (6) passing messages directly when avida is in an inactive state, and finally
  (7) provide functionality for Avida to pass messages to worker-spawners

  All function calls into and out of Avida handling messages will
  be points to strings of JSON objects.
*/

var avida_running = 0;
var msg_queue = [];
var diagnostic_socket = null;

/*
  Try to import external socketio client
*/
self.importScripts("https://cdn.socket.io/socket.io-1.4.5.js");

if (io){
  diagnostic_socket = io.connect('http://localhost:5000/avida');
  diagnostic_socket.on('message', function(e) {
      console.log('Received ' + e);
  });
  diagnostic_socket.on('connect', function(){
    console.log('Socket connected');
  });
  diagnostic_socket.on('disconnect', function(){
    console.log('Socket disconnected.');
  });
}

/*
  Handle incoming messages from parent
*/
onmessage = function(msg) {
    msg_queue.push(msg.data);
    if (diagnostic_socket){
      diagnostic_socket.emit('ui_msg', msg);
    }
}


function doGetMessage() {
  if (msg_queue.length > 0){
    var msg = JSON.stringify(msg_queue.shift());
    var buffer = _malloc(msg.length+1);
    writeStringToMemory(msg,buffer);
    return buffer;
  }
  return 0;
}


/*
  Avida will use doPostMessage when it wants to
  communicate with Worker owner(s).  The necessary
  pointer-to-string-to-json conversion occurs here.
*/
function doPostMessage(msg_str) {
  var json_msg = JSON.parse(msg_str);
  if (json_msg.key === 'AvidaStatus'){
    switch (json_msg.Status){
      case 'Paused':
      case 'Finished':
        avida_running = 0;
        break;
      case 'Running':
        avida_running = 1;
        break;
      default:
        throw 'Undefined Avida state';
        break;
    }
  }
  if (diagnostic_socket){
    diagnostic_socket.emit('av_msg', json_msg);
  }
  postMessage(json_msg);
}

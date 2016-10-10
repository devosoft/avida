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

var avida_update = -1;  //Holds the current update as notified
var msg_queue = [];     //Holds the current messages to send to avida
var diagnostic_socket = null;  //Holds the diagnostic socket if available
var enable_diagnostic_socket = true;  //Should an attempt be made to connect to the diagnostic socket?

/*
  Try to import external socketio client
*/
self.importScripts("https://cdn.socket.io/socket.io-1.4.5.js");

if (io && enable_diagnostic_socket){
  diagnostic_socket = io.connect('http://localhost:5000/avida');
  diagnostic_socket.on('connect', function(){
  });
  diagnostic_socket.on('disconnect', function(){
  });
  diagnostic_socket.on('ext_command', function(msg){
    msg_queue.push(msg);
    sendDiagMsg('av_ext_command', 'in', msg);
  });
}

function sendDiagMsg(msg_from, io_type, data)
{
  if (enable_diagnostic_socket && diagnostic_socket != null){
    msg = {
            data:data,
            meta:{
                _tx_update:avida_update,
                _from:msg_from,
                _io_type:io_type
            }
          };
    diagnostic_socket.emit('message', msg);
  }
}


/*
  Handle incoming messages from parent
*/
onmessage = function(msg) {
    msg_queue.push(msg.data);
    sendDiagMsg('ui', 'in', msg.data);
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
  switch(json_msg.type){
    case 'update':
      avida_update = json_msg.update;
      sendDiagMsg('av_debug', 'out', json_msg);
      return;
      break;
    case 'av_debug':
      sendDiagMsg('av_debug', 'out', json_msg);
      return;
      break;
  }
  sendDiagMsg('av', 'out', json_msg);
  postMessage(json_msg);
}

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

onmessage = function(msg) {
  msg_queue.push(msg.data);
}

function getMessages() {
  var msgs = JSON.stringify(msg_queue);
  var buffer = _malloc(msgs.length+1);
  writeStringToMemory(msgs,buffer);
  msg_queue = [];
  return buffer;
}


/*
  Avida will use doPostMessage when it wants to
  communicate with Worker owner(s).  The necessary
  pointer-to-string-to-json conversion occurs here.
*/
function doPostMessage(msg_str) {
  var json_msg = JSON.parse(msg_str);
  if (json_msg.Key === 'AvidaStatus'){
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
  postMessage(json_msg);
}

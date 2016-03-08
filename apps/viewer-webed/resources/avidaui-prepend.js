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
var ports = { console:null, ui:null };

//Checking to see if we can use a shared web worker
if (false){
  onconnect = function(msg){
    var port = msg.port[0];
    var to_queue;
    if (msg.data['zsource'] == 'console'){
      ports.console = port;
      ports.console.onmessage = function(msg){
        to_queue = msg.data;
        port.postMessage(msg.data);
        delete to_queue.zsource;
        msg_queue.push(to_queue);
      }
    } else {
      port.ui = port;
      port.ui.onmessage = function(msg){
        to_queue = msg.data;
        msg_queue.push(to_queue);
        if (ports.console !== null){
          to_relay = msg.data;
          msg.data.zsource = 'ui';
          ports.console.postMessage(to_relay);
        }
      }
    }
  }
} else {
  onmessage = function(msg) {
    msg_queue.push(msg.data);
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
  
  //Relay information to the console interface if needed
  if (false){
    if (ports.ui !== null && msg.type !== 'debug'){
      ports.ui.postMessage(json_msg);
    }
    if (ports.console !== null){
      ports.ui.postMessage(json_msg);
    }
  } else {
    postMessage(json_msg);
  }
}

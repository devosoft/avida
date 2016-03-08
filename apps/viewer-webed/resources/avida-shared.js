if (SharedWorker != undefined){
   if (av.debug.root) console.log('before creating shared worker');
   av.fio.uiWorker = new SharedWorker('avida.js').port;
   if (av.debug.root) console.log('before fio.uiWorker on message');
   av.fio.uiWorker.onmessage = function (ee) {av.msg.readMsg(ee);};
} else {
   if (av.debug.root) console.log('before creating singleton worker');
   av.fio.uiWorker = new Worker('avida.js');
   av.fio.uiWorker.onmessage = function (ee) {av.msg.readMsg(ee);};
}

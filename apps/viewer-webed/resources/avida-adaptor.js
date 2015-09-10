// Start web worker for avida-core
var coreWorker = new Worker('avida.js');

coreWorker.onmessage = function(e){
   console.log(e.data);
}

var runpause_msg = {
  'Key':'RunPause'
}

var finish_msg = {
  'Key':'Finish'
}

var timer_func_called = 0;
var aTimer=setInterval( timerFunc, 1000 )

function timerFunc(){
   timer_func_called = timer_func_called + 1;

   if (timer_func_called == 50){
      console.log("Sending Finish");
      coreWorker.postMessage(finish_msg);
      clearInterval(aTimer);
   }
   if (timer_func_called % 10 == 0){
      console.log("Sending RunPause");
      coreWorker.postMessage(runpause_msg);
   }
}

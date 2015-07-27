// Start web worker for avida-core
var coreWorker = new Worker('avida.js');

coreWorker.onmessage = function(e){
   console.log(e.data);
}

var timer_func_called = 0;
var aTimer=setInterval( function () {timerFunc();}, 1000 )

function timerFunc(){
   timer_func_called = timer_func_called + 1;

   if (timer_func_called == 500){
      coreWorker.postMessage('Stop');
      exit;
   }
   if (timer_func_called % 10 == 0){
      coreWorker.postMessage('PlayPause');
   }
}

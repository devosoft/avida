#include <iostream>

#include "../../tools/tArray.h"
#include "../../tools/cFile.h"
#include "../../tools/cRandom.h"
#include "../../tools/cString.h"
#include "../../tools/cInitFile.h"
#include "../../tools/cStringList.h"

using namespace std;


int main(int argc, char * argv[])
{
  if (argc != 6) {
    cerr << "Usage: " << argv[0]
	 << " [env_file] [task_count] [start] [stop] [step]"
	 << endl
	 << "  [env_file] is the the environment file you are using." << endl
	 << "  [task_count] is the number of tasks active at once." << endl
	 << "  [start] is the first update to make a change at" << endl
	 << "  [stop] is the last update to make a change at" << endl
	 << "  [step] is the time between task changes." << endl
	 << endl;
    exit(1);
  }


  // Collect the information from the command line arguments.
  const cString env_filename(argv[1]);
  const cString task_count_str(argv[2]);
  const cString start_str(argv[3]);
  const cString stop_str(argv[4]);
  const cString step_str(argv[5]);

  const int task_count = task_count_str.AsInt();
  const int update_start = start_str.AsInt();
  const int update_stop  = stop_str.AsInt();
  const int update_step  = step_str.AsInt();

  // Load in the environment file that we're using...
  cInitFile env_file(env_filename);
  env_file.Load();
  env_file.Compress();

  // Create a container for the resource names...
  cStringList res_list;

  // Collect the resource names...
  for (int i = 0; i < env_file.GetNumLines(); i++) {
    cString cur_line = env_file.GetLine(i);
    cString line_type = cur_line.PopWord();
    line_type.ToUpper();                      // Make keyword is upper case.
    
    // Only pay attention to the RESOURCE commands...
    if (line_type != "RESOURCE") continue;

    // Grab each resource defined on this line...
    while (cur_line.GetSize() > 0) {
      // Grab just the current resource name; strip everything past the ':'.
      cString cur_res = cur_line.PopWord().Pop(':');
      res_list.PushRear(cur_res);
    }
  }

  // Make sure everything is okay so far...
  const int num_tasks = res_list.GetSize();
  if (task_count >= num_tasks) {
    cerr << "Error: Only " << num_tasks << " taks are available; cannot have "
	 << task_count << "rotate!" << endl;
    exit(1);
  }

  if (update_start >= update_stop) {
    cerr << "Error: Stop update (" << update_stop
	 << ") must be *after* start update (" << update_start
	 << ")!" << endl;
    exit(1);
  }

  // Let the user know what's going on
  cerr << "Generating events with " << task_count
       << " simultaneously active tasks out of " << num_tasks
       << " available." << endl
       << "...changes occur every " << update_step
       << " updates, from " << update_start
       << " until " << update_stop << "." << endl;

  // Now, generate the output file!
  cRandom random;

  // Start with turning on the initial tasks.
  tArray<int> cur_task(task_count);
  for (int i = 0; i < task_count; i++) {
    int next_task = random.GetUInt(num_tasks);
    cur_task[i] = next_task;
    // Make sure this task hasn't already been chosen.
    for (int j = 0; j < i; j++) {
      if (cur_task[j] == next_task) {
	i--;
	break;
      }
    }
  }

  // Now write out the initial state...
  for (int i = 0; i < task_count; i++) {
    cout << "u 0 set_resource " << res_list.GetLine(cur_task[i]) << " 1" << endl;
  }

  // Loop through the updates making random changes at each step.
  for (int cur_update = update_start;
       cur_update <= update_stop;
       cur_update += update_step) {
    bool found_next = false;
    int next_task = -1;
    while (found_next == false) {
      next_task = random.GetUInt(num_tasks);
      found_next = true;

      // Test to make sure this isn't a duplicate!
      for (int i = 0; i < task_count; i++) {
	if (next_task == cur_task[i]) found_next = false;
      }
    }

    // Figure out which task is being replaced and do so.
    int task_pos = random.GetUInt(task_count);
    int old_task = cur_task[task_pos];
    cur_task[task_pos] = next_task;

    // Write out the replacement...
    cout << "u " << cur_update << " set_resource "
	 << res_list.GetLine(old_task) << " 0" << endl
	 << "u " << cur_update << " set_resource "
	 << res_list.GetLine(next_task) << " 1" << endl;
  }
}

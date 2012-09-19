#include "cContextPhenotype.h"

using namespace std;


void cContextPhenotype::AddTaskCounts(int number_tasks, Apto::Array<int>& cur_task_count)
{
    // Step 1: Resize m_cur_thread_task_count array if necessary.  This is necessary
    // since CThreadPhenotype object does not have any information apriori about m_world.

    if(m_number_tasks != number_tasks) {
      m_cur_task_count.ResizeClear(number_tasks);
      for(int count=0;count<number_tasks;count++) {
        m_cur_task_count[count] = 0;
      }
      m_number_tasks = number_tasks;
    }

    // Step 2 : Add tasks for each count.
    for(int count=0;count<cur_task_count.GetSize();count++)
    {
      m_cur_task_count[count] += cur_task_count[count];
    }
}
void cContextPhenotype::AddReactionCounts(int number_reactions, Apto::Array<int>& cur_reaction_count)
{
    // Step 1: Resize m_cur_thread_task_count array if necessary.  This is necessary
    // since CThreadPhenotype object does not have any information apriori about m_world.

    if(m_number_reactions != number_reactions)
    {
      m_cur_reaction_count.ResizeClear(number_reactions);
      for(int count=0;count<number_reactions;count++) {
        m_cur_reaction_count[count] = 0;
      }
      m_number_reactions = number_reactions;
    }

    // Step 2 : Add tasks for each count.
    for(int count=0;count<cur_reaction_count.GetSize();count++)
    {
      m_cur_reaction_count[count] += cur_reaction_count[count];
    }
}

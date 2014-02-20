/*
 *  environment/actions/SequenceActions.cc
 *  avida-core
 *
 *  Created by David on 2/20/14.
 *  Copyright 2014 Michigan State University. All rights reserved.
 *  http://avida.devosoft.org/
 *
 *
 *  This file is part of Avida.
 *
 *  Avida is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 *  Avida is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along with Avida.
 *  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Authors: David M. Bryson <david@programerror.com>
 *
 */

#include "avida/private/environment/CoreActions.h"

#include "avida/core/Feedback.h"


void cTaskLib::Load_SortInputs(const cString& name, const cString& argstr, cEnvReqs& envreqs, Feedback& feedback)
{
  Util::ArgSchema schema;
  
  // Integer Arguments
  schema.Define("size", Util::INT); // Number of items to sort
  schema.Define("direction", 0); // < 0 = Descending, Otherwise = Ascending
  schema.Define("contiguous", 1); // 0 = No, Otherwise = Yes
  // Double Arguments
  schema.Define("halflife", Util::DOUBLE);
  
  Util::Args* args = Util::Args::Load((const char*)argstr, schema, ',', '=', &feedback);
  if (args) {
    envreqs.SetMinInputs(args->Int(0));
    envreqs.SetMinOutputs(args->Int(0) * 2);
    envreqs.SetTrueRandInputs();
    NewTask(name, "Sort Inputs", &cTaskLib::Task_SortInputs, 0, args);
  }
}

double cTaskLib::Task_SortInputs(cTaskContext& ctx) const
{
  const Util::Args& args = ctx.GetTaskEntry()->GetArguments();
  const tBuffer<int>& output = ctx.GetOutputBuffer();
  const int size = args.Int(0);
  const int stored = output.GetNumStored();
  
  // if less than half, can't possibly reach threshold
  if (stored <= (size / 2)) return 0.0;
  
  Apto::Map<int, int> valmap;
  int score = 0;
  int maxscore = 0;
  
  // add all valid inputs into the value map
  for (int i = 0; i < size; i++) valmap.Set(ctx.GetOrganism()->GetInputAt(i), -1);
  
  int span_start = -1;
  int span_end = stored;
  
  if (args.Int(2)) { // Contiguous
    // scan for the largest contiguous span
    // - in the event of a tie, keep the first discovered
    for (int i = 0; i < stored; i++) {
      if (valmap.Has(output[i])) {
        int t_start = i;
        while (++i < stored && valmap.Has(output[i])) ;
        if (span_start == -1 || (i - t_start) > (span_end - span_start)) {
          span_start = t_start;
          span_end = i;
        }
      }
    }
    
    // no span was found
    if (span_start == -1) return 0.0;
  } else { // Scattered
    // search for first valid entry
    while (++span_start < stored && valmap.Has(output[span_start])) ;
    
    // scanned past the end of the output, nothing to validate
    if (span_start >= stored) return 0.0;
  }
  
  // again, if span is less than half the size can't possibly reach threshold
  if ((span_end - span_start) <= (size / 2)) return 0.0;
  
  // insertion sort span
  // - count number of actual entries
  // - count moves required
  // - update valmap, tracking observed inputs
  Apto::Array<int> sorted(size);
  const bool ascending = (args.Int(1) >= 0);
  int count = 1;
  
  // store first value
  valmap.Set(output[span_start], span_start);
  sorted[0] = output[span_start];
  
  // iterate over the remaining span (discovered for contiguous, full output for scattered)
  for (int i = span_start + 1; i < span_end; i++) {
    int value = output[i];
    
    // check for a dup or invalid output, skip it if so
    int idx;
    if (!valmap.Get(value, idx) || idx != -1) continue;
    
    maxscore += count; // count the maximum moves possible
    count++; // iterate the observed count
    valmap.Set(value,i); // save position, so that missing values can be determined later
    
    // sort value based on ascending for descending, counting moves
    int j = count - 2;
    while (j >= 0 && ((ascending && sorted[j] > value) || (!ascending && sorted[j] < value))) {
      sorted[j + 1] = sorted[j];
      j--;
      score++;
    }
    sorted[j + 1] = value;
  }
  
  // if not all of the inputs were observed
  if (count < size) {
    // iterate over all inputs
    for (int i = 0; i < size; i++) {
      int idx;
      // if input was not observed
      if (valmap.Get(ctx.GetOrganism()->GetInputAt(i), idx) && idx == -1) {
        maxscore += count; // add to the maximum move count
        score += count; // missing values, scored as maximally out of order
        count++; // increment observed count
      }
    }
  }
  
  double quality = 0.0;
  
  // score of 50% expected with random output
  // - only grant quality when less than 50% maximum moves are required
  if (static_cast<double>(score) / static_cast<double>(maxscore) < 0.5) {
    double halflife = -1.0 * fabs(args.Double(0));
    quality = pow(2.0, static_cast<double>(score) / halflife);
  }
  
  return quality;
}




class cFibSeqState : public cTaskState {
public:
  int seq[2];
  int count;
  
  cFibSeqState() : count(0) { seq[0] = 1; seq[1] = 0; }
};


void cTaskLib::Load_FibonacciSequence(const cString& name, const cString& argstr, cEnvReqs&, Feedback& feedback)
{
  Util::ArgSchema schema;
  
  // Integer Arguments
  schema.Define("target", Util::INT);
  // Double Arguments
  schema.Define("penalty", 0.0);
  
  Util::Args* args = Util::Args::Load((const char*)argstr, schema, ',', '=', &feedback);
  
  if (args) NewTask(name, "Fibonacci Sequence", &cTaskLib::Task_FibonacciSequence, 0, args);
}


double cTaskLib::Task_FibonacciSequence(cTaskContext& ctx) const
{
  const Util::Args& args = ctx.GetTaskEntry()->GetArguments();
  cFibSeqState* state = static_cast<cFibSeqState*>(ctx.GetTaskState());
  if (state == NULL) {
    state = new cFibSeqState();
    ctx.AddTaskState(state);
  }
  
  const int next = state->seq[0] + state->seq[1];
  
  // If output matches next in sequence
  if (ctx.GetOutputBuffer()[0] == next) {
    // Increment count and store next value
    state->count++;
    state->seq[state->count % 2] = next;
    
    // If past target sequence ending point, return the penalty setting
    if (state->count > args.Int(0)) return args.Double(0);
    
    return 1.0;
  }
  
  return 0.0;
}



bool Avida::Environment::RegisterLogicActions(Feedback& feedback)
{
  if (name == "sort_inputs") Load_SortInputs(name, info, envreqs, feedback);
  else if (name == "fibonacci_seq") Load_FibonacciSequence(name, info, envreqs, feedback);
}

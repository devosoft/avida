//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef ENVIRONMENT_HH
#include "environment.hh"
#endif

#ifndef INIT_FILE_HH
#include "init_file.hh"
#endif
#ifndef MUTATION_MACROS_HH
#include "mutation_macros.hh"
#endif
#ifndef RANDOM_HH
#include "random.hh"
#endif
#ifndef REACTION_HH
#include "reaction.hh"
#endif
#ifndef REACTION_MACROS_HH
#include "reaction_macros.hh"
#endif
#ifndef REACTION_PROCESS_HH
#include "reaction_process.hh"
#endif
#ifndef REACTION_REQUISITE_HH
#include "reaction_requisite.hh"
#endif
#ifndef REACTION_RESULT_HH
#include "reaction_result.hh"
#endif
#ifndef RESOURCE_HH
#include "resource.hh"
#endif
#ifndef STRING_UTIL_HH
#include "string_util.hh"
#endif
#ifndef TASK_ENTRY_HH
#include "task_entry.hh"
#endif
#ifndef TOOLS_HH
#include "tools.hh"
#endif

using namespace std;

cEnvironment::cEnvironment()
{
}

// cEnvironment::cEnvironment(const cString & filename)
// {
//   if (Load(filename) == false) {
//     cerr << "Unable to load environment... aborting!" << endl;
//     abort();
//   }
// }

bool cEnvironment::ParseSetting(cString entry, cString & var_name,
				cString & var_value, const cString & var_type)
{
  // Make sure we have an actual entry to parse.
  if (entry.GetSize() == 0) {
    cerr << "Error: Empty setting to parse in " << var_type << endl;
    return false;
  }

  // Collect the values...
  var_name = entry.Pop('=');
  var_value = entry;

  // Make sure we have both a name and a value...
  if (var_name.GetSize() == 0) {
    cerr << "Error: No variable povided to set to '" << var_value
	 << "' in " << var_type << endl;
    return false;
  }

  if (var_value.GetSize() == 0) {
    cerr << "Error: No value given for '" << var_name
	 << "' in " << var_type << endl;
    return false;
  }

  // Make the names case insensitive.
  var_name.ToLower();

  return true;
}

bool cEnvironment::AssertInputInt(const cString & input,
				  const cString & name,
				  const cString & type)
{
  if (input.IsNumeric() == false) {
    cerr << "Error: In " << type << "," << name << " set to non-integer."
	 << endl;
    return false;
  }
  return true;
}

bool cEnvironment::AssertInputDouble(const cString & input,
				     const cString & name,
				     const cString & type)
{
  if (input.IsNumber() == false) {
    cerr << "Error: In " << type << "," << name << " set to non-number."
	 << endl;
    return false;
  }
  return true;
}

bool cEnvironment::AssertInputBool(const cString & input,
				  const cString & name,
				  const cString & type)
{
  if (input.IsNumber() == false) {
    cerr << "Error: In " << type << "," << name << " set to non-number."
	 << endl;
    return false;
  }
  int value = input.AsInt();
  if ((value != 1) && (value != 0))  {
    cerr << "Error: In " << type << "," << name << " set to non-bool."
	 << endl;
    return false;
  }
  return true;
}

bool cEnvironment::AssertInputValid(void * input,
				    const cString & name,
				    const cString & type,
				    const cString & value)
{
  if (input == NULL) {
    cerr << "Error: In " << type << ", '" << name << "' setting of '"
	 << value << "' not found." << endl;
    return false;
  }
  return true;
}



bool cEnvironment::LoadReactionProcess(cReaction * reaction, cString desc)
{
  cReactionProcess * new_process = reaction->AddProcess();

  // Loop through all entries in description.
  while (desc.GetSize() > 0) {
    // Entries are divided by colons.
    cString var_entry = desc.Pop(':');
    cString var_name;
    cString var_value;
    const cString var_type =
      cStringUtil::Stringf("reaction '%s' process", reaction->GetName()());

    // Parse this entry.
    if (!ParseSetting(var_entry, var_name, var_value, var_type)) return false;

    // Now that we know we have a variable name and its value, set it!
    if (var_name == "resource") {
      cResource * test_resource = resource_lib.GetResource(var_value);
      if (!AssertInputValid(test_resource, "resource", var_type, var_value)) {
	return false;
      }
      new_process->SetResource(test_resource);
    }
    else if (var_name == "value") {
      if (!AssertInputDouble(var_value, "value", var_type)) return false;
      new_process->SetValue(var_value.AsDouble());
    }
    else if (var_name == "type") {
      if (var_value=="add") new_process->SetType(REACTION_PROCTYPE_ADD);
      else if (var_value=="mult") new_process->SetType(REACTION_PROCTYPE_MULT);
      else if (var_value=="pow") new_process->SetType(REACTION_PROCTYPE_POW);
      else {
	cerr << "Unknown reaction process type '" << var_value
	     << "' found in '" << reaction->GetName() << "'." << endl;
	return false;
      }
    }
    else if (var_name == "max") {
      if (!AssertInputDouble(var_value, "max", var_type)) return false;
      new_process->SetMaxNumber(var_value.AsDouble());
    }
    else if (var_name == "min") {
      if (!AssertInputDouble(var_value, "min", var_type)) return false;
      new_process->SetMinNumber(var_value.AsDouble());
    }
    else if (var_name == "frac") {
      if (!AssertInputDouble(var_value, "frac", var_type)) return false;
      double in_frac = var_value.AsDouble();
      if (in_frac > 1.0) in_frac = 1.0;
      new_process->SetMaxFraction(in_frac);
    }
    else if (var_name == "product") {
      cResource * test_resource = resource_lib.GetResource(var_value);
      if (!AssertInputValid(test_resource, "product", var_type, var_value)) {
	return false;
      }
      new_process->SetProduct(test_resource);
    }
    else if (var_name == "conversion") {
      if (!AssertInputDouble(var_value, "conversion", var_type)) return false;
      new_process->SetConversion(var_value.AsDouble());
    }
    else if (var_name == "inst") {
      new_process->SetInstID( inst_set.GetInst(var_value).GetOp() );
    }
    else if (var_name == "lethal") {
      if (!AssertInputBool(var_value, "lethal", var_type)) 
	return false;
      new_process->SetLethal(var_value.AsInt());
    }
    else if (var_name == "detect") {
      cResource * test_resource = resource_lib.GetResource(var_value);
      if (!AssertInputValid(test_resource, "product", var_type, var_value)) {
	return false;
      }
      new_process->SetDetect(test_resource);
    }
    else if (var_name == "threshold") {
      if (!AssertInputDouble(var_value, "threshold", var_type))
	return false;
      new_process->SetDetectionThreshold(var_value.AsDouble());
    }
    else if (var_name == "detectionerror") {
      if (!AssertInputDouble(var_value, "detectionerror", var_type)) 
	return false;
      new_process->SetDetectionError(var_value.AsDouble());
    }
    else {
      cerr << "Error: Unknown process variable '" << var_name
	   << "' in reaction '" << reaction->GetName() << "'" << endl;
      return false;
    }
  }

  return true;
}

bool cEnvironment::LoadReactionRequisite(cReaction * reaction, cString desc)
{
  cReactionRequisite * new_requisite = reaction->AddRequisite();

  // Loop through all entries in description.
  while (desc.GetSize() > 0) {
    // Entries are divided by colons.
    cString var_entry = desc.Pop(':');
    cString var_name;
    cString var_value;
    const cString var_type =
      cStringUtil::Stringf("reaction '%s' requisite", reaction->GetName()());

    // Parse this entry.
    if (!ParseSetting(var_entry, var_name, var_value, var_type)) return false;

    // Now that we know we have a variable name and its value, set it!
    if (var_name == "reaction") {
      cReaction * test_reaction = reaction_lib.GetReaction(var_value);
      if (!AssertInputValid(test_reaction, "reaction", var_type, var_value)) {
	return false;
      }
      new_requisite->AddReaction(test_reaction);
    }
    else if (var_name == "noreaction") {
      cReaction * test_reaction = reaction_lib.GetReaction(var_value);
      if (!AssertInputValid(test_reaction,"noreaction",var_type, var_value)) {
	return false;
      }
      new_requisite->AddNoReaction(test_reaction);
    }
    else if (var_name == "min_count") {
      if (!AssertInputInt(var_value, "min_count", var_type)) return false;
      new_requisite->SetMinTaskCount(var_value.AsInt());
    }
    else if (var_name == "max_count") {
      if (!AssertInputInt(var_value, "max_count", var_type)) return false;
      new_requisite->SetMaxTaskCount(var_value.AsInt());
    }
    else {
      cerr << "Error: Unknown requisite variable '" << var_name
	   << "' in reaction '" << reaction->GetName() << "'" << endl;
      return false;
    }
  }

  return true;
}


bool cEnvironment::LoadResource(cString desc)
{
  if (desc.GetSize() == 0) {
    cerr << "Warning: Resource line with no resources listed." << endl;
    return false;
  }

  while (desc.GetSize() > 0) {
    cString cur_resource = desc.PopWord();
    const cString name = cur_resource.Pop(':');
    cResource * new_resource = resource_lib.AddResource(name);

    while (cur_resource.GetSize() != 0) {
      cString var_entry = cur_resource.Pop(':');
      cString var_name;
      cString var_value;
      const cString var_type = cStringUtil::Stringf("resource '%s'", name());

      // Parse this entry.
      if (!ParseSetting(var_entry, var_name, var_value, var_type)) {
	return false;
      }

      if (var_name == "inflow") {
	if (!AssertInputDouble(var_value, "inflow", var_type)) return false;
	new_resource->SetInflow( var_value.AsDouble() );
      }
      else if (var_name == "outflow") {
	if (!AssertInputDouble(var_value, "outflow", var_type)) return false;
	new_resource->SetOutflow( var_value.AsDouble() );
      }
      else if (var_name == "initial") {
	if (!AssertInputDouble(var_value, "initial", var_type)) return false;
	new_resource->SetInitial( var_value.AsDouble() );
      }
      else if (var_name == "geometry") {
	if (!new_resource->SetGeometry( var_value )) {
          cerr << "Error: In " << var_type << "," << var_value << 
	          " unknown geometry" << endl;
          return false;
        }
      }
      else if (var_name == "inflowx1" || var_name == "inflowx") {
	if (!AssertInputInt(var_value, "inflowX1", var_type)) return false;
	new_resource->SetInflowX1( var_value.AsInt() );
      }
      else if (var_name == "inflowx2") {
	if (!AssertInputInt(var_value, "inflowX2", var_type)) return false;
	new_resource->SetInflowX2( var_value.AsInt() );
      }
      else if (var_name == "inflowy1" || var_name == "inflowy") {
	if (!AssertInputInt(var_value, "inflowY1", var_type)) return false;
	new_resource->SetInflowY1( var_value.AsInt() );
      }
      else if (var_name == "inflowy2") {
	if (!AssertInputInt(var_value, "inflowY2", var_type)) return false;
	new_resource->SetInflowY2( var_value.AsInt() );
      }
      else if (var_name == "outflowx1" || var_name == "outflowx") {
	if (!AssertInputInt(var_value, "outflowX1", var_type)) return false;
	new_resource->SetOutflowX1( var_value.AsInt() );
      }
      else if (var_name == "outflowx2") {
	if (!AssertInputInt(var_value, "outflowX2", var_type)) return false;
	new_resource->SetOutflowX2( var_value.AsInt() );
      }
      else if (var_name == "outflowy1" || var_name == "outflowy") {
	if (!AssertInputInt(var_value, "outflowY1", var_type)) return false;
	new_resource->SetOutflowY1( var_value.AsInt() );
      }
      else if (var_name == "outflowy2") {
	if (!AssertInputInt(var_value, "outflowY2", var_type)) return false;
	new_resource->SetOutflowY2( var_value.AsInt() );
      }
      else if (var_name == "xdiffuse") {
	if (!AssertInputDouble(var_value, "xdiffuse", var_type)) return false;
	new_resource->SetXDiffuse( var_value.AsDouble() );
      }
      else if (var_name == "xgravity") {
	if (!AssertInputDouble(var_value, "xgravity", var_type)) return false;
	new_resource->SetXGravity( var_value.AsDouble() );
      }
      else if (var_name == "ydiffuse") {
	if (!AssertInputDouble(var_value, "ydiffuse", var_type)) return false;
	new_resource->SetYDiffuse( var_value.AsDouble() );
      }
      else if (var_name == "ygravity") {
	if (!AssertInputDouble(var_value, "ygravity", var_type)) return false;
	new_resource->SetYGravity( var_value.AsDouble() );
      }
      else {
	cerr << "Error: Unknown variable '" << var_name
	     << "' in resource '" << name << "'" << endl;
	return false;
      }
    }

    // If there are valid values for X/Y1's but not for X/Y2's assume that 
    // the user is interested only in one point and set the X/Y2's to the
    // same value as 

    if (new_resource->GetInflowX1()>-99 && new_resource->GetInflowX2()==-99){
         new_resource->SetInflowX2(new_resource->GetInflowX1());
    }
    if (new_resource->GetInflowY1()>-99 && new_resource->GetInflowY2()==-99){
         new_resource->SetInflowY2(new_resource->GetInflowY1());
    }
    if (new_resource->GetOutflowX1()>-99 && new_resource->GetOutflowX2()==-99) {
         new_resource->SetOutflowX2(new_resource->GetOutflowX1());
    }
    if (new_resource->GetOutflowY1()>-99 && new_resource->GetOutflowY2()==-99) {
         new_resource->SetOutflowY2(new_resource->GetOutflowY1());
    }
  }

  return true;
}

bool cEnvironment::LoadReaction(cString desc)
{
  // Make sure this reaction has a description...
  if (desc.GetSize() == 0) {
    cerr << "Error: Each reaction must include a name and trigger." << endl;
    return false;
  }

  // Load in the reaction name
  const cString name = desc.PopWord();
  cReaction * new_reaction = reaction_lib.AddReaction(name);

  // If only a name was present, assume this reaction is a pre-declaration.
  if (desc.GetSize() == 0) {
    return true;
  }

  // Make sure this reaction hasn't already been loaded with a different
  // definition.
  if (new_reaction->GetTask() != NULL) {
    cerr << "Error: Re-defining reaction '" << name << "'." << endl;
    return false;
  }

  // Finish loading in this reaction.
  const cString trigger = desc.PopWord();

  // Load the task trigger
  cTaskEntry * cur_task = task_lib.AddTask(trigger);
  if (cur_task == NULL) {
    cerr << "...failed to find task in cTaskLib..." << endl;
    return false;
  }
  new_reaction->SetTask(cur_task);      // Attack task to reaction.

  while (desc.GetSize()) {
    cString desc_entry = desc.PopWord();      // Get the next argument
    cString entry_type = desc_entry.Pop(':'); // Determine argument type
    entry_type.ToLower();                     // Make case insensitive.

    // Determine the type of each argument and process it.
    if (entry_type == "process") {
      if (LoadReactionProcess(new_reaction, desc_entry) == false) {
	cerr << "...failed in loading reaction-process..." << endl;
	return false;
      }
    }
    else if (entry_type == "requisite") {
      if (LoadReactionRequisite(new_reaction, desc_entry) == false) {
	cerr << "...failed in loading reaction-requisite..." << endl;
	return false;
      }
    }
    else {
      cerr << "Unknown entry type '" << entry_type
  	   << "' in reaction '" << name << "'"
  	   << endl;
      return false;
    }
  }

  return true;
}

bool cEnvironment::LoadMutation(cString desc)
{
  // Make sure this mutation has a description...
  if (desc.CountNumWords() < 5) {
    cerr << "Error: Each mutation must include a name, trigger, scope, type, and rate." << endl;
    return false;
  }

  // Load in the mutation info
  const cString name = desc.PopWord().ToLower();
  const cString trigger = desc.PopWord().ToLower();
  const cString scope = desc.PopWord().ToLower();
  const cString type = desc.PopWord().ToLower();
  const double rate = desc.PopWord().AsDouble();

  int trig_id = -1;
  int scope_id = -1;
  int type_id = -1;

  if (trigger == "none") trig_id = MUTATION_TRIGGER_NONE;
  else if (trigger == "update") trig_id = MUTATION_TRIGGER_UPDATE;
  else if (trigger == "divide") trig_id = MUTATION_TRIGGER_DIVIDE;
  else if (trigger == "parent") trig_id = MUTATION_TRIGGER_PARENT;
  else if (trigger == "write") trig_id = MUTATION_TRIGGER_WRITE;
  else if (trigger == "read") trig_id = MUTATION_TRIGGER_READ;
  else if (trigger == "exec") trig_id = MUTATION_TRIGGER_EXEC;
  else {
    cerr << "Error: Unknown mutation trigger '" << trigger << "'." << endl;
    return false;
  }

  if (scope == "genome") scope_id = MUTATION_SCOPE_GENOME;
  else if (scope == "local") scope_id = MUTATION_SCOPE_LOCAL;
  else if (scope == "prop") scope_id = MUTATION_SCOPE_PROP;
  else if (scope == "global") scope_id = MUTATION_SCOPE_GLOBAL;
  else if (scope == "spread") scope_id = MUTATION_SCOPE_SPREAD;
  else {
    cerr << "Error: Unknown mutation scope '" << scope << "'." << endl;
    return false;
  }

  if (type == "point") type_id = MUTATION_TYPE_POINT;
  else if (type == "insert") type_id = MUTATION_TYPE_INSERT;
  else if (type == "delete") type_id = MUTATION_TYPE_DELETE;
  else if (type == "head_inc") type_id = MUTATION_TYPE_HEAD_INC;
  else if (type == "head_dec") type_id = MUTATION_TYPE_HEAD_DEC;
  else if (type == "temp") type_id = MUTATION_TYPE_TEMP;
  else if (type == "kill") type_id = MUTATION_TYPE_KILL;
  else {
    cerr << "Error: Unknown mutation type '" << type << "'." << endl;
    return false;
  }

  // Lets do a few checks for legal combinations...
  if (trig_id == MUTATION_TRIGGER_NONE) {
    cerr << "Warning: Mutations with trigger 'none' will never occur." << endl;
  }

  if (scope_id == MUTATION_SCOPE_LOCAL || scope_id == MUTATION_SCOPE_PROP) {
    if (trig_id == MUTATION_TRIGGER_DIVIDE) {
      cerr << "Error: Offspring after divide have no " << scope
	   << " for mutations." << endl;
      return false;
    }
    if (trig_id == MUTATION_TRIGGER_UPDATE ||
	trig_id == MUTATION_TRIGGER_PARENT) {
      cerr << "Warning: Mutation trigger " << trigger
	   << "has no natural positions; IP used." << endl;
    }
  }
  else {  // Genome-wide scope
    if (type_id == MUTATION_TYPE_HEAD_INC ||
	type_id == MUTATION_TYPE_HEAD_DEC ||
	type_id == MUTATION_TYPE_TEMP) {
      cerr << "Error: " << scope << " scope not compatible with type "
	   << type << "." << endl;
      return false;
    }
  }

  if (type_id == MUTATION_TYPE_TEMP) {
    if (trig_id == MUTATION_TRIGGER_UPDATE ||
	trig_id == MUTATION_TRIGGER_DIVIDE ||
	trig_id == MUTATION_TRIGGER_PARENT ||
	trig_id == MUTATION_TRIGGER_WRITE) {
      cerr << "Error: " << trigger << " trigger not meaningful with type "
	   << type << "." << endl;
      return false;
    }
  }

  // If we made it this far, it should be safe to build the mutation.
  mutation_lib.AddMutation(name, trig_id, scope_id, type_id, rate);

  return true;
}

bool cEnvironment::LoadSetActive(cString desc)
{
  cString item_type = desc.PopWord(); 
  item_type.ToUpper();

  cString item_name = desc.PopWord();

  cString item_active = desc.PopWord();
  item_active.ToUpper();

  bool new_active = true;
  if (item_active == "0" || item_active == "FALSE") new_active = false;

  if (item_type == "REACTION") {
    cReaction * cur_reaction = reaction_lib.GetReaction(item_name);
    if (cur_reaction == NULL) {
      cerr << "Unknown REACTION: '" << item_name << "'" << endl;
      return false;
    }
    cur_reaction->SetActive(new_active);
  } else if (item_type == "") {
    cerr << "Format: SET_ACTIVE <type> <name> <new_status=true>" << endl;
  } else {
    cerr << "Error: Cannot deactivate items of type "
	 << item_type << endl;
    return false;
  }

  return true;
}

bool cEnvironment::LoadLine(cString line) 
{
  cString type = line.PopWord();      // Determine type of this entry.
  type.ToUpper();                     // Make type case insensitive.

  bool load_ok = true;
  if (type == "RESOURCE") load_ok = LoadResource(line);
  else if (type == "REACTION") load_ok = LoadReaction(line);
  else if (type == "MUTATION") load_ok = LoadMutation(line);
  else if (type == "SET_ACTIVE") load_ok = LoadSetActive(line);
  else {
    cerr << "Error: Unknown environment keyword '" << type << "." << endl;
    return false;
  }
  
  if (load_ok == false) {
    cerr << "...failed in loading '" << type << "'..." << endl;
    return false;
  }

  return true;
}

bool cEnvironment::Load(const cString & filename)
{
  cInitFile infile(filename);
  if (infile.Good() == false) {
    cerr << "Error: Failed to load environment '" << filename << "'." << endl;
    return false;
  }

  infile.Load();
  infile.Close();
  infile.Compress();

  for (int line_id = 0; line_id < infile.GetNumLines(); line_id++) {
    // Load the next line from the file.
    bool load_ok = LoadLine(infile.GetLine(line_id));
    if (load_ok == false) return false;
  }

  // Make sure that all pre-declared reactions have been loaded correctly.
  for (int i = 0; i < reaction_lib.GetSize(); i++) {
    if (reaction_lib.GetReaction(i)->GetTask() == NULL) {
      cerr << "Error: Pre-declared reaction '"
	   << reaction_lib.GetReaction(i)->GetName()
	   << "' never defined." << endl;
      return false;
    }
  }

  return true;
}


void cEnvironment::SetupInputs( tArray<int> & input_array ) const
{
  input_array.Resize(3);

  // Set the top 8 bits of the input buffer...
  input_array[0] = 15 << 24;  // 00001111
  input_array[1] = 51 << 24;  // 00110011
  input_array[2] = 85 << 24;  // 01010101

  // And randomize the rest...
  for (int i = 0; i < 3; i++) {
    input_array[i] += g_random.GetUInt(1 << 24);
  }
}


bool cEnvironment::TestInput( cReactionResult & result,
			      const tBuffer<int> & inputs,
			      const tBuffer<int> & outputs,
			      const tArray<double> & resource_count ) const
{
  // @CAO nothing for the moment...
  return false;
}


bool cEnvironment::TestOutput( cReactionResult & result,
			       const tBuffer<int> & input_buf,
			       const tBuffer<int> & output_buf,
			       const tBuffer<int> & send_buf,
			       const tBuffer<int> & receive_buf,
			       const tArray<int> & task_count,
			       const tArray<int> & reaction_count,
			       const tArray<double> & resource_count,
			       const tList< tBuffer<int> > & input_buffers,
			       const tList< tBuffer<int> > & output_buffers) const
{
  // Do setup for reaction tests...
  task_lib.SetupTests(input_buf, output_buf, input_buffers, output_buffers);

  // Loop through all reactions to see if any have been triggered...
  const int num_reactions = reaction_lib.GetSize();
  for (int i = 0; i < num_reactions; i++) {
    cReaction * cur_reaction = reaction_lib.GetReaction(i);
    assert(cur_reaction != NULL);

    // Only use active reactions...
    if (cur_reaction->GetActive() == false) continue;

    // Examine the task trigger associated with this reaction
    cTaskEntry * cur_task = cur_reaction->GetTask();
    assert(cur_task != NULL);
    const double task_quality = task_lib.TestOutput(*cur_task);
    const int task_id = cur_task->GetID();

    // If this task wasn't performed, move on to the next one.
    if (task_quality == 0.0) continue;

    // Mark this task as performed...
    result.MarkTask(task_id);

    // Examine requisites on this reaction
    if (TestRequisites(cur_reaction->GetRequisites(), task_count[task_id],
		       reaction_count) == false) {
      continue;
    }

    // And lets process it!
    DoProcesses(cur_reaction->GetProcesses(), resource_count,
		task_quality, result);

    // Mark this reaction as occuring...
    result.MarkReaction(cur_reaction->GetID());
  }

  // Loop again to check receive tasks...
  // if (receive_buf.GetSize() != 0)
  {
    // Do setup for reaction tests...
    task_lib.
      SetupTests(receive_buf, output_buf, input_buffers, output_buffers);

    for (int i = 0; i < num_reactions; i++) {
      cReaction * cur_reaction = reaction_lib.GetReaction(i);
      assert(cur_reaction != NULL);
      
      // Only use active reactions...
      if (cur_reaction->GetActive() == false) continue;
      
      // Examine the task trigger associated with this reaction
      cTaskEntry * cur_task = cur_reaction->GetTask();
      assert(cur_task != NULL);
      const double task_quality = task_lib.TestOutput(*cur_task);
      const int task_id = cur_task->GetID();
      
      // If this task wasn't performed, move on to the next one.
      if (task_quality == 0.0) continue;
      
      // Mark this task as performed...
      result.MarkReceiveTask(task_id);
    }
  }

  return result.GetActive();
}


bool cEnvironment::TestRequisites(const tList<cReactionRequisite> & req_list,
		  int task_count, const tArray<int> & reaction_count) const
{
  const int num_reqs = req_list.GetSize();

  // If there are no requisites, there is nothing to meet!
  if (num_reqs == 0) return true;

  tConstListIterator<cReactionRequisite> req_it(req_list);
  for (int i = 0; i < num_reqs; i++) {
    // See if this requisite batch can be satisfied.
    const cReactionRequisite * cur_req = req_it.Next();
    bool satisfied = true;

    // Have all reactions been met?
    tConstListIterator<cReaction> reaction_it(cur_req->GetReactions());
    while (reaction_it.Next() != NULL) {
      int react_id = reaction_it.Get()->GetID();
      if (reaction_count[react_id] == 0) {
	satisfied = false;
	break;
      }
    }
    if (satisfied == false) continue;

    // Have all no-reactions been met?
    tConstListIterator<cReaction> noreaction_it(cur_req->GetNoReactions());
    while (noreaction_it.Next() != NULL) {
      int react_id = noreaction_it.Get()->GetID();
      if (reaction_count[react_id] != 0) {
	satisfied = false;
	break;
      }
    }
    if (satisfied == false) continue;

    // Have all task counts been met?
    if (task_count < cur_req->GetMinTaskCount()) continue;

    // Have all reactions been met?
    if (task_count >= cur_req->GetMaxTaskCount()) continue;

    return true;
  }

  return false;
}


void cEnvironment::DoProcesses(const tList<cReactionProcess> & process_list,
			       const tArray<double> & resource_count,
			       const double task_quality,
			       cReactionResult & result) const
{
  const int num_process = process_list.GetSize();

  tConstListIterator<cReactionProcess> process_it(process_list);
  for (int i = 0; i < num_process; i++) {
    // See if this requisite batch can be satisfied.
    const cReactionProcess * cur_process = process_it.Next();
    const double max_consumed = cur_process->GetMaxNumber();
    const double min_consumed = cur_process->GetMinNumber();

    // Determine resource consumption
    double consumed = 0.0;
    cResource * in_resource = cur_process->GetResource();

    // Test if infinite resource
    if (in_resource == NULL) {
      consumed = max_consumed;
    }

    // Otherwise we're using a finite resource
    else {
      const int res_id = in_resource->GetID();

      consumed = resource_count[res_id] - result.GetConsumed(res_id);
      consumed *= cur_process->GetMaxFraction();

      // Make sure we're not above the maximum consumption.
      if (consumed > max_consumed) consumed = max_consumed;

      // Test if we are below the minimum consumption.
      if (consumed < min_consumed) consumed = 0.0;

      // If we don't actually have any resource to consume stop here.
      if (consumed == 0.0) continue;

      // Mark in the results the resource consumed.
      result.Consume(res_id, consumed);
    }

    // Calculate the bonus
    double bonus = consumed * cur_process->GetValue() * task_quality;

    switch (cur_process->GetType()) {
    case REACTION_PROCTYPE_ADD:
      result.AddBonus(bonus);
      break;
    case REACTION_PROCTYPE_MULT:
      result.MultBonus(bonus);
      break;
    case REACTION_PROCTYPE_POW:
      result.MultBonus( pow(2.0, bonus) );
      break;
    default:
      assert(false);  // Should not get here!
      break;
    };

    // Determine detection events
    cResource * detected = cur_process->GetDetect();
    if (detected != NULL) {
      const int detected_id = detected->GetID();
      const double real_amount = resource_count[detected_id];
      double estimated_amount =
	   g_random.GetRandNormal(real_amount, cur_process->GetDetectionError()*real_amount);
      if (estimated_amount < cur_process->GetDetectionThreshold()) {
	result.Detect(detected_id, 0.0);		
      } else {
        result.Detect(detected_id, estimated_amount);
      }
    }

    // Determine byproducts
    cResource * product = cur_process->GetProduct();
    if (product != NULL) {
      int product_id = product->GetID();
      double product_size = consumed * cur_process->GetConversion();
      result.Produce(product_id, product_size);
    }

    // Determine what instructions should be run...
    const int inst_id = cur_process->GetInstID();
    if (inst_id >= 0) {
      result.AddInst(inst_id);
    }

    result.Lethal(cur_process->GetLethal());
  }
}

double cEnvironment::GetReactionValue(int & reaction_id)
{
  cReaction * found_reaction = reaction_lib.GetReaction(reaction_id);
  if (found_reaction == NULL) return false;
  return found_reaction->GetValue();
}

bool cEnvironment::SetReactionValue(const cString & name, double value)
{
  cReaction * found_reaction = reaction_lib.GetReaction(name);
  if (found_reaction == NULL) return false;
  found_reaction->ModifyValue(value);
  return true;
}

bool cEnvironment::SetReactionValueMult(const cString & name, double value_mult)
{
  cReaction * found_reaction = reaction_lib.GetReaction(name);
  if (found_reaction == NULL) return false;
  found_reaction->MultiplyValue(value_mult);
  return true;
}

bool cEnvironment::SetReactionInst(const cString & name, cString inst_name)
{
  cReaction * found_reaction = reaction_lib.GetReaction(name);
  if (found_reaction == NULL) return false;
  found_reaction->ModifyInst( inst_set.GetInst(inst_name).GetOp() );
  return true;
}


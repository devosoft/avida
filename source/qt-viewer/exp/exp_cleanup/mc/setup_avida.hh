#ifndef SETUP_AVIDA_HH
#define SETUP_AVIDA_HH

class cEnvironment;
class cPopulationInterface;

void ExitAvidaQtMultithreading(int exit_code);
void SetupAvidaQtMultithreading(
  int argc,
  char *argv[],
  cEnvironment &environment,
  cPopulationInterface &test_interface
);

#endif

// arch-tag: header file for avida multithreading setup function

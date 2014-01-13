To build Avida Ed for Windows, you'll need Visual Studio 2012 and CMake.

To build:
- Pull all the code from the various avida_ed/csharp branches.
- Use Cmake to build a Visual Studio 2012. Deselect Apto Dynamic (leave only AptoStatic).
- Open up the generated Solution file.
- Add avida_wrapper and avida_ed_csharp projects from viewer-windows.
- Build avida_ed_csharp. It should automatically build all the dependencies.
- ???
- PROFIT!

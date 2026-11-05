/** @file calcUF26Field.cxx

 @brief  example program to calculate the rms strength of the random
         Galactic magnetic field
 @return an integer 0 upon success

 command line parameters are

    <model name> <x> <y> <z>

 where "x/y/z" are the galactocentric coordinates in kpc (Earth at negative x,
 North at positive z) and "model" is either expDisk or ringDisk
 (see arXiv:2608.21293 for details)

 prints the rms strength of the random field in microgauss

 note: this is just a test program. If many evaluations are needed
 it is very inefficient to call this program repeatedly due to
 the overhead of starting a process. For that case it is better
 to implement a loop over positions.

 Please send bugs and suggestions to michael.unger@kit.edu and gf25@nyu.edu

 If you use this code, please cite arXiv:2608.21293

*/

#include "UF26Field.h"

#include <string>
#include <map>
#include <iostream>
#include <iomanip>

using namespace std;
using ModelType = UF26Field::ModelType;
int readCommandLine(const int, const char**, Vector3&, ModelType&);

int
main(const int argc, const char** argv)
{
  // read and check command line parameters
  Vector3 position;
  ModelType model;
  if (const int iError = readCommandLine(argc, argv, position, model))
    return iError;

  // evaluate the corresponding UF26Field model at requested position
  const UF26Field uf26Field(model);
  const double b = uf26Field(position);
  cout << scientific << setprecision(4)
       << "(x,y,z) = (" << position << ") kpc\n"
       << "b_rms   = " << b << " microgauss" << endl;
  return 0;
}

void
usage(const string& progName)
{
  cerr << " usage: " << progName << " <model name> <x> <y> <z> \n"
       << "        " << " galactocentric coordinates x/y/z in kpc\n"
       << "        " << " (Earth at negative x, North at positive z)\n"
       << "        " << " available models: ";
  for (const auto& m : UF26Field::GetModelNames())
    cerr << m.second << " ";
  cerr << endl;
}

int
readCommandLine(const int argc, const char** argv,
                Vector3& position, ModelType& model)
{
  map<string, ModelType> uf26Models;
  for (const auto& mn : UF26Field::GetModelNames())
    uf26Models[mn.second] = mn.first;

  if (argc < 5 || !uf26Models.count(argv[1])) {
    usage(argv[0]);
    return 1;
  }

  model = uf26Models.at(argv[1]);

  try {
    position.Set(stod(argv[2]),
                 stod(argv[3]),
                 stod(argv[4]));
  }
  catch (...) {
    usage(argv[0]);
    return 2;
  }
  return 0;
}

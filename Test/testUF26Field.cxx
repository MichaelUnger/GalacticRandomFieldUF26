/** @file testUF26Field.cxx

    @brief  test of UF26Field against reference values calculated with
            the code used in the UF26 paper (see makeReferenceValues.sh)
    @return 0 upon success

    usage: testUF26Field [reference file, default: Test/referenceValues.txt]

*/

#include "../UF26Field.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <map>
#include <sstream>
#include <string>

using namespace std;

int
main(const int argc, const char** argv)
{
  const string fileName = argc > 1 ? argv[1] : "Test/referenceValues.txt";
  ifstream in(fileName);
  if (!in) {
    cerr << " cannot open " << fileName << endl;
    return 1;
  }

  // reference values are given with six significant digits
  const double relTol = 2e-5;

  map<string, unsigned int> nTested;
  map<string, double> maxDeviation;
  unsigned int nFailed = 0;
  string line;
  while (getline(in, line)) {
    if (line.empty() || line[0] == '#')
      continue;
    istringstream iss(line);
    string model;
    Vector3 pos;
    double refVal;
    if (!(iss >> model >> pos.x >> pos.y >> pos.z >> refVal)) {
      cerr << " cannot parse line: " << line << endl;
      return 1;
    }
    const UF26Field uf26Field(model);
    const double val = uf26Field(pos);
    const double dev = abs(val - refVal) / max(abs(refVal), 1e-300);
    maxDeviation[model] = max(maxDeviation[model], dev);
    ++nTested[model];
    if (dev > relTol) {
      ++nFailed;
      cerr << scientific << setprecision(5)
           << " " << model << " at (" << pos << "): " << val
           << " not close to " << refVal << endl;
    }
  }

  if (nTested.empty()) {
    cerr << " no reference values found in " << fileName << endl;
    return 1;
  }
  for (const auto& m : UF26Field::GetModelNames()) {
    cout << " " << m.second << ": " << nTested[m.second] << " positions, "
         << "max. rel. deviation " << scientific << setprecision(1)
         << maxDeviation[m.second] << endl;
    if (!nTested[m.second]) {
      cerr << " no reference values for " << m.second << endl;
      ++nFailed;
    }
  }
  if (nFailed) {
    cerr << " ==> test of UF26Field FAILED" << endl;
    return 1;
  }
  cout << " ==> test of UF26Field successful" << endl;
  return 0;
}

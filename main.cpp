#include "include/searcher.h"

#include <iostream>


int main(int argc, char* argv[])
{
  namespace sp = searcher;
  using namespace std;

  sp::Primes primes = {2, 3, 5, 7, 9, 11, 13, 17, 19, 23};

  for (auto&[k, v] : sp::Factorizer{primes} .factorize(8 * 5 * 7 * 19 * 101)) {
    if (v > 0) {
      cout << k << "^" << v << endl;
    }
  }

  return 0;
}

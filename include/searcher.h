#ifndef SEARCHER_H
#define SEARCHER_H

#include <cmath>

#include <map>

#include <thread>
#include <future>

#include <functional>
#include <numeric>

#include <vector>

#include <iostream>


namespace searcher {

  using ull = unsigned long long;
  using Primes = std::vector<ull>;

  class Factorizer final {
      using Result = std::map<ull, ull>;

      Result _result;
      Primes _primes;

    public:

      explicit Factorizer(const Primes &pr) : _primes(pr) {}

      std::map<ull, ull> factorize(ull n)
      {
        _result.clear();
        std::cout << "init\t" << n << "\n\n";

        std::for_each(_primes.begin(), _primes.end(), [&n, this](const auto & p) {
          auto f {std::async(std::launch::async, power, n, p)};
          _result[p] = f.get();
        });

        if (const auto &&ext = must_extended(n); ext.first) {
          std::cout << "not enough prime numbers...\n";
          extend(ext.second);
        }

        return _result;
      }

    private:

      std::pair<bool, ull> must_extended(ull n)
      {
        ull tmp{1};
        std::for_each(_result.begin(), _result.end(), [&tmp](const auto kv) {
          tmp *= std::pow(kv.first, kv.second);
        });

        return {*(_primes.end() - 1) < (n / tmp), tmp};
      }

      void extend(ull bound)
      {
        auto tst {*(_primes.end() - 1) + 2};
        std::cout << bound << "; " << tst << "; " << bound % tst << std::endl;

        while (tst <= bound) {
          if (is_prime(tst)) {
            std::cout << "emplace\t" << tst << std::endl;
            _primes.emplace_back(tst);
          }

          tst += 2;
        }
      }

      bool is_prime(ull n)
      {
        std::all_of(_primes.begin(), _primes.end(), [&n](const auto & p) {
          return (n % p) != 0;
        });
      }

      static std::size_t power(ull n, ull delimiter)
      {
        std::size_t result{0};

        while (n % delimiter == 0) {
          result++;
          n /= delimiter;
        }

        return result;
      }
  };

}


#endif // SEARCHER_H

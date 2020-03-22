#ifndef SEARCHER_H
#define SEARCHER_H

#include <cmath>

#include <map>

#include <future>
#include <execution>

#include <functional>
#include <numeric>
#include <algorithm>

#include <vector>

#include <iostream>
#include <fstream>
#include <filesystem>


namespace searcher {

using ull = unsigned long long;
using Primes = std::vector<ull>;

template<typename Duration>
std::pair<float, std::string> adoptive_duration(Duration&& dur)
{
    if (dur>=1'000'000)
        return {dur/1'000'000., " sec"};
    else if (dur>=1'000)
        return {dur/1'000., " ms"};
    else
        return {dur, " microseconds"};
}

template<typename T>
void split(const std::basic_string<T>& str, T c, std::vector<std::basic_string<T>>& vec)
{
    using bs = std::basic_string<T>;
    typename bs::size_type i{0};
    typename bs::size_type j{str.find(c)};

    while (j!=bs::npos) {
        vec.emplace_back(str.substr(i, j-i));
        i = ++j;
        j = str.find(c, j);

        if (j==bs::npos)
            vec.emplace_back(str.substr(i, str.length()));
    }
}
class Primerizer final {

  public:
    Primerizer() = default;

    static std::vector<ull> load_primes(const std::filesystem::path& primes_file)
    {
        if (std::ifstream stream(primes_file); stream.is_open()) {
            std::vector<ull> res;
            std::string s;
            while (getline(stream, s)) {
                res.emplace_back(std::stoull(s));
            }

            std::cout << "\nLast prime number is " << s << std::endl;

            stream.close();
            return res;
        }
        else {
            std::cerr << "Enable to open file " << primes_file << std::endl;
        }

        return {};
    }

    static void save_primes(Primes&& primes, const std::filesystem::path& primes_file)
    {
        if (std::ofstream stream(primes_file); stream.is_open()) {
            for (auto&& p:primes) {
                stream << std::to_string(p) << std::endl;
            }

            stream.close();
        }
        else {
            std::cerr << "Enable to open file " << primes_file << std::endl;
        }
    }

};

class Factorizer final {
    using Result = std::map<ull, ull>;

    Result _result;
    Primes _primes;
    bool _extended = false;

  public:

    explicit Factorizer(Primes&& pr)
            :_primes(std::move(pr)) { }

    ~Factorizer()
    {
        if (_extended) {
            std::cout << "Prime list was be extended. Save it." << std::endl;
            searcher::Primerizer::save_primes(std::move(_primes), "primes.dat");
        }
    }

    Result factorize(ull n)
    {
        _result.clear();

        if (const auto f = std::find(_primes.begin(), _primes.end(), n); f!=_primes.end())
            return {{n, 1}};

        const auto start{std::chrono::steady_clock::now()};
        const auto end_{std::upper_bound(_primes.begin(), _primes.end(), n)};

        std::for_each(_primes.begin(), end_, [&n, this](const auto& p) {
          auto f{std::async(std::launch::deferred, power, n, p)};
          _result[p] = f.get();
        });

        if (const auto&& ext = must_extended(n); ext.first) {
            std::cout << "not enough prime numbers...\n";
            _extended = true;
            extend(n);
        }

        const auto end{std::chrono::steady_clock::now()};
        const auto elapsed_{std::chrono::duration_cast<std::chrono::microseconds>(end-start)};
        const auto[time, unit] = adoptive_duration(elapsed_.count());

        std::cout << "Elapsed time " << time << unit << std::endl;

        return _result;
    }

  private:

    std::pair<bool, ull> must_extended(ull n)
    {
        ull tmp{1};
        std::for_each(std::execution::par, _result.begin(), _result.end(), [&tmp](const auto kv) {
          tmp *= std::pow(kv.first, kv.second);
        });

        return {*(_primes.end()-1)<(n/tmp), tmp};
    }

    void extend(ull n)
    {
        auto tst{*(_primes.end()-1)+2};

        while (tst<=n) {
            if (is_prime(tst)) {
                _primes.emplace_back(tst);

                if (must_extended(n).second*tst==n) {
                    _result[tst]++;
                    break;
                }
            }

            tst += 2;
        }
    }

    bool is_prime(ull n)
    {
        return std::all_of(std::execution::par, _primes.begin(), _primes.end(), [&n](const auto& p) {
          return (n%p)!=0;
        });
    }

    static std::size_t power(ull n, ull delimiter)
    {
        std::size_t result{0};

        while (n%delimiter==0) {
            result++;
            n /= delimiter;
        }

        return result;
    }
};

}

#endif // SEARCHER_H

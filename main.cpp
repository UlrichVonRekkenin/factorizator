#include "include/searcher.h"
#include <iostream>


int main(int argc, char* argv[])
{
    namespace sp = searcher;

    std::cout << "Start reading prime number file...";
    static sp::Factorizer f(sp::Primerizer::load_primes(argv[1]));
    std::cout << "\nFinish.\n\nEnter number..." << std::endl;

    unsigned long long input{0};

    while (std::cin >> input) {
        for (auto&[k, v] : f.factorize(input)) {
            if (v>0) {
                std::cout << k << "^" << v << std::endl;
            }
        }

        std::cout << "Another one, please..." << std::endl;
    }

    return 0;
}

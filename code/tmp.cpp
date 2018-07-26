#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <fstream>
#include <iostream>
#include <string>

//---------------
int main() {
    std::ifstream cfileIn;
    cfileIn.open("1.tmp", std::ios::in);

    std::string s = "";
    cfileIn >> s;
    std::cout << s << std::endl;
    cfileIn >> s;
    std::cout << s << std::endl;
    cfileIn >> s;
    std::cout << s << std::endl;
    cfileIn >> s;
    std::cout << s << std::endl;
    cfileIn >> s;
    std::cout << s << std::endl;
    cfileIn >> s;
    std::cout << s << std::endl;
    cfileIn >> s;
    std::cout << s << std::endl;
    cfileIn.close();
}

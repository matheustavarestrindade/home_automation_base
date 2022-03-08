#include "uuid.h"

#include <stdlib.h>
#include <string.h>

#include <string>

void uuid_generate(char*& out) {
    std::string ret;
    for (int i = 0; i < 64; i++) {
        if (i > 0 && i % 5 == 0) {
            ret += '-';
        } else {
            ret += get_rand_char();
        }
    }
    out = new char[ret.length() + 1];

    strcpy(out, ret.c_str());
}

char get_rand_char() {
    static std::string charset("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
    return charset[rand() % charset.size()];
}


#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <fstream>
#include <cstring>

uint32_t my_ntohl(uint32_t a);

uint16_t my_ntohs(uint16_t a);

//Reads variable lenght argument (<delta-time>)
//Return (uint32_t)-1 if error happened
uint32_t readVarLenArg(std::ifstream &, int64_t &);

#endif

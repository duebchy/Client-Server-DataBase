#include <iostream>
#include <fstream>
#include "string"
#include "map"
#include "filesystem"
#include "iterator"
#include "algorithm"
#include "sstream"
#include "vector"
#include <cstring>
#include <winsock2.h>
#include <thread>
#pragma comment(lib, "ws2_32.lib")

class user {
    std::string password;
    std::string name;
    bool isAdmin;
};

using Data = std::map<std::string, std::string>;

Data data; //main data

std::map<std::string, int> COMMANDS = {
        {"/get", 1},
        {"/help", 2},
        {"/add", 3},
        {"/edit", 4},
        {"/delete", 5},
        {"/stop", 6},
        {"/show", 7}
};

const std::string bin_path = "C:/DataBase/Server/Server/Data/binlog.txt";

std::string binCommand;

void parse(std::string& str, bool bin);

SOCKET Connections[100];

int Counter = 0;



std::string command;
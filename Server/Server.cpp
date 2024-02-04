#include "includes.h"



void get_(std::string key) {


    if (data.contains(key)) {
        command = data[key];
    }
    else {
        command = "key doesn't exist";
    }
}

void show() {
    command = "";
    int i = 0;

    for (auto it = data.begin(); it != data.end(); it++)
    {
        if (i == 0) {

            command += it->first;
            i += 1;
            continue;
        }
        command += ", ";
        command += it->first;

    }
}


void help() {
    command = "/add, /del, /edit, /get, /show";
}
void add(std::string key = "", std::string val = "") {
    std::ofstream bin;
    if (key.empty() && val.empty()) {
        std::cin >> key >> val;
        bin.open(bin_path, std::ofstream::app);
        bin << binCommand << key << " " << val << "\n";
        binCommand = "";
    }
    if (data.contains(key)) {
        std::cerr << "key already exist";

    }

    else {

        data[key] = val;

        bin.close();
    }
}

void edit(std::string key = "", std::string val = "") {

    if (!data.contains(key)) {
        command = "key doesn't exist";
    }
    else
    {

        data[key] = val;

    }
}


void del(std::string key = "") {


    if (key.empty()) {
        std::ofstream bin;
        std::cin >> key;
        if (!data.contains(key)) {
            std::cerr << "key doesn't exist" << "\n";
        }
        
        data.erase(key);
        bin.open(bin_path, std::ofstream::app);
        bin << binCommand << key << '\n';
        binCommand = "";
        bin.close();
    }

    else {

        data.erase(key);

    }
}


//pars

void parse(std::string& str, bool bin) {


    std::string val1, val2;
    if (str[0] == '/' && str[1] == 'a') { //add
        int cnt = 5;
        while (str[cnt] != ' ') {
            val1 += str[cnt];
            cnt++;
        }
        while (cnt < str.length()) {
            cnt++;
            val2 += str[cnt];

        }


        add(val1, val2);
        if (bin == true) {
            std::ofstream file;
            file.open(bin_path, std::ofstream::app);
            if (!file.is_open()) {
                command = "unable to open file";
            }
            else {
                file << command << "\n";
                command = "";
                file.close();
            }
        }
    }
    else if (str[0] == '/' && str[1] == 'e' && str[2] == 'd' && str[3] == 'i' && str[4] == 't' && str[5] == ' ') { //edit
        int cnt = 6;
        while (str[cnt] != ' ') {
            if (str[cnt] == NULL) {
                command = "error";
                break;
            }


            val1 += str[cnt];
            cnt++;
        }
        while (cnt < str.length()) {
            if (str[cnt] == NULL) {
                command = "error";
                break;
            }
            cnt++;
            val2 += str[cnt];


        }
        edit(val1, val2);
        if (bin == true) {
            if (command != "key doesn't exist") {
                std::ofstream file;
                file.open(bin_path, std::ofstream::app);
                if (!file.is_open()) {
                    command = "unable to open file";
                }
                else {
                    file << command << "\n";
                    command = "";
                    file.close();
                }
            }
        }
    }
    else if (str[0] == '/' && str[1] == 'd' && str[4] == ' ') { //delete

        for (int i = 5; i < str.length(); i++) {
            val1 += str[i];

        }

        del(val1);
        if (bin == true) {
            std::ofstream file;
            file.open(bin_path, std::ofstream::app);
            if (!file.is_open()) {
                command = "unable to open file";
            }
            else {
                file << "/del " << val1 << "\n";
                command = "";
                file.close();
            }
        }
    }
    else if (str[0] == '/' && str[1] == 'g') { //get
        for (int i = 5; i < str.length(); i++) {
            val1 += str[i];

        }

        get_(val1);

    }
    else if (str[0] == '/' && str[1] == 'h') {
        command = "";
        help();
    }
    else if (str[0] == '/' && str[1] == 's') {

        show();
    }

    else {
        command = "unknown command";
    }
}





void init() {
    std::string str;
    std::ifstream file;
    file.open(bin_path, std::ifstream::app);
    if (!file.is_open()) {
        std::cerr << "unable to open binlog";
    }
    file.seekg(0, std::ios::beg);
    while (getline(file, str)) {
        parse(str, false);

    }

}

void ClientHandler(int index) {
    int msg_size;
    while (true) {
        int result = recv(Connections[index], (char*)&msg_size, sizeof(int), NULL);
        if (result <= 0) {
            std::cout << "Client disconnected.\n";
            closesocket(Connections[index]);
            break;
        }

        char* msg = new char[msg_size + 1];
        msg[msg_size] = '\0';
        result = recv(Connections[index], msg, msg_size, NULL);

        if (result <= 0) {
            std::cout << "Client disconnected.\n";
            closesocket(Connections[index]);
            break;
        }

        std::cout << msg << std::endl;

        command = msg;

        // command = "ABOBA, BOBR, KURWA!!!!"; обработка строки command отправляется клиенту!!!
        parse(command, true);




        msg_size = command.size();
        send(Connections[index], (char*)&msg_size, sizeof(int), NULL);
        send(Connections[index], command.c_str(), command.size(), NULL);

        delete[] msg;
    }
}

int main() {
    data.clear();
    init();


    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 1), &wsaData) != 0) {
        std::cerr << "Error" << std::endl;
        exit(1);
    }

    SOCKADDR_IN addr;
    int sizeofaddr = sizeof(addr);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(1111);
    addr.sin_family = AF_INET;

    SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);
    bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
    listen(sListen, SOMAXCONN);

    SOCKET newConnection;
    std::vector<std::thread> clientThreads;

    for (int i = 0; i < 100; i++) {
        newConnection = accept(sListen, (SOCKADDR*)&addr, &sizeofaddr);

        if (newConnection == INVALID_SOCKET) {
            std::cerr << "Error accepting connection.\n";
            closesocket(sListen);
            WSACleanup();
            return 1;
        }

        std::cout << "Client Connected!\n";

        std::string msg;
        int msg_size = msg.size();
        send(newConnection, (char*)&msg_size, sizeof(int), NULL);
        send(newConnection, msg.c_str(), msg_size, NULL);

        Connections[i] = newConnection;
        Counter++;
        clientThreads.emplace_back(ClientHandler, i);
    }

    // Дожидаемся завершения всех потоков
    for (auto& thread : clientThreads) {
        thread.join();
    }

    closesocket(sListen);
    WSACleanup();
    return 0;
}

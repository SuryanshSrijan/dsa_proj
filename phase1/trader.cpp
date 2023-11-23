#include "receiver.h"
#include <vector>
#include <map>

int ind = 0;
std::vector < std::vector <std::string>> data;

bool next(std::string message, std::string& str) {
    str = "";
    while(ind < message.size()) {
        char ch = message[ind++];
        if(ch == ' ') return 0;
        if(ch == '#') {
            if(message[ind] != '$') ind++;
            return 1;
        }
        str += ch;
    }
    return 1;
}

void solve1();

void solve2();

void solve3();

int main() {
    Receiver rcv;
    // sleep(5);
    std::string message = rcv.readIML();
    std::cout<<message<<std::endl;
    ind = 0;
    bool flag = 1;
    int part = 0;
    while(message[ind] != '$') {
        std::vector <std::string> mess;
        std::string temp = "";
        while(!next(message,temp)) mess.push_back(temp);
        mess.push_back(temp);
        if(flag) { part = mess.size() == 3?1:(mess.size() %2== 0?2:3); flag = 0;}
        data.push_back(mess);
    }
    switch(part) {
        case 1: solve1(); break;
        case 2: solve2(); break;
        case 3: solve3();
    }
    return 0;
}

void solve1() {
    for(auto x: data) {
        for(auto y: x) {
            std::cout<<y<<" ♥ ";
        }
        std::cout<<std::endl;
    }
}

void solve2() {
    for(auto x: data) {
        for(auto y: x) {
            std::cout<<y<<" ♦ ";
        }
        std::cout<<std::endl;
    }
}

void solve3() {
    for(auto x: data) {
        for(auto y: x) {
            std::cout<<y<<" ♣ ";
        }
        std::cout<<std::endl;
    }
}
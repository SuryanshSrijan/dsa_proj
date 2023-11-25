#include "receiver.h"
#include <vector>
#include <cstring>
#include <map>

int ind = 0;
std::vector < std::vector <std::string>> data;

bool next(std::string message, std::string& str) {
    str = "";
    while(ind < message.size()) {
        char ch = message[ind++];
        if(ch == ' ') return 0;
        if(ch == '#') return 1;
        if(ch>=32 && ch<127)
            str += ch;
    }
    return 1;
}

void solve1();

void solve2();

void solve3();

int main(int argc, char* argv[]) {
    Receiver rcv;
    // sleep(5);
    std::string message = rcv.readIML();
    while(message[message.size()-1] != '$') {
        std::string temp = rcv.readIML();
        message += temp;
    }
    rcv.terminate();
    ind = 0;
    while(message[ind] != '$') {
        std::vector <std::string> mess;
        std::string temp = "";
        while(!next(message,temp)) if(temp != "") mess.push_back(temp);
        mess.push_back(temp);
        data.push_back(mess);
    }
    switch(atoi(argv[1])) {
        case 1: solve1(); break;
        case 2: solve2(); break;
        case 3: solve3();
    }
    return 0;
}

namespace part1 {
    class stock{
      private:
        std::string name;
        int price;
        char last;
        int active_buy;
        bool active_buy_valid = 0;
        int active_sell;
        bool active_sell_valid = 0;

      public:
        void print(bool flag) {
            if(flag) std::cout<<name<<" "<<price<<" "<<((last == 'b')?'s':'b')<<std::endl;
            else std::cout<<"No Trade"<<std::endl;
        }

        stock(std::vector<std::string> store) {
            name = store[0];
            price = stoi(store[1]);
            last = store[2][0];
            print(1);
        }
        void process(std::vector<std::string> store) {
            int newprice = stoi(store[1]);
            char type = store[2][0];
            if(type == 'b') {
                if(active_sell_valid && active_sell == newprice) {
                    active_sell_valid = 0;
                    print(0);
                }
                else if(newprice > price) {
                    price = newprice;
                    last = type;
                    print(1);
                }
                else {
                    print(0);
                    if(active_buy_valid) {
                        if(newprice > active_buy) active_buy = newprice;
                    }
                    else {
                        active_buy = newprice;
                    }
                }
            }
            else {
                if(active_buy_valid && active_buy == newprice) {
                    active_buy_valid = 0;
                    print(0);
                }
                else if(newprice < price) {
                    price = newprice;
                    last = type;
                    print(1);
                }
                else {
                    print(0);
                    if(active_sell_valid) {
                        if(newprice < active_sell) active_sell = newprice;
                    }
                    else {
                        active_sell = newprice;
                    }
                }
            }
        }
    };
    std::vector <stock*> stocks;
};

void solve1() {
    std::map <std::string, int> index_map;
    int indexx = 0;
    for(int i=0;i<data.size();++i) {
        auto it = index_map.find(data[i][0]);
        if(it != index_map.end()){
            part1::stocks[it->second]->process(data[i]);
        }
        else {
            index_map[data[i][0]] = indexx++;
            part1::stock * temp = new part1::stock(data[i]);
            part1::stocks.push_back(temp);
        }
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
            std::cout<<y<<" ♥ ";
        }
        std::cout<<std::endl;
    }
}
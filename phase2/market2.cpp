#include "market.h"
#include "map.h"
#include <fstream>
#include <vector>
#include <string>

mapSI stocks;
mapSI brokers;

std::vector<std::vector<std::string>> data;

struct order{
    int st_time;
    int ed_time;
    int prc;
    int qnt;
    std::string owner;
};

struct order_heap {

    bool comp(const order a, const order b) {
        return a.prc < b.prc;
    }

    int sz;
    std::vector <order> store;
    order_heap() {sz = 0;}
    bool isEmpty() {return (sz==0);}
    void insert(order a) {
        store.push_back(a);
        for(int i=sz; i>0; i=(i-1)/2) {
            if(!comp(store[(i-1)/2],a)) break;
            store[i] = store[(i-1)/2];
            store[(i-1)/2] = a;
        }
        sz++;
    }
    void heapify(int i) {
        int c = i,l = 2*i+1, r = l+1;
        if(l < sz) {
            if(comp(store[i],store[l])) c = l;
        }
        if(r < sz) {
            if(comp(store[c],store[r])) c = r;
        }
        if(c == i) return;
        order temp = store[i];
        store[i] = store[c];
        store[c] = temp;
        heapify(c);
    }

    order max() {
        return store[0];
    }

    void remove(int i) {
        store[i].prc = 2147483647;
        while(i>0) {
            if(!comp(store[(i-1)/2],store[i])) break;
            order temp = store[i];
            store[i] = store[(i-1)/2];
            store[(i-1)/2] = temp;
            i = (i-1)/2;
        }
        --sz;
        order temp = store[sz];
        store[sz] = store[0];
        store[0] = temp;
        heapify(0);
    }

    void remove_expiry(int time) {
        for(int i=0;i<sz;++i) {
            if((store[i].ed_time != -1) && (store[i].ed_time<time)) remove(i);
        }
    }
};

struct stock{
    order_heap buy_orders;
    order_heap sell_orders;
};

struct broker {
    std::string name;

};
std::vector<stock> stock_vector;
market::market(int argc, char** argv)
{
    std::ifstream file("output.txt");
	std::string line;
    std::getline(file,line);
    while(std::getline(file,line)) {
        if(line.size() == 2 && line[0] == '!') break;
        std::vector<std::string> temp;
        std::string word = "";
        for(int i=0;i<line.length();++i) {
            if(line[i] == ' ') {
                if(word.size()) temp.push_back(word);
                word = "";
            }
            else if(line[i]>=32 && line[i]<127 && line[i] != '#' && line[i] != '$') word += line[i];
        }
        if(word.size()) temp.push_back(word);
        data.push_back(temp);
    }
}

void market::start()
{   
    int stock_index=0;
    int broker_index=0;
    int trades=0;
    int shares=0;

	for(int i=0;i<data.size();++i ) {
        // std::cout<<"HELLO: "<<i<<std::endl;
        int L = data[i].size();
        order ord;                   //created an order
        ord.st_time = stoi(data[i][0]);
        if(stoi(data[i][L-1]) == -1) ord.ed_time = -1;
        else ord.ed_time = stoi(data[i][L-1]) + ord.st_time;
        ord.qnt = stoi(data[i][L-2]);
        // std::string bro = data[i][0];
        ord.prc = stoi(data[i][L-3]);
        ord.owner = data[i][1];

        std::string stock_name;         //derived the stock name to put it into map stocks and to keep track of its index in vector stock_vector

        for(int j=3;j<=L-4;j++){
            stock_name+=data[i][j];  // still have to take care of cases like a 1 b 1 and b 1 a 1 as they are same
        }

        // std::cout<<stock_name<<" ♥ "<<std::endl;

        auto it=stocks.find(stock_name); //if not found then a completely new stock so input it into the database

        if(it==nullptr){
            struct stock st;
            if(data[i][2]=="SELL") {
                ord.prc *= -1;
                st.sell_orders.insert(ord);
            }
            else st.buy_orders.insert(ord);
            stock_vector.push_back(st);
            stocks.insert(stock_name,stock_index);
            stock_index++;
        }
        else{ //if found then look for trade
            int index=it->value;
            struct stock st = stock_vector[index]; //this is the stock of which the order is
            if(data[i][2]=="SELL"){
                st.buy_orders.remove_expiry(ord.st_time);
                if(st.buy_orders.isEmpty()) {
                    ord.prc *= -1;
                    st.sell_orders.insert(ord);
                    continue;
                }
                order best = st.buy_orders.max();
                if(ord.qnt <= 0) continue;
                if(best.prc < ord.prc) {
                    ord.prc *= -1;
                    st.sell_orders.insert(ord);
                    continue;
                }
                if(best.qnt > ord.qnt) {
                    trades++;
                    shares += ord.qnt;
                    best.qnt -= ord.qnt;
                    std::cout<<best.owner<<" purchased "<<ord.qnt<<" share of "<<stock_name<<" from "<<ord.owner<<" for $"<<best.prc<<"/share"<<std::endl;
                    continue;
                }
                else {
                    trades++;
                    shares += best.qnt;
                    st.buy_orders.remove(0);
                    ord.prc *= -1;
                    st.sell_orders.insert(ord);
                    std::cout<<best.owner<<" purchased "<<best.qnt<<" share of "<<stock_name<<" from "<<ord.owner<<" for $"<<best.prc<<"/share"<<std::endl;
                    continue;
                }
            }
            else{
                st.sell_orders.remove_expiry(ord.st_time);
                if(st.sell_orders.isEmpty()) {
                    st.buy_orders.insert(ord);
                    continue;
                }
                order best = st.sell_orders.max();
                if(ord.qnt <= 0) continue;
                if(best.prc + ord.prc < 0) {
                    st.buy_orders.insert(ord);
                    continue;
                }
                if(best.qnt > ord.qnt) {
                    trades++;
                    shares += ord.qnt;
                    best.qnt -= ord.qnt;
                    std::cout<<ord.owner<<" purchased "<<ord.qnt<<" share of "<<stock_name<<" from "<<best.owner<<" for $"<<(-1)*best.prc<<"/share"<<std::endl;
                    continue;
                }
                else {
                    trades++;
                    shares += best.qnt;
                    st.buy_orders.remove(0);
                    st.buy_orders.insert(ord);
                    std::cout<<ord.owner<<" purchased "<<best.qnt<<" share of "<<stock_name<<" from "<<best.owner<<" for $"<<(-1)*best.prc<<"/share"<<std::endl;
                    continue;
                }
            }
        }
    }
    std::cout<<std::endl<<"---End of Day---"<<std::endl;
    std::cout<<"Number of Completed Trades: "<<trades<<std::endl<<"Number of Shares Traded: "<<shares<<std::endl;
}

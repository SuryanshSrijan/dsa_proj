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
    void print() {
        std::cout<<st_time<<" "<<owner<<" "<<prc<<" "<<qnt<<" "<<ed_time<<std::endl;
    }
};

struct order_heap {

    bool comp(const order * a, const order * b) {
        return a->prc < b->prc;
    }

    int sz;
    std::vector <order*> store;
    order_heap() {sz = 0;}
    bool isEmpty() {return (sz==0);}
    void insert(order* a) {
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
        order * temp = store[i];
        store[i] = store[c];
        store[c] = temp;
        heapify(c);
    }

    order * max() {
        return store[0];
    }

    void remove(int i) {
        while(i>0) {
            order * temp = store[i];
            store[i] = store[(i-1)/2];
            store[(i-1)/2] = temp;
            i = (i-1)/2;
        }
        --sz;
        order * temp = store[sz];
        store[sz] = store[0];
        store[0] = temp;
        heapify(0);
    }

    void remove_expiry(int time) {
        for(int i=sz-1;i>=0;--i) {
            if((store[i]->ed_time != -1) && ((store[i]->ed_time)<time)) remove(i);
        }
    }
};

struct stock{
    order_heap* buy_orders;
    order_heap* sell_orders;
    stock() {
        buy_orders = new order_heap();
        sell_orders = new order_heap();
    }
};

struct broker {
    std::string name;
    int transfer, bought, sold;
    broker() {transfer = 0; bought = 0; sold = 0;}
};
std::vector <broker*> * broker_vector = new std::vector <broker*>();
std::vector <stock*> * stock_vector = new std::vector<stock*>();
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
    int total = 0;

	for(int i=0;i<data.size();++i ) {
        // std::cout<<"HELLO: "<<i<<std::endl;
        int L = data[i].size();
        order * ord = new order();                   //created an order
        ord->st_time = stoi(data[i][0]);
        if(stoi(data[i][L-1]) == -1) ord->ed_time = -1;
        else ord->ed_time = stoi(data[i][L-1]) + ord->st_time;
        ord->qnt = stoi(data[i][L-2]);
        // std::string bro = data[i][0];
        ord->prc = stoi(data[i][L-3]);
        ord->owner = data[i][1];

        std::string stock_name;         //derived the stock name to put it into map stocks and to keep track of its index in vector stock_vector

        for(int j=3;j<=L-4;j++){
            stock_name+=data[i][j];  // still have to take care of cases like a 1 b 1 and b 1 a 1 as they are same
        }
        auto it=stocks.find(stock_name); //if not found then a completely new stock so input it into the database

        if(it==nullptr){
            stock * st = new stock();
            if(data[i][2]=="SELL") {
                ord->prc *= -1;
                st->sell_orders->insert(ord);
            }
            else st->buy_orders->insert(ord);
            (*stock_vector).push_back(st);
            stocks.insert(stock_name,stock_index);
            stock_index++;
        }
        else{ //if found then look for trade
            int index=it->value;
            stock * st = (*stock_vector)[index]; //this is the stock of which the order is
            if(data[i][2]=="SELL"){
                st->buy_orders->remove_expiry(ord->st_time);
                if(st->buy_orders->isEmpty()) {
                    ord->prc *= -1;
                    st->sell_orders->insert(ord);
                    continue;
                }
                order * best = st->buy_orders->max();
                if(ord->qnt <= 0) continue;
                if(best->prc < ord->prc) {
                    ord->prc *= -1;
                    st->sell_orders->insert(ord);
                    continue;
                }
                bool flag = 0;
                while(best->qnt <= ord->qnt) {
                    trades++;
                    shares += best->qnt;
                    ord->qnt -= best->qnt;
                    int ind1,ind2;
                    auto it1 = brokers.find(best->owner);
                    if(!it1) {
                        brokers.insert(best->owner,broker_index++);
                        broker * temp = new broker();
                        temp->name = best->owner;
                        (*broker_vector).push_back(temp);
                    }
                    auto it2 = brokers.find(ord->owner);
                    if(!it2) {
                        brokers.insert(ord->owner,broker_index++);
                        broker * temp = new broker();
                        temp->name = ord->owner;
                        (*broker_vector).push_back(temp);
                    }
                    it1 = brokers.find(best->owner);
                    it2 = brokers.find(ord->owner);
                    (*broker_vector)[it1->value]->transfer -= (best->qnt)*(best->prc);
                    (*broker_vector)[it2->value]->transfer += (best->qnt)*(best->prc);
                    (*broker_vector)[it1->value]->bought += best->qnt;
                    (*broker_vector)[it2->value]->sold += best->qnt;
                    total += (best->qnt)*(best->prc);
                    std::cout<<best->owner<<" purchased "<<best->qnt<<" share of "<<stock_name<<" from "<<ord->owner<<" for $"<<best->prc<<"/share"<<std::endl;
                    st->buy_orders->remove(0);
                    if(st->buy_orders->isEmpty()) {
                        flag = 1;
                        ord->prc *= -1;
                        st->sell_orders->insert(ord);
                        break;
                    }
                    best = st->buy_orders->max();
                    if(best->prc < ord->prc) {
                        flag = 1;
                        ord->prc *= -1;
                        st->sell_orders->insert(ord);
                        break;
                    }
                }
                if(flag) continue;
                if(best->qnt > ord->qnt) {
                    trades++;
                    shares += ord->qnt;
                    best->qnt -= ord->qnt;
                    int ind1,ind2;
                    auto it1 = brokers.find(best->owner);
                    if(!it1) {
                        brokers.insert(best->owner,broker_index++);
                        broker * temp = new broker();
                        temp->name = best->owner;
                        (*broker_vector).push_back(temp);
                    }
                    auto it2 = brokers.find(ord->owner);
                    if(!it2) {
                        brokers.insert(ord->owner,broker_index++);
                        broker * temp = new broker();
                        temp->name = ord->owner;
                        (*broker_vector).push_back(temp);
                    }
                    it1 = brokers.find(best->owner);
                    it2 = brokers.find(ord->owner);
                    (*broker_vector)[it1->value]->transfer -= (ord->qnt)*(best->prc);
                    (*broker_vector)[it2->value]->transfer += (ord->qnt)*(best->prc);
                    (*broker_vector)[it1->value]->bought += ord->qnt;
                    (*broker_vector)[it2->value]->sold += ord->qnt;
                    total += (best->qnt)*(best->prc);
                    std::cout<<best->owner<<" purchased "<<ord->qnt<<" share of "<<stock_name<<" from "<<ord->owner<<" for $"<<best->prc<<"/share"<<std::endl;
                    continue;
                }
            }
            else{
                st->sell_orders->remove_expiry(ord->st_time);
                if(st->sell_orders->isEmpty()) {
                    st->buy_orders->insert(ord);
                // if(stock_name == "AMD") std::cout<<st->sell_orders->sz<<"♥♥"<<st->buy_orders->sz<<std::endl;
                    continue;
                }
                order * best = st->sell_orders->max();
                if(ord->qnt <= 0) continue;
                if(best->prc + ord->prc < 0) {
                    st->buy_orders->insert(ord);
                    continue;
                }
                bool flag = 0;
                while(best->qnt <= ord->qnt) {
                    trades++;
                    shares += best->qnt;
                    ord->qnt -= best->qnt;
                    int ind1,ind2;
                    auto it1 = brokers.find(best->owner);
                    if(!it1) {
                        brokers.insert(best->owner,broker_index++);
                        broker * temp = new broker();
                        temp->name = best->owner;
                        (*broker_vector).push_back(temp);
                    }
                    auto it2 = brokers.find(ord->owner);
                    if(!it2) {
                        brokers.insert(ord->owner,broker_index++);
                        broker * temp = new broker();
                        temp->name = ord->owner;
                        (*broker_vector).push_back(temp);
                    }
                    it1 = brokers.find(best->owner);
                    it2 = brokers.find(ord->owner);
                    (*broker_vector)[it1->value]->transfer -= (best->qnt)*(best->prc);
                    (*broker_vector)[it2->value]->transfer += (best->qnt)*(best->prc);
                    (*broker_vector)[it1->value]->sold += best->qnt;
                    (*broker_vector)[it2->value]->bought += best->qnt;
                    total -= (best->qnt)*(best->prc);
                    std::cout<<ord->owner<<" purchased "<<best->qnt<<" share of "<<stock_name<<" from "<<best->owner<<" for $"<<(-1)*best->prc<<"/share"<<std::endl;
                    st->sell_orders->remove(0);
                    if(st->sell_orders->isEmpty()) {
                        flag = 1;
                        st->buy_orders->insert(ord);
                        break;
                    }
                    best = st->sell_orders->max();
                    if(best->prc + ord->prc < 0) {
                        flag = 1;
                        st->buy_orders->insert(ord);
                        break;
                    }
                }
                if(flag) continue;
                if(best->qnt > ord->qnt) {
                    trades++;
                    shares += ord->qnt;
                    best->qnt -= ord->qnt;
                    int ind1,ind2;
                    auto it1 = brokers.find(best->owner);
                    if(!it1) {
                        brokers.insert(best->owner,broker_index++);
                        broker * temp = new broker();
                        temp->name = best->owner;
                        (*broker_vector).push_back(temp);
                    }
                    auto it2 = brokers.find(ord->owner);
                    if(!it2) {
                        brokers.insert(ord->owner,broker_index++);
                        broker * temp = new broker();
                        temp->name = ord->owner;
                        (*broker_vector).push_back(temp);
                    }
                    it1 = brokers.find(best->owner);
                    it2 = brokers.find(ord->owner);
                    (*broker_vector)[it1->value]->transfer -= (ord->qnt)*(best->prc);
                    (*broker_vector)[it2->value]->transfer += (ord->qnt)*(best->prc);
                    (*broker_vector)[it1->value]->sold += ord->qnt;
                    (*broker_vector)[it2->value]->bought += ord->qnt;
                    total -= (best->qnt)*(best->prc);
                    std::cout<<ord->owner<<" purchased "<<ord->qnt<<" share of "<<stock_name<<" from "<<best->owner<<" for $"<<(-1)*best->prc<<"/share"<<std::endl;
                    continue;
                }
            }
        }
    }
    std::cout<<std::endl<<"---End of Day---"<<std::endl;
    std::cout<<"Total Amount of Money Transferred: $"<<total<<std::endl;
    std::cout<<"Number of Completed Trades: "<<trades<<std::endl<<"Number of Shares Traded: "<<shares<<std::endl;
    for(int i = 0;i< broker_index;++i) {
        auto temp = (*broker_vector)[i];
        std::cout<<temp->name<<" bought "<<temp->bought<<" and sold "<<temp->sold<<" for a net transfer of $"<<temp->transfer<<std::endl;
    }
}

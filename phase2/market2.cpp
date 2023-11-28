#include "market.h"
#include "map.h"
#include <fstream>
#include <vector>
#include <string>
// #include <bits/stdc++.h>

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

struct stock{
    std::vector<order> buy_orders;
    std::vector<order> sell_orders;
};

struct broker {
    std::string name;

};
std::vector<stock> stock_vector;
// bool scmp(struct order a,struct order b){
//     if(a.prc!=b.prc) return a.prc>b.prc;
//     else{
//         if(a.st_time!=b.st_time) return a.st_time < b.st_time;
//         else{
//             return a.owner < b.owner;
//         }
//     }
// }
// bool bcmp(struct order a,struct order b){
//     if(a.prc!=b.prc) return a.prc < b.prc;
//     else{
//         if(a.st_time!=b.st_time) return a.st_time < b.st_time;
//         else{
//             return a.owner < b.owner;
//         }
//     }
// }

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
        struct order ord;                   //created an order
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

        auto it=stocks.find(stock_name); //if not found then a completely new stock so input it into the database

        if(it==nullptr){
            struct stock st;
            if(data[i][2]=="SELL") st.sell_orders.push_back(ord);
            else st.buy_orders.push_back(ord);
            stock_vector.push_back(st);
            stocks.insert(stock_name,stock_index);
            stock_index++;
        }
        else{ //if found then look for trade
            int index=it->value;
            struct stock st = stock_vector[index]; //this is the stock of which the order is
            if(data[i][2]=="SELL"){
                std::vector<int> dlt;
                // if(stock_name == "AMD"){
                //     std::cout<<"HEY"<<std::endl;
                //     // std::cout<<st.buy_orders[j].owner<<std::endl;
                // }
                // std::sort(st.buy_orders.begin(),st.buy_orders.end(),scmp);
                for(int j=0;j<st.buy_orders.size();j++){
                    if((st.buy_orders[j].ed_time!=-1) && (st.buy_orders[j].ed_time<ord.st_time)) dlt.push_back(j);
                    else{
                        if(ord.qnt<=0) break; //to handle irrelevant quantities
                        if(st.buy_orders[j].prc >= ord.prc){
                            trades++;
                            if(st.buy_orders[j].qnt <= ord.qnt){
                                shares += st.buy_orders[j].qnt;
                                ord.qnt -= st.buy_orders[j].qnt;
                                dlt.push_back(j);
                                std::cout<<"hi"<<std::endl;
                                std::cout<<st.buy_orders[j].owner<<" purchased "<<st.buy_orders[j].qnt<<" share of "<<stock_name<<" from "<<ord.owner<<" for $"<<st.buy_orders[j].prc<<"/share"<<std::endl;
                            }
                            else{
                                shares += ord.qnt;
                                std::cout<<"hii"<<std::endl;
                                std::cout<<st.buy_orders[j].owner<<" purchased "<<ord.qnt<<" share of "<<stock_name<<" from "<<ord.owner<<" for $"<<st.buy_orders[j].prc<<"/share"<<std::endl;
                                ord.qnt = 0;
                                break;
                            }
                        }
                    }
                }
                // for(int j=0;j<dlt.size();j++){
                //     // std::cout<<dlt[j]<<std::endl;
                //     st.buy_orders.erase(st.buy_orders.begin()+dlt[j]);
                // }
                if(ord.qnt>0){
                    st.sell_orders.push_back(ord);
                    // std::cout<<st.sell_orders.size()<<std::endl;
                }
            }
            else{
                // std::cout<<"hi"<<std::endl;
                std::vector<int> dlt;
                // std::sort(st.sell_orders.begin(),st.sell_orders.end(),bcmp);
                for(int j=0;j<st.sell_orders.size();j++){
                    if(stock_name=="AMD"){
                    std::cout<<st.sell_orders.size()<<std::endl;
                    // std::cout<<st.sell_orders[0].owner<<std::endl;
                    std::cout<<"HEY"<<std::endl;
                    }
                    // std::cout<<st.sell_orders.size()<<std::endl;
                    if((st.sell_orders[j].ed_time!=-1) && (st.sell_orders[j].ed_time<ord.st_time)) dlt.push_back(j);
                    else{
                        if(ord.qnt<=0) break; //to handle irrelevant quantities
                        // std::cout<<"hi"<<std::endl;
                        if(st.sell_orders[j].prc <= ord.prc){
                            trades++;
                            if(st.sell_orders[j].qnt <= ord.qnt){
                                shares += st.sell_orders[j].qnt;
                                ord.qnt -= st.sell_orders[j].qnt;
                                dlt.push_back(j); 
                                std::cout<<"hiii"<<std::endl;
                                std::cout<<ord.owner<<" purchased "<<st.sell_orders[j].qnt<<" share of "<<stock_name<<" from "<<st.sell_orders[j].owner<<" for $"<<st.sell_orders[j].prc<<"/share"<<std::endl;
                            }
                            else{
                                shares += ord.qnt;
                                std::cout<<"hiiii"<<std::endl;
                                std::cout<<st.sell_orders[j].owner<<" purchased "<<ord.qnt<<" share of "<<stock_name<<" from "<<ord.owner<<" for $"<<st.sell_orders[j].prc<<"/share"<<std::endl;
                                ord.qnt = 0;
                                break;
                            }
                        }
                    }
                }
                // for(int j=0;j<dlt.size();j++){
                //     // std::cout<<dlt[j]<<std::endl;
                //     st.sell_orders.erase(st.sell_orders.begin()+dlt[j]);
                // }
                if(ord.qnt>0){
                    st.buy_orders.push_back(ord);
                    // std::cout<<st.buy_orders.size()<<std::endl;
                }
            }
        }
    }
}

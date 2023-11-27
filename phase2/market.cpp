#include "market.h"
#include "map.h"
#include <fstream>
#include <vector>
#include <string>

mapSI stocks;
mapSI broker;

std::vector<std::vector<std::string>> data;

struct stock{
    int price;
    std::vector <int> offer;
};

struct broker {
    std::string name;

};

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
	for(int i=0;i<data.size();++i ) {
        int L = data[i].size();
        int sttime = stoi(data[i][0]);
        int endtime = stoi(data[i][L-1]);
        int quant = stoi(data[i][L-2]);
        std::string bro = data[i][0];
    }
}

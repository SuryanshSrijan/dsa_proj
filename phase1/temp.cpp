#include "receiver.h"
#include <vector>
#include <cstring>
#include "map.h"
#include <cmath>
#include <cassert>

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
    mapSI index_map;
    int indexx = 0;
    for(int i=0;i<data.size();++i) {
        auto it = index_map.find(data[i][0]);
        if(it != nullptr){
            part1::stocks[it->value]->process(data[i]);
        }
        else {
            index_map.insert(data[i][0],indexx++);
            part1::stock * temp = new part1::stock(data[i]);
            part1::stocks.push_back(temp);
        }
    }
}

namespace part2{

    double eps = 1e-5;

    std::vector<double> vec_mul(std::vector<std::vector<double>> A, std::vector<double> B) {
        int n = A.size();
        int d = A[0].size();
        std::vector<double> ret_val(n,0);
        for(int i = 0; i < n; ++i) {
            for(int j = 0; j < d; ++j) {
                ret_val[i] += A[i][j]*B[j];
            }
        }
        return ret_val;
    }

    std::vector<std::vector<double>> transpose(std::vector<std::vector<double>> A) {
        std::vector<std::vector<double>> B(A[0].size(), std::vector<double>(A.size()));
        for(int i=0;i<A.size();++i) {
            for(int j=0;j<B.size();++j) B[j][i] = A[i][j];
        }
        return B;
    }

    std::vector<std::vector<double>> matrix_mul(std::vector<std::vector<double>> A, std::vector<std::vector<double>> B) {
        int n = A.size();
        int k = A[0].size();
        int d = B[0].size();
        std::vector<std::vector<double>> ret_val(n, std::vector<double> (d,0));
        for(int i = 0; i < n; ++i) {
            for(int j = 0; j < d; ++j) {
                for(int p = 0; p < k; ++p) {
                    ret_val[i][j] += A[i][p] * B[p][j];
                }
            }
        }
        return ret_val;
    }
    
    std::vector<std::vector<double>> matrix_inverse(std::vector<std::vector<double>> matr) {
        int n = matr.size();
        std::vector<std::vector<double>> inv(n, std::vector<double> (n,0));
        for(int i=0;i<n;++i) inv[i][i] = 1;
        for (int i = 0; i < n; i++) {
            if(std::abs(matr[i][i]) < 1e-10) {
                return std::vector<std::vector<double>> temp(n, std::vector<double> (n,0));
            }
            double diag = matr[i][i];
            for (int j = 0; j < n; j++) {
                matr[i][j] /= diag;
                inv[i][j] /= diag;
            }
            for (int j = 0; j < n; j++) {
                if (i != j) {
                    double ratio = matr[j][i];
                    for (int k = 0; k < n; k++) {
                        matr[j][k] -= ratio * matr[i][k];
                        inv[j][k] -= ratio * inv[i][k];
                    }
                }
            }
        }
        return inv;
    }

    std::vector<int> valid_arbitrage(std::vector<std::vector<double>> A, std::vector<double> B) {
        for(int i=0;i<B.size();++i) B[i] *= -1;
        std::vector<double> coeff = vec_mul(matrix_mul(matrix_inverse(matrix_mul(A,transpose(A))),A),B);
        bool flag = 1;
        for(int i=0;i<coeff.size();++i) {
            if(fabs(coeff[i])>eps && fabs(coeff[i]-1)>eps ) flag = 0;
        }
        std::vector <int> ans;
        if(flag) for(int i=0;i<coeff.size();++i) if(fabs(coeff[i]-1) <= eps) ans.push_back(i);
        return ans;
    }


};

void solve2() {
    mapSI index_map;
    std::vector <std::vector<double>> lcombs;
    std::vector <int> price;
    std::vector <bool> type;
    int indexx = 0, num_l = 0;
    for(int i=0;i<data.size();++i) {
        int L = data[i].size();
        for(int j=0;j<(L-2);j+=2) {
            auto it = index_map.find(data[i][j]);
            if(it == nullptr) {
                index_map.insert(data[i][j],indexx++);
                for(int k=0;k<lcombs.size();++k) lcombs[k].push_back(0);
            }
        }
        price.push_back(stoi(data[i][L-2]));
        type.push_back(((data[i][L-1][0] == 'b')?0:1));
        std::vector<double> temp(indexx,0);
        for(int j=0;j<(L-2);j+=2){
            temp[index_map.find(data[i][j])->value]=stoi(data[i][j+1])*((data[i][L-1]=='s')?-1:1);
        }
        lcombs.push_back(temp);
        price.pb(stoi(data[i][L-2]));
        type.pb((data[i][L-1]=='b')?0:1);
        num_l++;
        for(int j=0;j<num_l-1;j++){
            bool flag=true;
            for(int k=0;k<lcombs[j].size();k++){
                if(lcombs[j][k]!=(data[i][L-1]=='s')?(-1*lcombs[num_l-1][k]):(lcombs[num_l-1][k])) flag=false;
            }
            if(flag){
                if(data[i][L-1]=='b' && type[j]==0) {
                    if(stoi(data[i][L-2])<price[j]) price[j]=
                }
            }
        }
        // check cancellation
    }
}

void solve3() {
    mapSI index_map;
    std::vector <std::vector<double>> lcombs;
    std::vector <int> price;
    std::vector <bool> type;
    int indexx = 0, num_l = 0;
    for(int i=0;i<data.size();++i) {
        int L = data[i].size();
        for(int j=0;j<(L-3);j+=2) {
            auto it = index_map.find(data[i][j]);
            if(it == nullptr) {
                index_map.insert(data[i][j],indexx++);
                for(int k=0;k<lcombs.size();++k) lcombs[k].push_back(0);
            }
        }
        price.push_back(stoi(data[i][L-3]));
        type.push_back(((data[i][L-1][0] == 'b')?0:1));
        std::vector<double> temp(indexx,0);
        // check cancellation
        for(int j=0;j<(L-3);j+=2){
            temp[index_map.find(data[i][j])->value]=stoi(data[i][j+1])*((data[i][L-1]=='s')?-1:1);
        }
    }
}
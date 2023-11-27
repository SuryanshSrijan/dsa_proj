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
            if(fabs(matr[i][i]) < 1e-10) {
                std::vector<std::vector<double>> tempp(n, std::vector<double> (n,0));
                return tempp;
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
        ans.push_back(flag);
        if(flag) for(int i=0;i<coeff.size();++i) if(fabs(coeff[i]-1) <= eps) ans.push_back(i);
        return ans;
    }


};

void solve2() {
    mapSI index_map;
    std::vector <std::vector<double>> lcombs;
    std::vector <std::string> names;
    std::vector <int> price;
    std::vector <bool> type;
    int indexx = 0, num_l = 0;
    int total = 0;
    for(int i=0;i<data.size();++i) {
        bool trade = 0;
        int L = data[i].size();
        for(int j=0;j<(L-2);j+=2) {
            auto it = index_map.find(data[i][j]);
            if(it == nullptr) {
                index_map.insert(data[i][j],indexx++);
                names.push_back(data[i][j]);
                for(int k=0;k<num_l;++k) lcombs[k].push_back(0);
            }
        }
        int currprice = stoi(data[i][L-2]);
        bool currtype = ((data[i][L-1][0] == 'b')?0:1);
        std::vector<double> temp(indexx,0);
        for(int j = 0;j<(L-2);j+=2) {
            temp[index_map.find(data[i][j])->value] = stoi(data[i][j+1])*(currtype?-1:1);
        }
        if(i==0) {
            std::cout<<"No Trade"<<std::endl;
            lcombs.push_back(temp);
            type.push_back(currtype);
            price.push_back(currprice);
            num_l++;
            continue;
        }
        bool flag = 0;
        for(int j=0;j<num_l;++j) {
            flag = 1;
            for(int k=0;k<indexx;++k) {
                if(temp[k]*(currtype?-1:1) != lcombs[j][k]) {
                    flag = 0;
                    break;
                }
            }
            if(!flag) continue;
            if(type[j] != currtype) {
                //remove both
                lcombs.erase(lcombs.begin()+j);
                price.erase(price.begin()+j);
                type.erase(type.begin()+j);
                num_l--;
            }
            else {
                if(currtype == 'b') {
                    if(currprice >= price[j]) {
                        price[j] = currprice;
                    }
                }
                else {
                    if(currprice <= price[j]){
                        price[j] = currprice;
                    }
                }
            }
            break;
        }
        if(flag) {
            std::cout<<"No Trade"<<std::endl;
            continue;
        }
        std::vector<int> ans = part2::valid_arbitrage(lcombs,temp);
        if(ans.size()==1) {
            if(ans[0]) {
                int bestprice = -100000000, besta = 0;
                for(int a = 0;a<num_l;++a) {
                    std::vector<std::vector<double>> lc(num_l-1, std::vector<double>(indexx));
                    for(int b=0;b<num_l;++b) {
                        if(b == a )continue;
                        for(int c=0;c<indexx;++c) lc[b-(b>a?1:0)][c] = lcombs[b][c];
                    }
                    std::vector<int> nans = part2::valid_arbitrage(lc,temp);
                    if(nans.size()==1) continue;
                    int c_price = currprice*(currtype?-1:1);
                    for(int t = nans.size()-1;t>0;--t) {
                        nans[t] += (nans[t]>=a?1:0);
                        int x= nans[t];
                        c_price += price[x]*(type[x]?-1:1);
                    }
                    if(c_price>bestprice) {bestprice = c_price; besta = a; ans = nans;}
                }
                if(bestprice <= 0) {
                    std::cout<<"No Trade"<<std::endl;
                    lcombs.push_back(temp);
                    type.push_back(currtype);
                    price.push_back(currprice);
                    num_l++;
                    continue;
                }
            }
            else {
                std::cout<<"No Trade"<<std::endl;
                lcombs.push_back(temp);
                type.push_back(currtype);
                price.push_back(currprice);
                num_l++;
                continue;
            }
        }
        int total_price = currprice*(currtype?-1:1);
        for(int t = ans.size()-1;t>0;--t) {
            total_price += price[ans[t]]*(type[ans[t]]?-1:1);
        }
        if(total_price > 0) {
            for(int k=0;k<indexx;++k) {
                if(temp[k]) std::cout<<names[k]<<" "<<temp[k]*(currtype?-1:1)<<" ";
            }
            std::cout<<currprice<<" "<<(currtype?'b':'s')<<std::endl;
            for(int t = ans.size()-1;t>0;--t) {
                int j = ans[t];
                // std::cout<<j<<std::endl;
                for(int k = 0;k<indexx;++k) {
                    if(lcombs[j][k]) {
                        std::cout<<names[k]<<" "<<lcombs[j][k]*(type[j]?-1:1)<<" ";
                    }
                    // lcombs.erase(lcombs.begin()+j);
                    // price.erase(price.begin()+j);
                    // type.erase(type.begin()+j);
                    // num_l--;
                }
                std::cout<<price[j]<<" "<<(type[j]?'b':'s')<<std::endl;
            }
            total += total_price;
        }
        else {
            std::cout<<"No Trade"<<std::endl;
        }
        for(int t = ans.size()-1;t>0;--t) {
            // for(int j=0;j<num_l;++j) {
            //     for(int k=0;k<indexx;++k) {
            //         std::cout<<lcombs[j][k]<<" ";
            //     }
            //     std::cout<<std::endl;
            // }
            int j = ans[t];
            lcombs.erase(lcombs.begin()+j);
            price.erase(price.begin()+j);
            type.erase(type.begin()+j);
            num_l--;   
        }
    }
    std::cout<<total<<std::endl;
}

namespace part3{

    struct pair{
        int a,b;
    };

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
            if(fabs(matr[i][i]) < 1e-10) {
                std::vector<std::vector<double>> tempp(n, std::vector<double> (n,0));
                return tempp;
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

    std::vector<pair*> valid_arbitrage(std::vector<std::vector<double>> A, std::vector<double> B, int currquant, std::vector<int> quant) {
        for(int i=0;i<B.size();++i) B[i] *= -1;
        std::vector<double> coeff = vec_mul(matrix_mul(matrix_inverse(matrix_mul(A,transpose(A))),A),B);
        bool flag = 1;
        for(int i=0;i<coeff.size();++i) {
            if(fabs(quant[i]*coeff[i]-round(quant[i]*coeff[i]))>eps ) flag = 0;
        }
        std::vector <pair*> ans;
        pair* temp = new pair();
        temp->a = flag;
        temp->b = 0;
        ans.push_back(temp);
        if(flag) for(int i=0;i<coeff.size();++i) if(fabs(coeff[i]-1) <= eps) {
            pair* temp = new pair();
            temp->a = i;
            temp->b = round(quant[i]*coeff[i]);
            ans.push_back(temp);
        }
        return ans;
    }
};

void solve3() {
    mapSI index_map;
    std::vector <std::vector<double>> lcombs;
    std::vector <std::string> names;
    std::vector <int> price;
    std::vector <int> quant;
    std::vector <bool> type;
    int indexx = 0, num_l = 0;
    int total = 0;
    for(int i=0;i<data.size();++i) {
        bool trade = 0;
        int L = data[i].size();
        for(int j=0;j<(L-3);j+=2) {
            auto it = index_map.find(data[i][j]);
            if(it == nullptr) {
                index_map.insert(data[i][j],indexx++);
                names.push_back(data[i][j]);
                for(int k=0;k<num_l;++k) lcombs[k].push_back(0);
            }
        }
        int currprice = stoi(data[i][L-3]);
        int currquant = stoi(data[i][L-2]);
        bool currtype = ((data[i][L-1][0] == 'b')?0:1);
        std::vector<double> temp(indexx,0);
        for(int j = 0;j<(L-2);j+=2) {
            temp[index_map.find(data[i][j])->value] = stoi(data[i][j+1])*(currtype?-1:1);
        }
        if(i==0) {
            std::cout<<"No Trade"<<std::endl;
            lcombs.push_back(temp);
            type.push_back(currtype);
            price.push_back(currprice);
            num_l++;
            continue;
        }
        bool flag = 0;
        for(int j=0;j<num_l;++j) {
            flag = 1;
            for(int k=0;k<indexx;++k) {
                if(temp[k]*(currtype?-1:1) != lcombs[j][k]) {
                    flag = 0;
                    break;
                }
            }
            if(!flag) continue;
            if(type[j] != currtype && price[j] == currprice) {
                //remove both
                if(quant[j] == currquant) {
                    lcombs.erase(lcombs.begin()+j);
                    price.erase(price.begin()+j);
                    type.erase(type.begin()+j);
                    num_l--;
                }
                else {
                    if(quant[j] > currquant) quant[j] -= currquant;
                    else {
                        quant[j] = currquant -quant[j];
                        type[j] = !type[j];
                        for(int k=0;k<indexx;++k) lcombs[j][k] *= -1;
                    }
                }
            }
            break;
        }
        if(flag) {
            std::cout<<"No Trade"<<std::endl;
            continue;
        }
        std::vector<part3::pair> ans = part2::valid_arbitrage(lcombs,temp,currquant,quant);
        if(ans.size()==1) {
            if(ans[0]->a) {
                int bestprice = -100000000, bestq = 0;
                for(int a = 0;a<num_l;++a) {
                    std::vector<std::vector<double>> lc(num_l-1, std::vector<double>(indexx));
                    std::vector<int> nquant(num_l-1);
                    for(int b=0;b<num_l;++b) {
                        if(b == a )continue;
                        nquant[b-(b>a?1:0)] = quant[b];
                        for(int c=0;c<indexx;++c) lc[b-(b>a?1:0)][c] = lcombs[b][c];
                    }
                    std::vector<part3::pair> nans = part2::valid_arbitrage(lc,temp,currquant,nquant);
                    if(nans.size()==1) continue;
                    int cq = 10000000;
                    for(int t = nans.size()-1;t>0;--t) {
                        nans[t]->a += ((nans[t]->a)>=a?1:0);
                        int x= nans[t]->a;
                        cq = min(cq,nans[t]->b);
                    }
                    int c_price = currprice*(currtype?-1:1);
                    for(int t = nans.size()-1;t>0;--t) {
                        int x= nans[t]->a;
                        c_price += price[x]*(type[x]?-1:1)*cq;
                    }
                    if(c_price>bestprice) {bestprice = c_price; bestq = cq; ans = nans;}
                }
                if(bestprice <= 0) {
                    std::cout<<"No Trade"<<std::endl;
                    lcombs.push_back(temp);
                    type.push_back(currtype);
                    price.push_back(currprice);
                    num_l++;
                    continue;
                }
            }
            else {
                std::cout<<"No Trade"<<std::endl;
                lcombs.push_back(temp);
                type.push_back(currtype);
                price.push_back(currprice);
                num_l++;
                continue;
            }
        }
        int total_quantity = 100000000;
        int total_price = currprice*(currtype?-1:1);
        for(int t = ans.size()-1;t>0;--t) {
            total_price += price[ans[t]]*(type[ans[t]]?-1:1);
        }
        if(total_price > 0) {
            for(int k=0;k<indexx;++k) {
                if(temp[k]) std::cout<<names[k]<<" "<<temp[k]*(currtype?-1:1)<<" ";
            }
            std::cout<<currprice<<" "<<(currtype?'b':'s')<<std::endl;
            for(int t = ans.size()-1;t>0;--t) {
                int j = ans[t];
                // std::cout<<j<<std::endl;
                for(int k = 0;k<indexx;++k) {
                    if(lcombs[j][k]) {
                        std::cout<<names[k]<<" "<<lcombs[j][k]*(type[j]?-1:1)<<" ";
                    }
                    // lcombs.erase(lcombs.begin()+j);
                    // price.erase(price.begin()+j);
                    // type.erase(type.begin()+j);
                    // num_l--;
                }
                std::cout<<price[j]<<" "<<(type[j]?'b':'s')<<std::endl;
            }
            total += total_price;
        }
        else {
            std::cout<<"No Trade"<<std::endl;
        }
        for(int t = ans.size()-1;t>0;--t) {
            // for(int j=0;j<num_l;++j) {
            //     for(int k=0;k<indexx;++k) {
            //         std::cout<<lcombs[j][k]<<" ";
            //     }
            //     std::cout<<std::endl;
            // }
            int j = ans[t];
            lcombs.erase(lcombs.begin()+j);
            price.erase(price.begin()+j);
            type.erase(type.begin()+j);
            num_l--;   
        }
    }
    std::cout<<total<<std::endl;
}
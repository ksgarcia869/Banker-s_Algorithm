#ifndef customer_h
#define customer_h
#include <iostream>
#include <iomanip>
#include <random>
#include "bank.h"
#include "ext_vector.h"

extern pthread_mutex_t mutex_;

class Bank;
class Customer {
  static int id;
private:
  ext_vector<int> need;
  ext_vector<int> alloc;
  ext_vector<int> max_;
  Bank* bank;
  int idx;
  pthread_t thread_id;
  std::random_device rd;
  std::mt19937 gen;
  std::uniform_real_distribution<> dis;

public:
    Customer(int index, ext_vector<int>& allocate, ext_vector<int>& maximum,Bank* bank_ = nullptr): idx(index), max_(maximum), need(max_ - alloc), bank(bank_){
      gen = std::mt19937(rd());
      dis = std::uniform_real_distribution<>(0,1);
    }
    int get_id() const{return idx;}
    pthread_t* get_threadid(){return &thread_id;}
    Bank* get_bank() const{return bank;}
    ext_vector<int> get_max() const{return max_;}

    bool needs_met() const{return alloc == max_;}
    bool need_exceeded(const ext_vector<int>& req) const{return req > need;}

    void alloc_req(const ext_vector<int>&req){alloc += req; need-= req;}
    void dealloc_req(const ext_vector<int>&req){alloc -= req; need+= req;}

    const ext_vector<int> create_req(){
      ext_vector<int> req;
      while(req.is_zero()){
        req.clear();
        for(int i = 0; i < alloc.size(); ++i){
          int needs = need[i];
          double gener = dis(gen);
          int val = (int)(needs * gener + 1);
          val = std::min(needs,val);
          req.push_back(val);
        }
        //if(req.is_zero()){std::cout << "..... req is zero: " << req <<"\n";}
      }
      return ext_vector<int>(req);
    }
    void release_all_resources(){
      ext_vector<int> zero(alloc.size(),0);
      alloc = max_ = need = zero;
      pthread_mutex_lock(&mutex_);
      std::cout << "Customer p#" << idx <<" has released all resources and is shutting down\n";
      pthread_mutex_unlock(&mutex_);
    }
    void show()const {
      std::cout<< "P#" << idx << " " << alloc << " " << max_ << " " << need << "\n";
    }

    friend std:: ostream& operator <<(std::ostream& os, const Customer& c){
      c.show();
      return os;
    }

  };
//release rescources
#endif

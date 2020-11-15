#ifndef bank_h
#define bank_h
#include <queue>
#include "ext_vector.h"
#include "customer.h"

class Bank {
private:
  ext_vector<int> avail;
  ext_vector<Customer*> customers;
public:
  Bank() = default;
  Bank(const ext_vector<int>& available):avail(available), customers(){ }

  ext_vector<int> get_avail() const{return avail;}

  bool is_avail(const ext_vector<int>& req) const { return req < avail; }

  bool req_approved(int idx, const ext_vector<int>&req) const { return true; }


  void add_customer(Customer* c){customers.push_back(c);}

  void withdraw_resources(const ext_vector<int>& req) {
    if (!is_avail(req)) {
      pthread_mutex_lock(&mutex_);
      std::cerr << "WARNING: req: " << req << " is not available for withdrawing\n";
      pthread_mutex_unlock(&mutex_);
      return;
    }
    if (is_avail(req)) { avail -= req; }
  }

  void deposit_resources(const ext_vector<int>& req) { avail += req; }

  ext_vector<Customer*> get_customers() const{return customers;}

  void show() const{
    pthread_mutex_lock(&mutex_);
    std::cout << "avail: [" << avail << "]\n";
    pthread_mutex_unlock(&mutex_);
    for(Customer* c: customers){
      c->show();
    }
    std::cout << "\n";
  }

  friend std::ostream& operator <<(std::ostream& os, const Bank& be){
    be.show();
    return os;
  }

  bool is_safe(int idx, const ext_vector<int>&req){
    if(is_avail(req)){return false;}
    if(idx > customers.size()){std::cerr <<"WARNING: idx out of range\n"; return false;}
    Customer* cust = customers[idx];
    if(cust->need_exceeded(req)){ return false;}

    ext_vector<int> save_avail = avail;
    std::queue<int> qsafe;
    std::queue<Customer*> qcustomers;

    for(Customer* c : customers){
      if((cust != c) && !c->needs_met()){
        qcustomers.push(c);
      }
    }
    withdraw_resources(req);
    ext_vector<int> max_req;
    bool one_allocated = true;

    while(!qcustomers.empty() && one_allocated){
      one_allocated = false;
      Customer* c = qcustomers.front();
      qcustomers.pop();

      max_req = c->get_max();
      if(is_avail(max_req)){
        deposit_resources(max_req);
        qsafe.push(c->get_id());
        one_allocated = true;
      }
      else{qcustomers.push(c);}
    }
    avail = save_avail;
    return qcustomers.empty();
  }


};

#endif

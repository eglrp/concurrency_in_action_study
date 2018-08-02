#ifndef ATM
#define ATM
#include "atm_msg.hpp"
#include "receiver.hpp"
#include "sender.hpp"
#include <iostream>

class Atm {
    messaging::Receiver incoming;
    messaging::Sender bank_;
    messaging::Sender interface_hardware_;
    void (Atm::*state)();
    std::string account_;
    unsigned withdrawal_amount_;
    std::string pin_;

    void process_withdrawl();
    void process_balance();
    void wait_for_action();
    void verifying_pin();
    void getting_pin();
    void waiting_for_card();
    void done_processing();
    Atm(Atm const &) = delete;
    Atm &operator=(Atm const &) = delete;

  public:
    Atm(messaging::Sender bank, messaging::Sender interface_hardware)
        : bank_(bank), interface_hardware_(interface_hardware) {}
    void done();
    //   // 通过指向该类成员的指针的变化,利用for循环,在各个状态之间转化
    //   // 每个状态中都有调用到wait,会自动等待其他队列消息(阻塞),并提供处理方式
    void run();
    messaging::Sender get_sender();
};
#endif /* ifndef ATM */

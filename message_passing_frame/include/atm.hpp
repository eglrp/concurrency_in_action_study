#ifndef ATM
#define ATM
#include <iostream>
#include "atm_msg.hpp"
#include "receiver.hpp"
#include "sender.hpp"

class Atm {
    messaging::Receiver incoming;
    messaging::Sender bank_;
    messaging::Sender interface_hardware_;
	void (Atm::*state)();
    std::string account_;
    unsigned withdrawal_amount_;
    std::string pin_;

    void process_withdrawl() {
        incoming.wait()
            .handle<withdraw_ok>(
                [&](withdraw_ok const &msg) {
                    interface_hardware_.send(issue_money(withdrawal_amount_));
                    bank_.send(withdrawal_processed(account_, withdrawal_amount_));
                    state = &Atm::done_processing;
                })
            .handle<withdraw_denied>(
                [&](withdraw_denied const &msg) {
                    interface_hardware_.send(display_insufficient_funds());
                    state = &Atm::done_processing;
                })
            .handle<cancel_pressed>(
                [&](cancel_pressed const &msg) {
                    bank_.send(cancel_withdrawal(account_, withdrawal_amount_));
                    interface_hardware_.send(display_withdraw_canclled());
                });
    }

    void process_balance() {
        incoming.wait()
            .handle<balance>(
                [&](balance const &msg) {
                    interface_hardware_.send(display_balance(msg.amount_));
                    state = &Atm::wait_for_action;
                })
            .handle<cancel_pressed>(
                [&](cancel_pressed const &msg) {
                    state = &Atm::done_processing;
                });
    }

    void wait_for_action() {
        interface_hardware_.send(display_withdrawal_options());
        incoming.wait().handle<withdraw_pressed>(
                           [&](withdraw_pressed const &msg) {
                               withdrawal_amount_ = msg.amount_;
                               bank_.send(withdraw(account_, msg.amount_, incoming));
                               state = &Atm::process_withdrawl;
                           })
            .handle<balance_pressed>(
                [&](balance_pressed const &msg) {
                    bank_.send(get_balance(account_, incoming));
                    state = &Atm::process_balance;
                })
            .handle<cancel_pressed>(
                [&](cancel_pressed const &msg) {
                    state = &Atm::done_processing;
                });
    }

    void verifying_pin() {
        incoming.wait()
            .handle<pin_verified>(
                [&](pin_verified const &msg) {
                    state = &Atm::wait_for_action;
                })
            .handle<pin_incorrect>(
                [&](pin_incorrect const &msg) {
                    interface_hardware_.send(display_pin_incorrect_message());
                    state = &Atm::done_processing;
                })
            .handle<cancel_pressed>(
                [&](cancel_pressed const &msg) {
                    state = &Atm::done_processing;
                });
    }

    void getting_pin() {
        incoming.wait()
            .handle<digit_pressed>(
                [&](digit_pressed const &msg) {
                    unsigned const pin_length = 4;
                    pin_ += msg.digit_;
                    if(pin_.length() == pin_length) {
                        bank_.send(verify_pin(account_, pin_, incoming));
                        state = &Atm::verifying_pin;
                    }
                })
            .handle<cancel_pressed>(
                [&](cancel_pressed const &msg) {
                    state = &Atm::done_processing;
                });
    }

    void waiting_for_card() {
        interface_hardware_.send(display_enter_card());
        incoming.wait()
            .handle<card_inserted>(
                [&](card_inserted const &msg) {
                    account_ = msg.account_;
                    pin_ = "";
                    interface_hardware_.send(display_enter_pin());
                    state = &Atm::getting_pin;
                });
    }

    void done_processing() {
        interface_hardware_.send(eject_card());
        state = &Atm::waiting_for_card;
    }

    Atm(Atm const &) = delete;
    Atm &operator=(Atm const &) = delete;

  public:
    Atm(messaging::Sender bank, messaging::Sender interface_hardware)
        : bank_(bank), interface_hardware_(interface_hardware) {}

    void done() {
        get_sender().send(messaging::CloseQueue());
    }

	// 通过指向该类成员的指针的变化,利用for循环,在各个状态之间转化
	// 每个状态中都有调用到wait,会自动等待其他队列消息(阻塞),并提供处理方式
    void run() {
        state = &Atm::waiting_for_card;
        try {
            for(;;) {
                (this->*state)();
            }
        } catch(messaging::CloseQueue const &) {
        }
    }

    messaging::Sender get_sender() {
		// Reveiver的operator Sender()实现了Received向Send的转化
		// 搭配Sender()的构造函数,实现返回的Sender对象中Queue* queue_ 指向该类的Queue对象
        return incoming;
    }
};
#endif /* ifndef ATM */

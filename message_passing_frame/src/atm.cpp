#include "../include/atm.hpp"

void Atm::process_withdrawl() {
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

void Atm::process_balance() {
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

void Atm::wait_for_action() {
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

void Atm::verifying_pin() {
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

void Atm::getting_pin() {
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

void Atm::waiting_for_card() {
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

void Atm::done_processing() {
    interface_hardware_.send(eject_card());
    state = &Atm::waiting_for_card;
}

void Atm::done() {
    get_sender().send(messaging::CloseQueue());
}

void Atm::run() {
    state = &Atm::waiting_for_card;
    try {
        for(;;) {
            (this->*state)();
        }
    } catch(messaging::CloseQueue const &) {
    }
}

messaging::Sender Atm::get_sender() {
    //       // Reveiver的operator Sender()实现了Received向Send的转化
    //       // 搭配Sender()的构造函数,实现返回的Sender对象中Queue* queue_ 指向该类的Queue对象
    return incoming;
}

#include "../include/interface_machine.hpp"

void InterfaceMachine::done() {
    get_sender().send(messaging::CloseQueue());
}

messaging::Sender InterfaceMachine::get_sender() {
    return incoming_;
}

void InterfaceMachine::run() {
    try {
        for(;;) {
            incoming_.wait()
                .handle<issue_money>(
                    [&](issue_money const &msg) {
                        std::lock_guard<std::mutex> lk(m_);
                        std::cout << "Iusuing " << msg.amount_ << std::endl;
                    })
                .handle<display_insufficient_funds>(
                    [&](display_insufficient_funds const &msg) {
                        std::lock_guard<std::mutex> lk(m_);
                        std::cout << "Insufficient funds" << std::endl;
                    })
                .handle<display_enter_pin>(
                    [&](display_enter_pin const &msg) {
                        std::lock_guard<std::mutex> lk(m_);
                        std::cout << "Please enter you PIN (0-9)" << std::endl;
                    })
                .handle<display_enter_card>(
                    [&](display_enter_card const &msg) {
                        std::lock_guard<std::mutex> lk(m_);
                        std::cout << "Please enter you card(T)" << std::endl;
                    })
                .handle<display_balance>(
                    [&](display_balance const &msg) {
                        std::lock_guard<std::mutex> lk(m_);
                        std::cout << "The balance of your account is " << msg.amount_ << std::endl;

                    })
                .handle<display_withdrawal_options>(
                    [&](display_withdrawal_options const &msg) {
                        std::lock_guard<std::mutex> lk(m_);
                        std::cout << "Withdraw 50?(w)" << std::endl;
                        std::cout << "Cancel?(c)" << std::endl;
                    })
                .handle<display_withdraw_canclled>(
                    [&](display_withdraw_canclled const &msg) {
                        std::lock_guard<std::mutex> lk(m_);
                        std::cout << "Withdraw cancelled" << std::endl;
                    })
                .handle<eject_card>(
                    [&](eject_card const &msg) {
                        std::lock_guard<std::mutex> lk(m_);
                        std::cout << "Eject card" << std::endl;

                    })
                .handle<display_pin_incorrect_message>(
                    [&](display_pin_incorrect_message const &msg) {
                        std::lock_guard<std::mutex> lk(m_);
                        std::cout << "PIN incorrect" << std::endl;
                    });
        }
    } catch(messaging::CloseQueue &) {
    }
}

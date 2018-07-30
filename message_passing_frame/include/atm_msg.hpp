#ifndef ATM_MSG
#define ATM_MSG 
#include "message.hpp"
#include "sender.hpp"

// 定义消息类型及结构, 消息通过队列在线程传递


struct withdraw {
    std::string account_;
    unsigned amount_;
    mutable messaging::Sender atm_queue_;
    withdraw(std::string const &account, unsigned amount, messaging::Sender atm_queue)
        : account_(account), amount_(amount), atm_queue_(atm_queue) {}
};

struct withdraw_ok {};

struct withdraw_denied {};

struct cancel_withdrawal {
    std::string account_;
    unsigned amount_;
    cancel_withdrawal(std::string const &account, unsigned amount)
        : account_(account), amount_(amount) {}
};

struct withdrawal_processed {
    std::string account_;
    unsigned amount_;
    withdrawal_processed(std::string const &account, unsigned amount)
        : account_(account), amount_(amount) {}
};

struct card_inserted {
    std::string account_;
    explicit card_inserted(std::string const &account)
        : account_(account) {}
};

struct digit_pressed {
    char digit_;
    explicit digit_pressed(char digit)
        : digit_(digit) {}
};

struct clear_last_pressed {};

struct eject_card {};

struct withdraw_pressed {
    unsigned amount_;
    explicit withdraw_pressed(unsigned amount)
        : amount_(amount) {}
};

struct cancel_pressed {};

struct issue_money {
    unsigned amount_;
    issue_money(unsigned amount)
        : amount_(amount) {}
};

struct verify_pin {
    std::string account_;
    std::string pin_;
    mutable messaging::Sender atm_queue_;
    verify_pin(std::string const &account, std::string const &pin, messaging::Sender atm_queue)
        : account_(account), pin_(pin), atm_queue_(atm_queue) {}
};

struct pin_verified {};

struct pin_incorrect {};

struct display_enter_pin {};

struct display_enter_card {};

struct display_insufficient_funds {};

struct display_withdraw_canclled {};

struct display_pin_incorrect_message {};

struct display_withdrawal_options {};

struct get_balance {
    std::string account_;
    mutable messaging::Sender atm_queue_;
    get_balance(std::string const &account, messaging::Sender atm_queue)
        : account_(account), atm_queue_(atm_queue) {}
};

struct balance {
    unsigned amount_;
    explicit balance(unsigned amount)
        : amount_(amount) {}
};

struct display_balance {
    unsigned amount_;
    explicit display_balance(unsigned amount)
        : amount_(amount) {}
};

struct balance_pressed {};
#endif /* ifndef ATM_MSG */

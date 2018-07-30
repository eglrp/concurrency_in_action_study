#include "receiver.cpp"
// #include "sen"
#include "atm_msg.cpp"
class BankMachine {
    messaging::Receiver incoming_;
    unsigned balance_;

  public:
    BankMachine()
        : balance_(199) {}

    void done() {
        get_sender().send(messaging::CloseQueue());
    }

    void run() {
        try {
            for(;;)
                incoming_.wait()
                    .handle<verify_pin>(
                        [&](verify_pin const &msg) {
                            if(msg.pin_ == "1234") {
                                msg.atm_queue_.send(pin_verified());
                            } else {
                                msg.atm_queue_.send(pin_incorrect());
                            }
                        })
                    .handle<withdraw>(
                        [&](withdraw const &msg) {
                            if(balance_ >= msg.amount_) {
                                msg.atm_queue_.send(withdraw_ok());
                                balance_ -= msg.amount_;
                            } else {
                                msg.atm_queue_.send(withdraw_denied());
                            }
                        })
                    .handle<get_balance>(
                        [&](get_balance const &msg) {
                            msg.atm_queue_.send(::balance(balance_));
                        })
                    .handle<withdrawal_processed>(
                        [&](withdrawal_processed const &msg) {})
                    .handle<cancel_withdrawal>(
                        [&](cancel_withdrawal const &msg) {});
        }catch(messaging::CloseQueue const&){}
    }

	messaging::Sender get_sender(){
		return incoming_;
	}
};

#ifndef BANK_MACHINE
#define BANK_MACHINE
#include "receiver.hpp"
#include "atm_msg.hpp"
class BankMachine {
    messaging::Receiver incoming_;
    unsigned balance_;

  public:
    BankMachine();

    void done();

    void run();

	messaging::Sender get_sender();
};
#endif /* ifndef BANK_MACHINE */

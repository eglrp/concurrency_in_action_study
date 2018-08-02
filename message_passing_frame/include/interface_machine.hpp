#ifndef INTERFACE_MACHINE
#define INTERFACE_MACHINE
#include "atm_msg.hpp"
#include "receiver.hpp"
#include <iostream>
class InterfaceMachine {
    messaging::Receiver incoming_;
    std::mutex m_;

  public:
    void done();
    messaging::Sender get_sender();
    void run();
};
#endif /* ifndef INTERFACE_MACHINE */

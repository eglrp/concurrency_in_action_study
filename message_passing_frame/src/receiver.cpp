#include "../include/receiver.hpp"

messaging::Receiver::operator Sender() {
    // Sender(Queue* q)构造函数复制其指针
    return Sender(&queue_);
}

messaging::Dispatcher messaging::Receiver::wait() {
    return Dispatcher(&queue_);
}

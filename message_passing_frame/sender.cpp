#ifndef SENDER
#define SENDER 
#include "message.cpp"
namespace messaging {
class Sender {
    Queue *queue_;

  public:
    Sender() : queue_(nullptr) {}
	// 复制队列仅仅复制对指向队列的指针
	// ??是否可以采用智能指针
    explicit Sender(Queue *q) : queue_(q) {}

	// 发送消息,将消息push到发送队列中
    template <typename Msg>
    void send(Msg const &msg) {
        if(queue_) {
            queue_->push(msg);
        }
    }
};
} // namespace messaging
#endif /* ifndef SENDER */

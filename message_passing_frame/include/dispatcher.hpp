#ifndef DISPATCHER
#define DISPATCHER
#include "message.hpp"
#include "template_dispacher.hpp"
namespace messaging {
class CloseQueue {};
class Dispatcher {
    Queue *queue_;
    bool chained_;

    // 禁止复制和赋值
    Dispatcher(Dispatcher const &) = delete;
    Dispatcher &operator=(const Dispatcher &) = delete;

    template <
        typename T_Dispatcher,
        typename Msg,
        typename Func>
    friend class TemplateDispatcher;

    void wait_and_dispatch() {
        for(;;) {
            auto msg = queue_->wait_and_pop();
            dispatch(msg);
        }
    }

    bool dispatch(std::shared_ptr<MessageBase> const &msg) {
        //动态转化, 如果是WrappedMessage<CloseQueue>* 类型,则转化为该类型, 否则为空指针
        if(dynamic_cast<WrappedMessage<CloseQueue> *>(msg.get())) {
            throw CloseQueue();
        }
        return false;
    }

  public:
    Dispatcher(Dispatcher &&other) : queue_(other.queue_), chained_(other.chained_) {
        //?? what is chained_?
        other.chained_ = true;
    }

    explicit Dispatcher(Queue *q) : queue_(q), chained_(false) {}

    template <typename Msg, typename Func>
    TemplateDispatcher<Dispatcher, Msg, Func> handle(Func &&f) {
        return TemplateDispatcher<Dispatcher, Msg, Func>(queue_, this, std::forward<Func>(f));
    }

    ~Dispatcher() noexcept(false) {
        if(!chained_) {
            wait_and_dispatch();
        }
    }
};
} // namespace messaging
#endif /* ifndef DISPATCHER */

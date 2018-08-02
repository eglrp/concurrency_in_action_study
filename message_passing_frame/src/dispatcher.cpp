#include "../include/dispatcher.hpp"

messaging::Dispatcher::Dispatcher(Dispatcher &&other) {
    other.chained_ = true;
}

messaging::Dispatcher::Dispatcher(Queue *q)
    : queue_(q), chained_(false) {}

// template <typename Msg, typename Func>
// messaging::TemplateDispatcher<messaging::Dispatcher, Msg, Func> messaging::Dispatcher::handle(Func &&f) {
//     return TemplateDispatcher<messaging::Dispatcher, Msg, Func>(queue_, this, std::forward<Func>(f));
// }

messaging::Dispatcher::~Dispatcher() {
}


void messaging::Dispatcher::wait_and_dispatch() {
    for(;;) {
        auto msg = queue_->wait_and_pop();
        dispatch(msg);
    }
}

bool messaging::Dispatcher::dispatch(std::shared_ptr<MessageBase> const &msg) {
    //     //动态转化, 如果是WrappedMessage<CloseQueue>* 类型,则转化为该类型, 否则为空指针
    if(dynamic_cast<WrappedMessage<CloseQueue> *>(msg.get())) {
        throw CloseQueue();
    }
    return false;
}

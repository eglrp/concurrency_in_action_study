#include "message.cpp"
namespace messaging {
template <typename PreviousDispatcher, typename Msg, typename Func>
class TemplateDispatcher {
    Queue *queue_;
    PreviousDispatcher *prev_;
    Func f_;
    bool chained_;

	// 禁止赋值
    TemplateDispatcher(TemplateDispatcher const &) = delete;
    TemplateDispatcher &operator=(TemplateDispatcher const &) = delete;

	//友元类(自己):是该类的不同实例可以访问其private和protected成员
    template <typename Dispatcher, typename OtherMsg, typename OtherFunc>
    friend class TemplateDispatcher;

    //等待消息队列并弹出第一个元素,并进行消息的处理
    void wait_and_dispatch() {
        for(;;) {
            auto msg = queue_->wait_and_pop();
            //如果处理成功,则退出并析构该单向消息处理链表的所有节点
            //ps: 在生成单向消息处理链表处有while循环,可再次生成新的单向消息处理链表
            if(dispatch(msg))
                break;
        }
    }

    //检测信息类型和当前实例的信息类型是否一样,一样则处理,否则交由prev_指针的对象处理.
    //prev_最终指向Dispatcher对象,Dispatcher::dispatch的处理方式不一样.
    bool dispatch(std::shared_ptr<MessageBase> const &msg) {
        if(WrappedMessage<Msg> *wrapper = dynamic_cast<WrappedMessage<Msg> *>(msg.get())) {
            f_(wrapper->content_);
            return true;
        } else {
            return prev_->dispatch(msg);
        }
    }

  public:
    TemplateDispatcher(TemplateDispatcher &&other)
        : queue_(other.queue_), prev_(other.prev_), f_(std::move(other.f_)), chained_(other.chained_) {
        other.chained_ = true;
    }

    TemplateDispatcher(Queue *q, PreviousDispatcher *prev, Func &&f)
        : queue_(q), prev_(prev), f_(std::forward<Func>(f)), chained_(false) {
        // 将prev_指向的前一个TemplateDispatcher对象的chained设置为true,配合析构函数,避免重复处理队列的消息
        prev_->chained_ = true;
    }

    //std::forward将rvalue转为lvalue.
    //该函数和上面的构造函数是实现单向消息处理链表的关键
    template <typename OtherMsg, typename OtherFunc>
    TemplateDispatcher<TemplateDispatcher, OtherMsg, OtherFunc> handle(OtherFunc &&other_func) {
        return TemplateDispatcher<TemplateDispatcher, OtherMsg, OtherFunc>(
            queue_, this, std::forward<OtherFunc>(other_func));
    }

    //该析构函数是实现自动等待队列请求并处理的方式
    ~TemplateDispatcher() noexcept(false) {
        if(!chained_)
            wait_and_dispatch();
    }
};
} // namespace messaging

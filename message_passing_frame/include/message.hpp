#ifndef MESSAGE
#define MESSAGE
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>

namespace messaging {

//消息基类
struct MessageBase {
    virtual ~MessageBase() {}
};

//消息子类
template <typename Msg>
struct WrappedMessage : MessageBase {
    Msg content_;
    explicit WrappedMessage(Msg const &content) : content_(content) {}
};

//线程安全队列
class Queue {
    std::mutex m_;
    std::condition_variable cond_var_;
    std::queue<std::shared_ptr<MessageBase>> queue_;

  public:
    template <typename T>
    void push(T const &msg) {
        std::lock_guard<std::mutex> lk(m_);
		//向队列插入子类
        queue_.push(std::make_shared<WrappedMessage<T>>(msg));
        cond_var_.notify_all();
    }

	// 等待并获取队列最前的数据
    std::shared_ptr<MessageBase> wait_and_pop() {
        std::unique_lock<std::mutex> lk(m_);
		//wait直到condition_variable::notify_one或condition_variable::notify_all被调用
        cond_var_.wait(lk, [&] { return !queue_.empty(); });
        auto res = queue_.front();
        queue_.pop();
        return res;
    }
};

} // namespace messaging
#endif /* ifndef MESSAGE */

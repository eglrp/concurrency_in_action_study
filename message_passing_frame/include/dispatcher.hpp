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

    template <typename T_Dispatcher, typename Msg, typename Func>
    friend class TemplateDispatcher;

    void wait_and_dispatch();

    bool dispatch(std::shared_ptr<MessageBase> const &msg);


  public:
    Dispatcher(Dispatcher &&other);

    explicit Dispatcher(Queue *q);

	template <typename Msg, typename Func>
	TemplateDispatcher<Dispatcher, Msg, Func> handle(Func &&f) {
		return TemplateDispatcher<Dispatcher, Msg, Func>(queue_, this, std::forward<Func>(f));
	}

    ~Dispatcher();
};
} // namespace messaging
#endif /* ifndef DISPATCHER */

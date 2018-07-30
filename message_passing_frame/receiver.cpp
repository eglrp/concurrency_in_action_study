#ifndef RECEIVER
#define RECEIVER 
#include "dispatcher.cpp"
#include "message.cpp"
#include "sender.cpp"
namespace messaging {
class Receiver {
    // 非指针
    Queue queue_;

  public:
	//Conversion operator overload: 转换重载
	// example
	// 	Receiver x;
	// 	Sender y = x; //自动转化为Sender类型,不会报错
    operator Sender() {
		// Sender(Queue* q)构造函数复制其指针
        return Sender(&queue_);
    }
    Dispatcher wait() {
        return Dispatcher(&queue_);
    }
};
} // namespace messaging
#endif /* ifndef RECEIVER */

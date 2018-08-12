// copy from <<C++并发编程>> P43
#include <exception>
#include <iostream>
#include <memory>
#include <mutex>
#include <stack>
using namespace std;

//书里自定义异常的写法在clang不能编译,需要增加return
struct empty_stack : exception {
    const char *what() const throw() {
        return "the stack you access is empty";
    }
};

template <typename T>
class ThreadSafeStack {
  private:
    stack<T> data_;
    mutable mutex m_;

  public:
    ThreadSafeStack() {}
    ThreadSafeStack(const ThreadSafeStack &other) {
        lock_guard<mutex> lock(other.m_);
        data_ = other.data_;
    }

    ThreadSafeStack &operator=(const ThreadSafeStack) = delete;

    void push(T new_value) {
        lock_guard<mutex> lock(m_);
        data_.push(new_value);
    }

	// 对pop() 实现两种方法
	// 返回指针
    shared_ptr<T> pop() {
        lock_guard<mutex> lock(m_);
        if(data_.empty())
            throw empty_stack();
        shared_ptr<T> const res(make_shared<T>(data_.top()));
        data_.pop();
        return res;
    }

	// 参数引用
    void pop(T &value) {
        lock_guard<mutex> lock(m_);
        if(data_.empty())
            throw empty_stack();
        value = data_.top();
        data_.pop();
    }

    bool empty() const {
        lock_guard<mutex> lock(m_);
        return data_.empty();
    }
};

int main() {
    ThreadSafeStack<int> s;
    s.push(1);
    s.push(2);
    s.push(3);
    std::cout << *s.pop() << std::endl;
    int x;
    s.pop(x);
    std::cout << x << std::endl;
}

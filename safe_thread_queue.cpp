// copy from <<C++并发编程>> P69
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>

using namespace std;

template <typename T>
class ThreadSafeQueue {
  private:
    mutable mutex m_;
    queue<T> data_queue_;
    condition_variable data_cond_;

  public:
    ThreadSafeQueue() {}
    //? const&
    ThreadSafeQueue(ThreadSafeQueue const &other) {
        lock_guard<mutex> lock(other.m_);
        data_queue_ = other.data_queue_;
    }
    void push(T new_value) {
        lock_guard<mutex> lock(m_);
        data_queue_.push(new_value);
        data_cond_.notify_one();
    }

    void wait_and_pop(T &value) {
        unique_lock<mutex> u_lock(m_);
        data_cond_.wait(u_lock, [this] { return !data_queue_.empty(); });
		value = data_queue_.front();
		data_queue_.pop();
    }

	shared_ptr<T> wait_and_pop(){
        unique_lock<mutex> u_lock(m_);
        data_cond_.wait(u_lock, [this] { return !data_queue_.empty(); });
		shared_ptr<T> res{make_shared<T>(data_queue_.front())};
		data_queue_.pop();
		return res;
	}

	bool try_pop(T& value){
		lock_guard<mutex> lock(m_);
		if(data_queue_.empty())
			return false;
		value = data_queue_.front();
		data_queue_.pop();
		return true;
	}

	shared_ptr<T> try_pop(){
		lock_guard<mutex> lock(m_);
		if(data_queue_.empty())
			return shared_ptr<T>();
		shared_ptr<T> res(make_shared<T>(data_queue_.front()));
		data_queue_.pop();
		return res;
	}

	bool empty() const{
		lock_guard<mutex> lock(m_);
		return data_queue_.empty();
	}
};

int
main(int argc, char *argv[]) {
	ThreadSafeQueue<int> queue;
	queue.push(1);
	queue.push(2);
	queue.push(3);
	std::cout << *queue.try_pop() << std::endl;

}

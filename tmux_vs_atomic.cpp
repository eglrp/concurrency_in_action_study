// 参考博客: https://blog.csdn.net/hspingcc/article/details/54617531
// 线程冲突的原因: https://blog.masterliu.net/cpp11-atomic/
#include <atomic>
#include <iostream>
#include <mutex>
#include <thread>

#define TEST_DATA_LENGTH 100000 //每个线程操作次数
#define THREAD_NUM 10           //线程个数

using namespace std; //引入std命名空间

mutex m; //声明互斥锁m

long n_total = 0;
long m_total = 0;
atomic_long a_total; //原子量的初始化

std::chrono::duration<double> measure_time(void (*f)(void)) {
    auto start = std::chrono::high_resolution_clock::now();
    f();
    // Record end time
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    return elapsed;
    // 在不采用互斥锁和原子类的情况下
}

//在不采用互斥锁和原子类的情况下
void test_f_normal() {
    for(int i = 0; i < TEST_DATA_LENGTH; i++) {
        n_total += 1;
    }
}

//使用mutex互斥锁
void test_f_mutex() {
    for(int i = 0; i < TEST_DATA_LENGTH; i++) {
        m.lock();
        m_total += 1;
        m.unlock();
    }
}
//使用原子操作
void test_f_atomic() {
    for(int i = 0; i < TEST_DATA_LENGTH; i++) {
        a_total += 1;
    }
}

int main() {
    thread ts[THREAD_NUM];
    //normal mode ,result is error
    auto start = std::chrono::high_resolution_clock::now();

    for(int i = 0; i < THREAD_NUM; i++) {
        ts[i] = thread(&test_f_normal);
    }

    for(int i = 0; i < THREAD_NUM; i++) {
        ts[i].join();
    }
    // Record end time
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;

    cout << "total: " << n_total << endl;
    cout << "test_f_normal: " << elapsed.count() << endl;

    //use mutex ,
    auto m_start = std::chrono::high_resolution_clock::now();

    for(int i = 0; i < THREAD_NUM; i++) {
        ts[i] = thread(&test_f_mutex);
    }

    for(int i = 0; i < THREAD_NUM; i++) {
        ts[i].join();
    }

    auto m_finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> m_elapsed = m_finish - m_start;

    cout << "total: " << m_total << endl;
    cout << "test_f_mutex: " << m_elapsed.count() << endl;

    //use atomic
    auto a_start = std::chrono::high_resolution_clock::now();

    for(int i = 0; i < THREAD_NUM; i++) {
        ts[i] = thread(&test_f_atomic);
    }

    for(int i = 0; i < THREAD_NUM; i++) {
        ts[i].join();
    }

    auto a_finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> a_elapsed = a_finish - a_start;

    cout << "total: " << a_total << endl;
    cout << "test_f_atomic: " << a_elapsed.count() << endl;
}

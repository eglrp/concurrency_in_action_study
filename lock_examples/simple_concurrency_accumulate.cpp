// copy from <<C++并发编程实战>>P26
#include <algorithm>
#include <iostream>
#include <thread>
// using namespace std;

// 重载(),计算迭代器范围的总和并通过引用参数的方式传递
template <typename Iterator, typename T>
struct accumulate_block {
    void operator()(Iterator first, Iterator last, T &result) {
        result = std::accumulate(first, last, result);
    }
};

template <typename Iterator, typename T>
T parallel_accumulate(Iterator first, Iterator last, T init) {
	// 计算合适的线程数量,
    unsigned long const length = std::distance(first, last);
    if(!length)
        return init;

	// 书里的min_per_threads是25,但是如果数据量小于等于25时,会导致max_threads为1,num_threads为1
	// std::vector<std::thread> threads(num_threads -1) 出现错误
    unsigned long const min_per_threads = 2;
    unsigned long const max_threads = (length + min_per_threads - 1) / min_per_threads;
    unsigned long const hardware_threads = std::thread::hardware_concurrency();
    unsigned long const num_threads = std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);

	// 计算每个线程需要计算多少个数据相加,剩下的余数给主线程计算
    unsigned long const block_size = length / num_threads;

    std::vector<T> results(num_threads);
    std::vector<std::thread> threads(num_threads - 1);
    Iterator block_start = first;
	//把线程存在vector,线程运行结果存在vector results中
    for(unsigned long i = 0; i < (num_threads - 1); i++) {
        Iterator block_end = block_start;
        std::advance(block_end, block_size);
        threads[i] = std::thread(
            accumulate_block<Iterator, T>(),
            block_start,
            block_end,
            std::ref(results[i]));
            block_start = block_end;
    }
	// 计算最后那些没写线程计算的余数
    accumulate_block<Iterator, T>()(block_start, last, results[num_threads - 1]);
	
	// 对每个线程调用std::mem_fn(&std::thread::join)方法,确保所有线程的计算都完成
    std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));

	// 返回每个线程计算结果的总和
    return std::accumulate(results.begin(), results.end(), init);
}
int main(int argc, char *argv[]) {
	int arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
	std::cout << parallel_accumulate<decltype(std::begin(arr)), int>(std::begin(arr), std::end(arr), 0) << std::endl;
}

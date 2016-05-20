#pragma once

#include <mutex>
#include <deque>
#include <condition_variable>
template <class T>
struct LockedQueue {
	// キューにxを加える
	void enqueue(const T& x) {
		std::unique_lock<std::mutex> lock(m);
		// 外からnotify_all()またはnotify_one()によって起こされるまでブロックして待つ
		// ただし、起こされた時にキューが満杯だった場合は、またブロックして待つ
		c_enq.wait(lock, [this] { return mode==0; });
		mode = 1;
		data.push_back(x);
		// dequeueの準備ができたことを通知
		mode = 0;
		c_deq.notify_one();
	}

	// キューから要素を取り出す
	T dequeue() {
		std::unique_lock<std::mutex> lock(m);
		// 外からnotify_all()またはnotify_one()によって起こされるまでブロックして待つ
		// ただし、起こされた時にキューが空である場合は、またブロックして待つ
		c_deq.wait(lock, [this] { return mode == 0; });
		mode = 1;
		T ret = data.front();
		data.pop_front();
		// enqueueの準備ができたことを通知
		mode = 0;
		c_enq.notify_one();
		return ret;
	}

	int size(){
		std::unique_lock<std::mutex> lock(m);
		c_enq.wait(lock, [this] { return mode==0; });
		c_deq.wait(lock, [this] { return mode==0; });
		mode = 1;
		auto s = data.size();

		mode = 0;
		c_deq.notify_one();
		c_enq.notify_one();

		return (int)s;
	}

private:
	std::mutex m;
	std::deque<T> data;
	std::condition_variable c_enq;
	std::condition_variable c_deq;
	int mode = 0;
};


class DrawThreadQueue{
public:
	static void enqueue(const std::function<void(void)>& func){
		mQueue.enqueue(func);
	}
	static std::function<void(void)> dequeue(){
		return mQueue.dequeue();
	}
	static int size(){
		return mQueue.size();
	}
private:
	static LockedQueue<std::function<void(void)>> mQueue;
};

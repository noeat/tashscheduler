# taskscheduler
timer
类似linux 时间轮实现的任务调度器， c++17以上
直接引用task_scheduler.h就行
example：
```c++
	task_scheduler<int> sche;
	sche.init(10);

	int i = 0;
	task<int, std::function<void()>> t([&i]() {
		std::cout << i++ << ", ";
	});

	sche.scheduler1(10, 100, &t);

	sche.update(1000);
```
加上时间管理可以做定时器， 不支持多线程。
```c++
	task_scheduler<uint64_t> sche;
	sche.init(10);

	int i = 0;
	task<uint64_t, std::function<void()>> t([&i]() {
		std::cout << i++ << ", ";
	});

	sche.scheduler1(20, 100, &t);

	while (true)
	{
		auto dur = std::chrono::steady_clock::now() - last;
		last = std::chrono::steady_clock::now();
		sche.update(std::chrono::duration_cast<std::chrono::milliseconds>(dur).count());
		std::this_thread::sleep_for(std::chrono::milliseconds(30));
	}
```
上面是一个任务调度器最小间隔时间是10ms， 添加了一个定时器每隔20ms触发一次，重复100次。
不需要手动cancel_shceduler除非要中断定时器。
定时器会在task析构的时候自动移除定时队列。


添加了创建手动删除的task 如：
```c++
void fxx(int ) {
	std::cout << "fxx" << std::endl;
}

struct test_task_create
{
	void test(int) {}
};
test_task_create obj;

#include "task_scheduler.h"
int main()
{
	task_scheduler<int> sche;
	sche.init(10);

	//步骤1 
	auto t2 = sche.create_task(fxx, 1);
	auto t3 = sche.create_task([](char c) {}, 'c');
	auto t4 = sche.create_member_task(&obj, &test_task_create::test, 1);
	sche.scheduler_task(100, 1, t2);
	sche.scheduler_task(100, 1, t3);
	sche.scheduler_task(100, 1, t4);

	//步骤2
	auto t5 = sche.scheduler(100, 1, fxx, 1);
	auto t6 = sche.scheduler(100, 1, [](char c) {}, 'c');
	auto t7 = sche.scheduler_member(100, 1, &obj, &test_task_create::test, 1);

	//步骤3
	task<int, void(*)(int), int> t8(&fxx, 1);
	task<int, std::function<void(char)>, char> t9([](char c) {}, 'c');
	task<int, void(test_task_create::*)(int), int> t10(&obj, &test_task_create::test, 1);
	sche.scheduler_task(100, 1, &t8);
	sche.scheduler_task(100, 1, &t9);
	sche.scheduler_task(100, 1, &t10);

	sche.update(1000);

	sche.destory_task(t2);
	sche.destory_task(t3);
	sche.destory_task(t4);
	return 0;
}
```

步骤1，2，3的运行效果是一样的，只不过步骤1 最后要手动清除task。

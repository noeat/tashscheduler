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

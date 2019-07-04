# tashscheduler
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

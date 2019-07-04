#ifndef __TASK_SCHEDULER_H__
#define __TASK_SCHEDULER_H__
#include <iterator>
#include <tuple>
#include <assert.h>
#include<functional>

class linked_list_head;
class linked_list_element
{
private:
	friend class linked_list_head;

	linked_list_element* next_;
	linked_list_element* prev_;

public:
	linked_list_element() : next_(nullptr), prev_(nullptr) { }

	bool has_next() const { return (next_ && next_->next_ != nullptr); }
	bool has_prev() const { return (prev_ && prev_->prev_ != nullptr); }
	bool is_int_list() const { return (next_ != nullptr && prev_ != nullptr); }

	linked_list_element      * next() { return has_next() ? next_ : nullptr; }
	linked_list_element const* next() const { return has_next() ? next_ : nullptr; }
	linked_list_element      * prev() { return has_prev() ? prev_ : nullptr; }
	linked_list_element const* prev() const { return has_prev() ? prev_ : nullptr; }

	linked_list_element      * nocheck_next() { return next_; }
	linked_list_element const* nocheck_next() const { return next_; }
	linked_list_element      * nocheck_prev() { return prev_; }
	linked_list_element const* nocheck_prev() const { return prev_; }

	void delink()
	{
		if (!is_int_list())
			return;

		next_->prev_ = prev_;
		prev_->next_ = next_;
		next_ = nullptr;
		prev_ = nullptr;
	}

	void insert_before(linked_list_element* pElem)
	{
		pElem->next_ = this;
		pElem->prev_ = prev_;
		prev_->next_ = pElem;
		prev_ = pElem;
	}

	void insert_after(linked_list_element* pElem)
	{
		pElem->prev_ = this;
		pElem->next_ = next_;
		next_->prev_ = pElem;
		next_ = pElem;
	}

private:
	linked_list_element(linked_list_element const&) = delete;
	linked_list_element& operator=(linked_list_element const&) = delete;

protected:
	~linked_list_element()
	{
		delink();
	}
};

//============================================

class linked_list_head
{
private:
	linked_list_element first_;
	linked_list_element last_;
	uint32_t size_;

public:
	linked_list_head() : size_(0)
	{
		// create empty list

		first_.next_ = &last_;
		last_.prev_ = &first_;
	}

	bool empty() const { return(!first_.next_->is_int_list()); }

	linked_list_element      * get_first() { return (empty() ? nullptr : first_.next_); }
	linked_list_element const* get_first() const { return (empty() ? nullptr : first_.next_); }

	linked_list_element      * get_last() { return(empty() ? nullptr : last_.prev_); }
	linked_list_element const* get_last() const { return(empty() ? nullptr : last_.prev_); }

	void insert_first(linked_list_element* pElem)
	{
		first_.insert_after(pElem);
	}

	void insert_last(linked_list_element* pElem)
	{
		last_.insert_before(pElem);
	}

	uint32_t get_size() const
	{
		if (!size_)
		{
			uint32_t result = 0;
			linked_list_element const* e = get_first();
			while (e)
			{
				++result;
				e = e->next();
			}
			return result;
		}
		else
			return size_;
	}

	void inc_size() { ++size_; }
	void dec_size() { --size_; }

	template<class _Ty>
	class linked_iterator
	{
	public:
		typedef std::bidirectional_iterator_tag     iterator_category;
		typedef _Ty                                 value_type;
		typedef ptrdiff_t                           difference_type;
		typedef ptrdiff_t                           distance_type;
		typedef _Ty*                                pointer;
		typedef _Ty const*                          const_pointer;
		typedef _Ty&                                reference;
		typedef _Ty const &                         const_reference;

		linked_iterator() : _Ptr(nullptr)
		{                                           // construct with null node pointer
		}

		linked_iterator(pointer _Pnode) : _Ptr(_Pnode)
		{                                           // construct with node pointer _Pnode
		}

		linked_iterator& operator=(linked_iterator const &_Right)
		{
			_Ptr = _Right._Ptr;
			return *this;
		}

		linked_iterator& operator=(const_pointer const &_Right)
		{
			_Ptr = pointer(_Right);
			return *this;
		}

		reference operator*()
		{                                           // return designated value
			return *_Ptr;
		}

		pointer operator->()
		{                                           // return pointer to class object
			return _Ptr;
		}

		linked_iterator& operator++()
		{                                           // preincrement
			_Ptr = _Ptr->next();
			return (*this);
		}

		linked_iterator operator++(int)
		{                                           // postincrement
			iterator _Tmp = *this;
			++*this;
			return (_Tmp);
		}

		linked_iterator& operator--()
		{                                           // predecrement
			_Ptr = _Ptr->prev();
			return (*this);
		}

		linked_iterator operator--(int)
		{                                           // postdecrement
			iterator _Tmp = *this;
			--*this;
			return (_Tmp);
		}

		bool operator==(linked_iterator const &_Right) const
		{                                           // test for iterator equality
			return (_Ptr == _Right._Ptr);
		}

		bool operator!=(linked_iterator const &_Right) const
		{                                           // test for iterator inequality
			return (!(*this == _Right));
		}

		bool operator==(pointer const &_Right) const
		{                                           // test for pointer equality
			return (_Ptr != _Right);
		}

		bool operator!=(pointer const &_Right) const
		{                                           // test for pointer equality
			return (!(*this == _Right));
		}

		bool operator==(const_reference _Right) const
		{                                           // test for reference equality
			return (_Ptr == &_Right);
		}

		bool operator!=(const_reference _Right) const
		{                                           // test for reference equality
			return (_Ptr != &_Right);
		}

		pointer _Mynode()
		{                                           // return node pointer
			return (_Ptr);
		}

	protected:
		pointer _Ptr;                               // pointer to node
	};

	typedef linked_iterator<linked_list_element> iterator;

private:
	linked_list_head(linked_list_head const&) = delete;
	linked_list_head& operator=(linked_list_head const&) = delete;

protected:
	~linked_list_head() { }
};

template<typename INT, int, int >
class task_scheduler;

template<typename INT>
class task_node : public linked_list_element
{
public:
	INT expiry_value_;
	INT schdeuler_dur_;
	int repeat_count_;
};

template<typename INT>
class task_base : private task_node<INT>
{
public:
	virtual ~task_base() = default;
	virtual void on_timer() = 0;
	virtual void on_destroy() {}

	template<typename, int, int >
	friend class task_scheduler;
};

template<typename INT, typename _Fty, typename... _Arg>
class task : public task_base<INT>
{
public:
	task(_Fty&& fty, _Arg&& ...args)
		:fty_(std::forward<_Fty>(fty)), params_(std::forward<_Arg>(args)...)
	{

	}

	void on_timer() override
	{
		apply(std::make_index_sequence<sizeof...(_Arg)>());
	}

protected:
	template<size_t ...I>
	void apply(std::index_sequence<I...>)
	{
		//static_assert(I == 1, "aaaa");
		//assert(fty_ != nullptr);
		fty_(std::get<I>(params_)...);
	}
private:
	_Fty fty_;
	std::tuple<_Arg...> params_;
};

template<typename INT, typename C, typename ..._Arg>
class task<INT, void (C::*)(_Arg&&...), _Arg...>
{
	using _Fty = void (C::*)(_Arg&&...);
public:
	task(C* obj, _Fty&& fty, _Arg&& ...args)
		:obj_(obj), fty_(std::forward<_Fty>(fty)),
		params_(std::forward<_Arg>(args)...)
	{
		assert(fty_ != nullptr);
	}

	void on_timer() override
	{
		apply(std::make_index_sequence<sizeof...(_Arg)>());
	}

protected:
	template<size_t... I>
	void apply(std::index_sequence<I...>)
	{
		assert(fty_ != nullptr);
		(obj_->*fty_)(std::get<I>(params_)...);
	}
private:
	C* obj_;
	_Fty fty_;
	std::tuple<_Arg...> params_;
};

template<typename INT, int TVR_BITS = 8, int TVN_BITS = 6>
class task_scheduler
{
private:
	static_assert(std::is_integral<INT>::value, "must interal");
	static_assert(sizeof(INT) * 8 >= TVR_BITS, "root_bits too big");
	typedef std::array<linked_list_head*, INT(1) << TVR_BITS> TVR_Vec;
	typedef std::array<linked_list_head*, INT(1) << TVN_BITS> TVN_Vec;
	static const int NODE_COUNT =
		((sizeof(INT) * 8 - TVR_BITS) - 1) / TVN_BITS + 1;
	static const INT TVR_SIZE = INT(1) << TVR_BITS;
	static const INT TVN_SIZE = INT(1) << TVN_BITS;
	static const INT TVR_MASK = TVR_SIZE - 1;
	static const INT TVN_MASK = TVN_SIZE - 1;

	template<typename _Fty, typename ..._Arg>
	class internal_task : public task<INT, _Fty, _Arg...>
	{
	public:
		using task<INT, _Fty, _Arg...>::task;

		virtual void on_destroy()
		{
			delete this;
		}
	};

	template<typename C, typename ..._Arg>
	class internal_task<void (C::*)(_Arg&&...), _Arg...>
		: public task<INT, void (C::*)(_Arg&&...), _Arg...>
	{
	public:
		using task<INT, void (C::*)(_Arg&&...), _Arg...>::task;

		virtual void on_destroy()
		{
			delete this;
		}
	};
public:
	using task_t = task_base<INT> *;
	task_scheduler() = default;
	~task_scheduler() = default;

	void init(INT interval);

	void update(INT elapse);

	task_t scheduler1(INT elapse, int repeat, task_t task);

	template<typename _Fty, typename ..._Arg>
	task_t scheduler(INT elapse, int repeat, _Fty&& fty, _Arg&& ...args);

	void cancel_shceduler(task_t task);
private:
	task_t internal_add_task(task_t task);
	void ontick();
	void cascade(TVN_Vec* tv, INT idx);
private:
	task_scheduler(const task_scheduler&) = delete;
	task_scheduler& operator=(const task_scheduler&) = delete;

private:
	INT interval_;
	INT jiffies_;
	INT next_expiry_;
	TVR_Vec root_tv_;
	std::array<TVN_Vec, NODE_COUNT> node_tvs_;
};

template<typename INT, int TVR_BITS, int TVN_BITS>
void task_scheduler<INT, TVR_BITS, TVN_BITS>::init(INT interval)
{
	assert(interval > 0);
	interval_ = interval;
	jiffies_ = INT();
	next_expiry_ = INT();
	for (auto& item : root_tv_)
	{
		item = new linked_list_head();
	}

	for (auto& node : node_tvs_)
	{
		for (auto& item : node)
		{
			item = new linked_list_head();
		}
	}
}

template<typename INT, int TVR_BITS, int TVN_BITS>
void task_scheduler<INT, TVR_BITS, TVN_BITS>::update(INT elapse)
{
	INT last = next_expiry_ + elapse;
	while (true)
	{
		INT tick_e = (jiffies_ + 1) * interval_;
		if (tick_e > last)
		{
			next_expiry_ = last;
			break;
		}

		
		ontick();
		next_expiry_ = (jiffies_ + 1) * interval_;
		++jiffies_;
	}
}

template<typename INT, int TVR_BITS, int TVN_BITS>
void task_scheduler<INT, TVR_BITS, TVN_BITS>::ontick()
{
	INT index = jiffies_ & TVR_MASK;
	if (index == INT() && jiffies_ != INT())
	{
		for (std::size_t i = 0; i < NODE_COUNT; ++i)
		{
			INT tmp = jiffies_ >> (TVR_BITS + i * TVN_BITS);
			if (tmp == INT())
				break;

			tmp = INT(1) << (TVR_BITS + i * TVN_BITS);
			cascade(&node_tvs_[i], (jiffies_ - tmp) & TVN_MASK);
		}
	}

	linked_list_head* list = root_tv_[(std::size_t)index];
	assert(list != nullptr);
	linked_list_head::iterator itbeg(list->get_first());
	linked_list_head::iterator itend;
	while (itbeg != itend)
	{
		task_t task = static_cast<task_t>(&*itbeg);
		assert(task != nullptr);
		task->on_timer();
		++itbeg;
		task->delink();

		if (task->repeat_count_ > 0)
		{
			task->repeat_count_--;
		}

		if (task->repeat_count_ == -1 || task->repeat_count_ > 0)
		{
			task->expiry_value_ = next_expiry_ + task->schdeuler_dur_;
			internal_add_task(task);
		}
		else
		{
			task->on_destroy();
		}
	}
}

template<typename INT, int TVR_BITS, int TVN_BITS>
void task_scheduler<INT, TVR_BITS, TVN_BITS>::cascade(TVN_Vec* tv, INT idx)
{
	assert(tv != nullptr);
	assert(idx < TVN_SIZE);
	linked_list_head* head = (*tv)[(std::size_t)idx];
	linked_list_head::iterator itbeg(head->get_first());
	linked_list_head::iterator itend;
	while (itbeg != itend)
	{
		task_t task = static_cast<task_t>(&*itbeg);
		assert(task != nullptr);
		++itbeg;
		task->delink();
		internal_add_task(task);
	}
}


template<typename INT, int TVR_BITS, int TVN_BITS>
typename task_scheduler<INT, TVR_BITS, TVN_BITS>::task_t
task_scheduler<INT, TVR_BITS, TVN_BITS>::scheduler1(INT elapse,
	int repeat, task_t task)
{
	//static_assert(false, "c 1");
	assert(task != nullptr);
	assert(elapse >= 0);
	task->expiry_value_ = next_expiry_ + elapse;
	task->repeat_count_ = repeat;
	task->schdeuler_dur_ = elapse;
	assert(task->expiry_value_ >= next_expiry_);
	return internal_add_task(task);
}

template<typename INT, int TVR_BITS, int TVN_BITS>
typename task_scheduler<INT, TVR_BITS, TVN_BITS>::task_t
task_scheduler<INT, TVR_BITS, TVN_BITS>::internal_add_task(task_t task)
{
	INT expires = task->expiry_value_ / interval_;
	INT idx = task->expiry_value_ / interval_ - jiffies_;
	linked_list_head* vec = NULL;
	if (idx < TVR_SIZE && idx > 0)
	{
		INT index = expires & TVR_MASK;
		vec = root_tv_[(std::size_t)index];
	}
	else if (idx < 0)
	{
		INT index = jiffies_ & TVR_MASK;
		vec = root_tv_[(std::size_t)index];
	}
	else
	{
		for (std::size_t i = 0; i < NODE_COUNT; ++i)
		{
			INT compare_val = INT(1) << (TVR_BITS + (i + 1) * TVN_BITS);
			if (idx < compare_val)
			{
				INT index = (idx & (~TVR_MASK)) -
					(INT(1) << (TVR_BITS + i * TVN_BITS)) & TVN_MASK;
				vec = node_tvs_[i][(std::size_t)index];
				break;
			}
		}
	}

	if (vec != nullptr)
	{
		vec->insert_last(task);
		return task;
	}

	return nullptr;
}

template<typename INT, int TVR_BITS, int TVN_BITS>
template<typename _Fty, typename ..._Arg>
typename task_scheduler<INT, TVR_BITS, TVN_BITS>::task_t
task_scheduler<INT, TVR_BITS, TVN_BITS>::scheduler(INT elapse, int repeat, _Fty&& fty, _Arg&& ...args)
{
	static_assert(false, "c2222");
	internal_task<_Fty, _Arg...>* task = new internal_task<_Fty, _Arg...>(
		std::forward<_Fty>(fty), std::forward<_Arg>(args)...);
	assert(task != NULL);
	return scheduler(elapse, repeat, task);
}

template<typename INT, int TVR_BITS, int TVN_BITS>
void task_scheduler<INT, TVR_BITS, TVN_BITS>::cancel_shceduler(task_t task)
{
	assert(task != nullptr);
	task->delink();
}
#endif //__TASK_SCHEDULER_H__
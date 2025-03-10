module;

//////////////////////////////////////////////////////////////////////
//
// sequence.coroutine_handle.ixx: coroutine handle
//
// PWH
//	2023-11-13. moved from sequence.h
//	2024-09-30 biscuit
//
//////////////////////////////////////////////////////////////////////

export module biscuit.sequence.coroutine_handle;
import std;
import biscuit.aliases;
import biscuit.concepts;

using namespace std::literals;
using namespace biscuit::literals;

namespace concepts = biscuit::concepts;

export namespace biscuit::seq {

	using seq_id_t = std::string;
	using clock_t = std::chrono::high_resolution_clock;
	using ms_t = std::chrono::milliseconds;
	using us_t = std::chrono::microseconds;

	//-------------------------------------------------------------------------
	/// @brief exception storing file and line, function name
	class xException : public std::runtime_error {
	public:
		using this_t = xException;
		using base_t = std::runtime_error;

	public:
		using base_t::operator=;

		xException(std::string msg, std::source_location sl = std::source_location::current()) noexcept :
			base_t(std::format("{}\n\nFile\n\t{}:{}\n\nFunction\n\t{}", msg, sl.file_name(), sl.line(), sl.function_name())) {}
	};

	//-------------------------------------------------------------------------
	struct suspend_or_not {
		bool bAwaitReady{};
		bool await_ready() const noexcept { return bAwaitReady; }
		constexpr void await_suspend(std::coroutine_handle<>) const noexcept {}
		constexpr void await_resume() const noexcept {}
	};

	//-------------------------------------------------------------------------
	/// @brief used for scheduling
	struct sState {
	public:
		clock_t::time_point tNextDispatch{};
		mutable clock_t::time_point tNextDispatchChild{ clock_t::time_point::max() };	// cache
		//bool bDone{false};

		struct sPredicate {
			std::function<bool()> func;
			clock_t::time_point t0;
			clock_t::duration interval, timeout;
			std::promise<bool> result;
		};
		sPredicate pred;

	public:
		sState(clock_t::time_point t = clock_t::now()) : tNextDispatch(t) {}
		sState(clock_t::duration d) {
			tNextDispatch = (d.count() == 0) ? clock_t::time_point{} : clock_t::now() + d;
		}
		sState(std::suspend_always) : tNextDispatch(clock_t::time_point::max()) {}
		sState(std::suspend_never) : tNextDispatch{} {}
		sState(sState const&) = default;
		sState(sState&&) = default;
		sState& operator = (sState const&) = default;
		sState& operator = (sState&&) = default;

		void Clear() { *this ={}; }
	};

	//-------------------------------------------------------------------------
	template < typename tResult >
	class TSimpleCoroutineHandle;
	class ICoroutineHandle;
	template < typename tResult >
	class TCoroutineHandle;
	template < typename tResult, template < typename tResult2 > typename tCoroutineHandle >
	struct TPromise;

	//-------------------------------------------------------------------------
	/// @brief simple coroutine handle
	template < typename tResult >
	class TSimpleCoroutineHandle : protected std::coroutine_handle<TPromise<tResult, TSimpleCoroutineHandle>> {
	public:
		using promise_type = TPromise<tResult, TSimpleCoroutineHandle>;
		using this_t = TSimpleCoroutineHandle;
		using base_t = std::coroutine_handle<promise_type>;

	public:
		TSimpleCoroutineHandle(std::coroutine_handle<promise_type>&& h) noexcept : base_t(std::move(h)) { h = nullptr; }
		TSimpleCoroutineHandle(TSimpleCoroutineHandle const&) = delete;
		TSimpleCoroutineHandle(TSimpleCoroutineHandle&& b) noexcept : base_t(std::move(b)) { ((base_t&)b) = nullptr; }
		TSimpleCoroutineHandle& operator = (TSimpleCoroutineHandle const&) = delete;
		TSimpleCoroutineHandle& operator = (TSimpleCoroutineHandle&& b) noexcept { ((base_t&)*this) = std::move(b); ((base_t&)b) = nullptr; return *this; }
		using base_t::base_t;

	public:
		using base_t::from_promise;
		using base_t::promise;
		using base_t::operator bool;
		void Destroy() {
			if (*(base_t*)this)
				this->destroy();
			*(base_t*)this = nullptr;
		}
		bool Valid() const { return (bool)*(base_t*)this; }
		void Resume() { this->resume(); }
		bool Done() const { return this->done(); }
		std::exception_ptr Exception() const { return this->promise().m_exception; }
	};


	//-------------------------------------------------------------------------
	/// @brief coroutine handle
	class ICoroutineHandle {	// interface, pure virtual
	public:
		virtual ~ICoroutineHandle() = default;

		virtual void Destroy() = 0;
		virtual bool Valid() const = 0;
		virtual void Resume() = 0;
		virtual bool Done() const = 0;
		virtual std::exception_ptr Exception() const = 0;
	};

	//-------------------------------------------------------------------------
	/// @brief 
	template < typename tResult >
	class TCoroutineHandle : protected std::coroutine_handle<TPromise<tResult, TCoroutineHandle>>, public ICoroutineHandle {
	public:
		using this_t = TCoroutineHandle;
		using base_t = std::coroutine_handle<TPromise<tResult, TCoroutineHandle>>;
		using promise_type = TPromise<tResult, TCoroutineHandle>;

	public:
		TCoroutineHandle(std::coroutine_handle<promise_type>&& h) : base_t(std::exchange(h, nullptr)) {}
		TCoroutineHandle(TCoroutineHandle const&) = delete;
		TCoroutineHandle(TCoroutineHandle&& b) : base_t(std::move(b)) { ((base_t&)b) = nullptr; }
		TCoroutineHandle& operator = (std::nullptr_t) { Destroy(); return *this; }
		TCoroutineHandle& operator = (TCoroutineHandle const&) = delete;
		TCoroutineHandle& operator = (TCoroutineHandle&& b) { Destroy(); *(base_t*)this = std::move(b);  ((base_t&)b) = nullptr; return *this;  }
		virtual ~TCoroutineHandle() { Destroy(); }

	public:
		using base_t::from_promise;
		using base_t::promise;
		using base_t::operator bool;
		virtual void Destroy() {
			if (*(base_t*)this)
				this->destroy();
			*(base_t*)this = nullptr;
		}
		virtual bool Valid() const { return (bool)*(base_t*)this; }
		virtual void Resume() { this->resume(); }
		virtual bool Done() const { return this->done(); }
		virtual std::exception_ptr Exception() const { return this->promise().m_exception; }
	};

	//-------------------------------------------------------------------------
	/// @brief 
	template < typename tResult, template < typename tResult2 > typename tCoroutineHandle >
	struct TPromise {
		using result_t = tResult;
		using coroutine_t = tCoroutineHandle<result_t>;
		std::promise<result_t> m_result;
		std::exception_ptr m_exception;

		coroutine_t get_return_object() {
			return coroutine_t::from_promise(*this);
		}
		std::suspend_always initial_suspend() { return {}; }
		std::suspend_always final_suspend() noexcept { return {}; }
		void unhandled_exception() { m_exception = std::current_exception(); }

		std::suspend_always yield_value(result_t&& v) {
			m_result.set_value(std::move(v));
			return {};
		}
		//void return_void() {}
		void return_value(result_t&& v) {
			m_result.set_value(std::move(v));
		}
	};

}	// namespace biscuit::seq


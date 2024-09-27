module;

///////////////////////////////////////////////////////////////////////////////
//
// mutex.ixx
//
//
// 2018.01.15. recursive_recursive_shared_mutex (extension of std::recursive_mutex)
// 2019.07.24. QL -> GTL
// 2022.04.07. shared_recursive_mutex - refactoring
// 2024-09-27. biscuit -> shared_recursive_mutex (renamed)
//
// PWH
//
///////////////////////////////////////////////////////////////////////////////


#include "biscuit/config.h"
#include "biscuit/macro.h"

export module biscuit.mutex;
import std;
import biscuit.aliases;
import biscuit.concepts;

export namespace biscuit {

	//------------------------------------------------------------------------
	/// @brief null mutex
	class null_mutex {
	public:
		//null_mutex() noexcept {}
		//~null_mutex() noexcept {}

		void lock() const { // lock the mutex
		}
		[[nodiscard]] bool try_lock() const { // try to lock the mutex
			return true;
		}
		void unlock() const { // unlock the mutex
		}

		void lock_shared() const noexcept {}
		bool try_lock_shared() const noexcept {
			return true;
		}
		void unlock_shared() const noexcept {}
	};

	//------------------------------------------------------------------------
	/// @brief mutex - spin lock
	class spin_mutex {
	protected:
		std::atomic<bool> m_flag;
	public:
		//null_mutex() noexcept {}
		//~null_mutex() noexcept {}

		spin_mutex(bool bOwn = false) noexcept : m_flag(bOwn) {}
		spin_mutex(spin_mutex const&) = delete;
		spin_mutex& operator = (spin_mutex const&) = delete;
		spin_mutex(spin_mutex&&) = default;
		spin_mutex& operator = (spin_mutex&&) = default;

		void lock() { // lock the mutex
			while (!m_flag.exchange(true)) {
				std::this_thread::yield();
			}
		}
		[[nodiscard]] bool try_lock() { // try to lock the mutex
			return !m_flag.exchange(true);
		}
		void unlock() { // unlock the mutex
			m_flag = false;
		}
	};

	//------------------------------------------------------------------------
	/// @brief recursive shared mutex
	class shared_recursive_mutex {
	public:
		using this_t = shared_recursive_mutex;
		//using base_t = std::shared_mutex;
	private:
		std::shared_mutex m_mutex;
		std::atomic<std::thread::id> m_owner;
		size_t m_counter{};

		std::mutex m_mtxShared;
		std::map<std::thread::id, size_t> m_mapShared;

	public:
		//using base_t::base_t;
		shared_recursive_mutex() = default;
		shared_recursive_mutex(shared_recursive_mutex const&) = delete;
		shared_recursive_mutex& operator=(shared_recursive_mutex const&) = delete;

		void lock() noexcept {
			//_Smtx_lock_exclusive(&_Myhandle);
			if (auto idCurrent = std::this_thread::get_id(); idCurrent == m_owner) {
				m_counter++;
			}
			else {
				m_mutex.lock();
				m_owner = idCurrent;
				m_counter = 1;
			}
		}
		void lock_shared() noexcept {
			if (auto idCurrent = std::this_thread::get_id(); idCurrent == m_owner) {
				// if lock/unlock_shared or unlock/lock_shared crosses, it will fail.
				lock();
			}
			else {
				size_t n{};
				{
					std::lock_guard lock(m_mtxShared);
					n = m_mapShared[idCurrent];
				}
				if (n == 0) {
					m_mutex.lock_shared();
					{
						std::lock_guard lock(m_mtxShared);
						m_mapShared[idCurrent]++;
					}
				}
			}
		}
		bool try_lock() noexcept {
			auto idCurrent = std::this_thread::get_id();
			if (idCurrent == m_owner) {
				m_counter++;
				return true;
			}
			if (m_mutex.try_lock()) {
				m_owner = idCurrent;
				m_counter = 1;
				return true;
			}
			return false;
		}
		bool try_lock_shared() noexcept {
			if (auto idCurrent = std::this_thread::get_id(); idCurrent == m_owner) {
				m_counter++;
			} else {
				std::lock_guard lock(m_mtxShared);
				auto n = m_mapShared[idCurrent];
				if (n == 0) {
					if (!m_mutex.try_lock_shared())
						return false;
				}
				m_mapShared[idCurrent]++;
			}
			return true;
		}
		void unlock() noexcept {
			if (--m_counter == 0) {
				m_owner.store({});
				m_mutex.unlock();
			}
		}
		void unlock_shared() noexcept {
			if (auto idCurrent = std::this_thread::get_id(); idCurrent == m_owner) {
				unlock();
			} else {
				size_t n{};
				{
					std::lock_guard lock(m_mtxShared);
					n = --m_mapShared[idCurrent];
				}
				if (n == 0)
					m_mutex.unlock_shared();
			}
		}

	};

	// Mutex Locker
	// inspired from :
	//			https://www.reddit.com/r/cpp/comments/12def1f/mutexprotected_a_c_pattern_for_easier_concurrency/
	//
	template < typename T, typename TMutex = std::mutex >
	class TMutexLocker {
	protected:
		T m_value;
		mutable TMutex m_mutex;

	public:
		using value_type = T;
		using mutex_type = TMutex;

		template < typename ... TArgs >
		TMutexLocker(TArgs&& ... args) : m_value(std::forward<TArgs>(args) ...) {}

		struct Locker {
			TMutexLocker& m_locker;
			std::unique_lock<TMutex> m_lock;
			Locker(TMutexLocker& locker) : m_locker(locker), m_lock(locker.m_mutex) {}
			Locker(const Locker&) = delete;
			Locker(Locker&&) = default;
			Locker& operator=(const Locker&) = delete;
			Locker& operator=(Locker&&) = delete;
			T& operator*() = delete;
			const T& operator*() const = delete;
			T* operator->() noexcept { return &m_locker.m_value; }
			const T* operator->() const noexcept { return &m_locker.m_value; }
		};

		Locker Lock() noexcept {
			return Locker(*this);
		}

		T Release() noexcept {
			std::unique_lock lock(m_mutex);
			return std::move(m_value);
		}

	};


}	// namespace biscuit


#include <catch.hpp>
#include "biscuit/biscuit.h"
#include "biscuit/dependencies_fmt.h"

import std;
import biscuit;

using namespace std::literals;

constexpr static auto const ATTR = "[mutex]";

namespace test_mutex {

	TEST_CASE("shared_recursive_mutex", ATTR) {
		biscuit::shared_recursive_mutex m;

		// Test locking and unlocking
		SECTION("Lock and unlock") {
			m.lock();
			REQUIRE(m.try_lock()); // The mutex is locked by this thread, so try_lock should return true
			m.unlock();
			REQUIRE(m.try_lock()); // The mutex is unlocked, so try_lock should return true
		}

		// Test recursive locking
		SECTION("Recursive locking") {
			m.lock();
			m.lock(); // Recursive lock
			REQUIRE(m.try_lock() == true); // Recursive lock is allowed, so try_lock should return true
			m.unlock();
			m.unlock();
			REQUIRE(m.try_lock() == true); // The mutex is unlocked, so try_lock should return true
		}

		// Test multiple threads
		SECTION("Multiple threads") {
			std::vector<std::thread> threads;
			int counter = 0;

			// Create multiple threads that increment the counter
			constexpr int nThread = 10;
			constexpr int nLoop = 10'000;
			std::barrier barrier(nThread);
			for (int i = 0; i < nThread; i++) {
				threads.emplace_back([&]() {
					barrier.arrive_and_wait();
					for (int i = 0; i < nLoop; i++) {
						m.lock();
						counter++;
						std::this_thread::yield();
						m.unlock();
						std::this_thread::yield();
					}
				});
			}
			threads.emplace_back([&]() {
				do {
					std::this_thread::sleep_for(10ms);

					std::shared_lock lock(m);
					int c = counter;
					if (c >= nThread * nLoop)
						break;
					fmt::println("Counter: {}", c);
				} while(true);
			});

			// Wait for all threads to finish
			for (auto& thread : threads) {
				thread.join();
			}

			REQUIRE(counter == nThread * nLoop); // The counter should be incremented by each thread
		}

	}

}	// namespace


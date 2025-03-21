#include <catch.hpp>
#include "biscuit/biscuit.h"
//#include "biscuit/dependencies_fmt.h"

import std;
import fmt;
import biscuit;

static constexpr auto const ATTR = "[sequence_tReturn]";

namespace biscuit::seq::test {

	using namespace std::literals;
	//using namespace biscuit::literals;
	namespace chrono = std::chrono;

	using seq_t = biscuit::seq::xSequenceTReturn;
	template < typename tReturn >
	using tcoro_t = seq_t::tcoro_t<tReturn>;

	tcoro_t<std::string> SeqReturningString(seq_t& seq) {
		auto t0 = chrono::steady_clock::now();
		auto str = fmt::format("{} ended. take {}", seq.GetName(), chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t0));

		co_return std::move(str);
	}

	tcoro_t<int> SeqReturningInt(seq_t& seq) {
		fmt::print("Start: {}\n", seq.GetName());

		// step 1
		// do something ...
		fmt::print("SeqReturningInt : step1 wait 1s ...\n");
		co_await seq.WaitFor(1000ms);

		// step 2
		// do something ...
		fmt::print("SeqReturningInt : step2 wait 1s, printing ...\n");
		bool bOK = co_await seq.Wait([t0 = biscuit::seq::clock_t::now()] {
			auto t = biscuit::seq::clock_t::now();
			fmt::print("SeqReturningInt : step2 {}\n", chrono::duration_cast<chrono::milliseconds>(t - t0));
			return t - t0 > 1s;
		}, 100ms, 2s);

		// step 3
		fmt::print("SeqReturningInt : step3 wait...\n");
		auto i = 10;
		std::jthread count_down( [&](auto stop) {
			while (!stop.stop_requested()) {
				fmt::print("SeqReturningInt : step3 count down {}\n", i--);
				std::this_thread::sleep_for(100ms);
			}
		});

		co_await seq.Wait([&, t0 = biscuit::seq::clock_t::now()] {	// wait until i == 0
			return i == 0;
		}, 1ms);
		count_down.request_stop();
		count_down.join();

		// step 4
		fmt::print("SeqReturningInt : step4...\n");

		co_return bOK ? 3141592 : -3141592;
	}

	tcoro_t<int> Seq1(seq_t& seq) {
		fmt::print("{} : START\n", seq.GetName());
		std::function<bool()> pred = [t0 = std::chrono::steady_clock::now()] {
			return std::chrono::steady_clock::now() - t0 >= 10s;
		};
		[[maybe_unused]] bool bOK = co_await seq.Wait(pred, 10ms, 10s);
		fmt::print("{} : END\n", seq.GetName());
		co_return 0;
	}

	tcoro_t<int> Seq2(seq_t& seq) {
		fmt::print("{} : START\n", seq.GetName());
		[[maybe_unused]] bool bOK = co_await seq.Wait([t0 = std::chrono::steady_clock::now()] {
			return std::chrono::steady_clock::now() - t0 >= 5s;
		}, 1ms, 10s);
		fmt::print("{} : END\n", seq.GetName());
		co_return 0;
	}

	tcoro_t<int> Seq3(seq_t& seq) {
		auto i0 = co_await seq.Wait([&, t0 = biscuit::seq::clock_t::now()] {return true;}, 100ms);
		fmt::println("i0 = {}", i0);
		auto i1 = co_await seq.Wait([&, t0 = biscuit::seq::clock_t::now()] {return true;}, 100ms);
		fmt::println("i1 = {}", i1);
		auto i2 = co_await seq.Wait([&, t0 = biscuit::seq::clock_t::now()] {return true;}, 100ms);
		fmt::println("i2 = {}", i2);
		co_await seq.WaitFor(100ms);
		co_return 0;
	}


	TEST_CASE("specify_return_type", ATTR) {
		biscuit::seq::xSequenceTReturn driver;

		fmt::print("Creating 2 sequences returning string and int respectively\n");

		std::future<std::string> f1 = driver.CreateChildSequence("SeqReturningString", &SeqReturningString);
		std::future<int> f2 = driver.CreateChildSequence("SeqReturningInt", &SeqReturningInt);
		std::future<int> f3 = driver.CreateChildSequence("SeqReturningInt3", &Seq3);

		do {
			auto t = driver.Dispatch();
			if (driver.IsDone())
				break;
			if (auto ts = t - biscuit::seq::clock_t::now(); ts > 3s)
				t = biscuit::seq::clock_t::now() + 3s;
			std::this_thread::sleep_until(t);
		} while (!driver.IsDone());

		fmt::print("Result of SeqReturningString : {}\n", f1.get());
		fmt::print("Result of SeqReturningInt : {}\n", f2.get());

		fmt::print("End\n");
	}

}	// namespace biscuit::seq::test

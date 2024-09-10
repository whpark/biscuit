#include <catch.hpp>
#include "biscuit/biscuit.h"

import std;
import biscuit;

using namespace std::literals;

namespace test_memory {

	struct Base {
		virtual ~Base() {}
		virtual std::unique_ptr<Base> clone() const = 0;
		virtual std::string Print() = 0;
	};

	struct A : Base {
		int i{3};

		A() = default;
		A(int i) : i(i) {}

		std::unique_ptr<Base> clone() const override {
			return std::make_unique<A>(*this);
		}

		std::string Print() override {
			return std::format("A{} ", i);
		}
	};

	struct B : A {
		double b{4};

		B() = default;
		B(double b) : b(b) {}

		std::unique_ptr<Base> clone() const override {
			return std::make_unique<B>(*this);
		}

		std::string Print() override {
			return std::format("B{} ", b);
		}
	};

	TEST_CASE("cloneable_ptr") {
		biscuit::TCloneablePtr<Base> a = std::make_unique<B>();
		biscuit::TCloneablePtr<Base> b = a;
		REQUIRE(b);

		std::vector<biscuit::TCloneablePtr<Base>> lst;
		lst.push_back(std::make_unique<A>(1));
		lst.push_back(std::make_unique<B>(9.0));
		lst.push_back(std::make_unique<B>(10.0));
		lst.push_back(std::make_unique<A>(3));
		lst.push_back(std::make_unique<A>(2));

		std::string s1;
		for (auto const& v : lst) s1 += v->Print();

		REQUIRE(s1 == "A1 B9 B10 A3 A2 ");

		std::deque<biscuit::TCloneablePtr<Base>> lst2;
		std::ranges::copy(lst, std::back_inserter(lst2));

		std::string s2;
		for (auto const& v : lst2) s2 += v->Print();

		REQUIRE(s1 == s2);

	}

}	// namespace


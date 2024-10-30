#include <catch.hpp>
#include "biscuit/biscuit.h"

import std;
import biscuit;

using namespace std::literals;

constexpr static auto const ATTR = "[memory]";

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

	TEST_CASE("cloneable_ptr", ATTR) {
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

	class aaa { 
	public:
		int i, k;
	public:
		virtual std::unique_ptr<aaa> clone() const {
			return std::make_unique<aaa>(*this);
		}
		std::unique_ptr<aaa> cloneSelf() const {
			return std::make_unique<aaa>(*this);
		}

		auto operator <=> (aaa const&) const = default;
	};
	class bbb : public aaa { 
	public:
		virtual std::unique_ptr<aaa> clone() const {
			return std::make_unique<bbb>(*this);
		}
		static std::unique_ptr<bbb> cloneSelf(bbb const& self) {
			return std::make_unique<bbb>(self);
		}

		auto operator <=> (bbb const&) const = default;
	};

	TEST_CASE("cloneable") {
		using namespace biscuit;
		TCloneablePtr<aaa> a = std::make_unique<aaa>();
		a->i = 42;
		a->k = 20;
		TCloneablePtr<aaa> a2 = a;
		REQUIRE(a == a2);
		//TCloneablePtr<aaa, decltype(&aaa::cloneSelf)> a3 = a2;
		TCloneablePtr<aaa> a4 = std::move(a);
		REQUIRE(a4 != a);
		REQUIRE(a4 == a2);

		struct cloner {
			std::unique_ptr<bbb> operator () (bbb const& self) {
				return bbb::cloneSelf(self);
			}
		};
		TCloneablePtr<bbb, cloner> b;
		b = std::make_unique<bbb>();
		TCloneablePtr<bbb, TStaticCloner<bbb>> b2 = b;
		//TCloneablePtr<bbb, TCloner<bbb>> b4 = b2;
		TCloneablePtr<bbb, TStaticCloner<bbb>> b5 = b2;
		TCloneablePtr<aaa, decltype([](auto const& self) { return std::make_unique<aaa>(self); })> a5 = std::make_unique<bbb>();
		TCloneablePtr<aaa> a6 = a5;
		//b = a;
	}

}	// namespace


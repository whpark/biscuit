module;

export module biscuit.memory;
import std;
import biscuit.aliases;
import biscuit.concepts;

namespace biscuit {

	export template < /*concepts::cloneable */typename T >
	class TCloneablePtr : public std::unique_ptr<T> {
	public:
		using base_t = std::unique_ptr<T>;
		using this_t = TCloneablePtr<T>;

		using base_t::base_t;
		//TCloneablePtr(this_t&& other) : base_t(std::move(other)) {}
		using base_t::operator =;
		using base_t::operator *;
		using base_t::operator ->;
		using base_t::operator bool;

		TCloneablePtr(std::unique_ptr<T>&& other) : base_t(std::move(other)) {}
		TCloneablePtr(this_t&& other) : base_t(std::move(other)) {}
		// copy constructor
		TCloneablePtr(this_t const& other) : base_t(other ? other->clone() : nullptr) {}

		TCloneablePtr& operator = (std::unique_ptr<T>&& other) { base_t::operator = (std::move(other)); return *this; }
		TCloneablePtr& operator = (this_t&& other) { base_t::operator = (std::move(other)); return *this; }
		// copy operator
		TCloneablePtr& operator = (this_t const& other) { *this = other ? std::move(other->clone()) : nullptr; return*this;}
	};

}

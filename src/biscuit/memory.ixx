module;

export module biscuit.memory;
import std;
import biscuit.aliases;
import biscuit.concepts;

export namespace biscuit {

	template < typename T >
	struct TCloner {
		std::unique_ptr<T> operator () (T const& self) const { return self.clone(); }
	};
	template < typename T >
	struct TStaticCloner {
		std::unique_ptr<T> operator () (T const& self) const { return std::make_unique<T>(self); }
	};

	template < /*concepts::cloneable */typename T, class CLONER = TCloner<T> >
	class TCloneablePtr : public std::unique_ptr<T> {
	public:
		using base_t = std::unique_ptr<T>;
		using this_t = TCloneablePtr;

		using base_t::base_t;
		//TCloneablePtr(this_t&& other) : base_t(std::move(other)) {}
		using base_t::operator =;
		using base_t::operator *;
		using base_t::operator ->;
		using base_t::operator bool;

		static inline CLONER cloner;

		TCloneablePtr(std::unique_ptr<T>&& other) : base_t(std::move(other)) {}
		TCloneablePtr(this_t&& other) : base_t(std::move(other)) {}
		// copy constructor
		TCloneablePtr(std::unique_ptr<T> const& other) : base_t(other ? cloner(*other) : nullptr) {}
		TCloneablePtr(this_t const& other) : base_t(other ? cloner(*other) : nullptr) {}

		TCloneablePtr& operator = (std::unique_ptr<T>&& other) { base_t::operator = (std::move(other)); return *this; }
		TCloneablePtr& operator = (std::unique_ptr<T> const& other) { base_t::operator = (cloner(*other)); return *this; }
		TCloneablePtr& operator = (this_t&& other) { base_t::operator = (std::move(other)); return *this; }
		TCloneablePtr& operator = (this_t const& other) { base_t::operator = (cloner(*other)); return *this; }

		template < typename U, class CLONER2 >
		TCloneablePtr& operator = (TCloneablePtr<U, CLONER2>&& other) {
			this->reset(other.release());
			return*this;
		}
		template < typename U, class CLONER2 >
		TCloneablePtr& operator = (TCloneablePtr<U, CLONER2> const& other) {
			static CLONER2 cloner2;
			this->reset(other ? cloner2(*other).release() : nullptr);
			return*this;
		}
	};

	template < typename T1, typename T2, typename ... targs, typename ... targs2 >
	bool operator == (TCloneablePtr<T1, targs...> const& a, TCloneablePtr<T2, targs2...> const& b) {
		bool bEmptyA = !a;
		bool bEmptyB = !b;
		if (bEmptyA and bEmptyB) return true;
		else if (bEmptyA or bEmptyB) return false;
		return *a == *b;
	}
	template < typename T1, typename T2, typename ... targs, typename ... targs2 >
	bool operator != (TCloneablePtr<T1, targs...> const& a, TCloneablePtr<T2, targs2...> const& b) {
		return !(a == b);
	}
	template < typename T1, typename T2, typename ... targs, typename ... targs2 >
	bool operator < (TCloneablePtr<T1, targs...> const& a, TCloneablePtr<T2, targs2...> const& b) {
		bool bEmptyA = !a;
		bool bEmptyB = !b;
		if (bEmptyA and bEmptyB) return false;
		else if (bEmptyA) return true;
		else if (bEmptyB) return false;
		return *a < *b;
	}
	template < typename T1, typename T2, typename ... targs, typename ... targs2 >
	bool operator > (TCloneablePtr<T1, targs...> const& a, TCloneablePtr<T2, targs2...> const& b) {
		return b < a;
	}
	template < typename T1, typename T2, typename ... targs, typename ... targs2 >
	bool operator <= (TCloneablePtr<T1, targs...> const& a, TCloneablePtr<T2, targs2...> const& b) {
		return a == b or a < b;
	}
	template < typename T1, typename T2, typename ... targs, typename ... targs2 >
	bool operator >= (TCloneablePtr<T1, targs...> const& a, TCloneablePtr<T2, targs2...> const& b) {
		return b <= a;
	}

}


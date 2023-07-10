#ifndef BASIC_VARIANT_H
#define BASIC_VARIANT_H

#include <type_traits>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <tuple>
#include <array>

namespace basic_pt_variant
{
namespace detail
{
using index_t = std::uint_fast8_t;
static constexpr index_t invalidValue = index_t(-1);

template <typename...>
struct conjunction : std::true_type {};

template <typename B1>
struct conjunction<B1> : B1 {};

template <typename B1, typename B2>
struct conjunction<B1, B2> : std::conditional<B1::value, B2, B1>::type {};

template <typename B1, typename... Bs>
struct conjunction<B1, Bs...> : std::conditional<B1::value, conjunction<Bs...>, B1>::type {};

template<typename...> struct disjunction : std::false_type
{
};
template<typename B1> struct disjunction<B1> : B1
{
};

template <typename B1, typename B2>
struct disjunction<B1, B2> : std::conditional<B1::value, B1, B2>::type {};

template<typename B1, typename... Bn>
struct disjunction<B1, Bn...>
	: std::conditional<bool(B1::value), B1, disjunction<Bn...>>::type
{
};

template<size_t ...Sizes>
struct StaticMax;

template<size_t First>
struct StaticMax<First>
{
	static constexpr size_t value = First;
};

template<size_t First, size_t ...Rest>
struct StaticMax<First, Rest...>
{
	static constexpr size_t value = First > StaticMax<Rest...>::value ? First : StaticMax<Rest...>::value;
};

template <typename... Types>
struct VariantHelper;

template <typename T, typename... Types>
struct VariantHelper<T, Types...>
{
	inline static void destroy(const index_t index, void* data)
	{
		if (index == sizeof...(Types))
		{
			reinterpret_cast<T*>(data)->~T();
		}
		else
		{
			VariantHelper<Types...>::destroy(index, data);
		}
	}

	inline static void move(const index_t old_type_index, void* old_value, void* new_value)
	{
		if (old_type_index == sizeof...(Types))
		{
			new (new_value) T(std::move(*reinterpret_cast<T*>(old_value)));
		}
		else
		{
			VariantHelper<Types...>::move(old_type_index, old_value, new_value);
		}
	}

	inline static void copy(const index_t old_type_index, const void* old_value, void* new_value)
	{
		if (old_type_index == sizeof...(Types))
		{
			new (new_value) T(*reinterpret_cast<const T*>(old_value));
		}
		else
		{
			VariantHelper<Types...>::copy(old_type_index, old_value, new_value);
		}
	}
};

template <>
struct VariantHelper<>
{
	inline static void destroy(const index_t , void*) {}
	inline static void move(const index_t , void*, void*) {}
	inline static void copy(const index_t , const void*, void*) {}
};

// Only a helper, do not use directly
template <typename T, typename ...Types>
struct IndexTrait;

template <typename T, typename First, typename... Types>
struct IndexTrait<T, First, Types...>
{
	static constexpr index_t value = std::is_same<T, First>::value
		? sizeof...(Types)
		: IndexTrait<T, Types...>::value;
};

template <typename T>
struct IndexTrait<T>
{
	static constexpr index_t value = invalidValue;
};

template <typename T, typename... Types>
struct ValueTraits
{
	//using value_t = typename std::remove_const<typename std::remove_reference<T>::type>::type;

	// decrease from sizeof...(Types)-1, invalidValue if T not among Types
	static constexpr index_t reverseIndex = IndexTrait<T, Types...>::value;
	static constexpr bool isValid = reverseIndex != invalidValue;
	static constexpr index_t forwardIndex = isValid ? sizeof...(Types) - reverseIndex - 1 : 0; // increase from 0
	using target_t = typename std::tuple_element<forwardIndex, std::tuple<Types...>>::type;
};
} // detail

template<typename... Types>
class BasicPtVariant
{
	using index_t = detail::index_t;

	static_assert(sizeof...(Types) > 0, "Template parameter type list of variant can not be empty.");
	static_assert(!detail::disjunction<std::is_reference<Types
		> ...>::value, "Variant can not hold reference types.");
	static_assert(!detail::disjunction<std::is_array<Types> ...>::value, "Variant cannot hold array types.");
	static_assert(sizeof...(Types) < std::numeric_limits<index_t>::max(),
		"Internal index type must be able to accommodate all alternatives.");

public:
	using types = std::tuple<Types...>;

	inline BasicPtVariant() noexcept(std::is_nothrow_default_constructible<firstType>::value)
		: m_index(sizeof...(Types) - 1)
	{
		static_assert(std::is_default_constructible<firstType>::value,
			"First type in variant must be default constructible to allow default construction of the sum type.");
		new (m_data.data()) firstType();
	}

	template <typename T, typename Traits = detail::ValueTraits<T, Types...>,
		typename std::enable_if<Traits::isValid && !std::is_same<BasicPtVariant<Types...>, typename Traits::target_t>::value, int>::type = 0>
	inline BasicPtVariant(T&& val) noexcept(std::is_nothrow_constructible<typename Traits::target_t, T&&>::value)
		: m_index(Traits::reverseIndex)
	{
		new (m_data.data()) typename Traits::target_t(std::forward<T>(val));
	}

	inline BasicPtVariant(const BasicPtVariant<Types...>& old)
		: m_index(old.m_index)
	{
		helper_t::copy(old.m_index, old.m_data.data(), m_data.data());
	}

	inline BasicPtVariant(BasicPtVariant<Types...>&& old)
		noexcept(detail::conjunction<std::is_nothrow_move_constructible<Types>...>::value)
		: m_index(old.m_index)
	{
		helper_t::move(old.m_index, old.m_data.data(), m_data.data());
	}


	constexpr index_t index() const noexcept {return sizeof...(Types) - m_index - 1;} // actual index

private:
	using firstType = typename std::tuple_element<0, types>::type;
	using helper_t =  detail::VariantHelper<Types...>;

	enum size : std::size_t
	{
		k_size = detail::StaticMax<sizeof(Types)...>::value
	};

	index_t m_index{ detail::invalidValue };

	alignas(Types...) std::array<unsigned char, size::k_size> m_data;

}; // class BasicPtVariant

template<typename T, typename ...Types>
constexpr bool holds_alternative(const basic_pt_variant::BasicPtVariant<Types...>& v) noexcept
{
	return v.index() == detail::ValueTraits<T, Types...>::forwardIndex;
}
} // namespace basic_pt_variant
#endif //BASIC_VARIANT_H

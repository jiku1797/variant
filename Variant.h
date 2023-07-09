#ifndef VARIANT__VARIANT_H
#define VARIANT__VARIANT_H

#include <type_traits>
#include <cstdint>
#include <limits>
#include <tuple>

namespace basic_pt_variant
{
namespace detail
{
using index_t = std::uint_fast8_t;
static constexpr index_t invalidValue = index_t(-1);

template<typename...> struct disjunction : std::false_type
{
};
template<typename B1> struct disjunction<B1> : B1
{
};
template<typename B1, typename... Bn>
struct disjunction<B1, Bn...>
	: std::conditional<bool(B1::value), B1, disjunction<Bn...>>::type
{
};

template<size_t first, size_t... rest>
struct staticMax;

template<size_t first, size_t second>
struct staticMax<first, second>
{
	static constexpr size_t value = first > second ? first : second;
};

template<size_t first, size_t second, size_t ...rest>
struct staticMax<first, second, rest...>
{
	static constexpr size_t
		value = first >= second ? staticMax<first, rest...>::value : staticMax<second, rest...>::value;
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
	static constexpr index_t forwardIndex = sizeof...(Types) - reverseIndex - 1; // increase from 0
	static constexpr bool isValid = reverseIndex != invalidValue;
	using target_t = typename std::tuple_element<forwardIndex, std::tuple<Types...>>::type;
};
} // details

template<typename... Types>
class BasicPtVariant
{
	using index_t = detail::index_t;

	static_assert(sizeof...(Types) > 0, "Template parameter type list of variant can not be empty.");
	static_assert(!detail::disjunction<std::is_reference<Types
	> ...>::value, "Variant can not hold reference types.");
	static_assert(!detail::disjunction<std::is_array<Types> ...>::value, "Variant can not hold array types.");
	static_assert(sizeof...(Types) < std::numeric_limits<index_t>::max(),
		"Internal index type must be able to accommodate all alternatives.");

public:
	using types = std::tuple<Types...>;

	inline BasicPtVariant() noexcept(std::is_nothrow_default_constructible<firstType>::value)
		: m_index(sizeof...(Types) - 1)
	{
		static_assert(std::is_default_constructible<firstType>::value,
			"First type in variant must be default constructible to allow default construction of the sum type.");
		new (&m_data) firstType();
	}

	template <typename T, typename Traits = detail::ValueTraits<T, Types...>,
		typename Enable = typename std::enable_if<Traits::isValid && !std::is_same<BasicPtVariant<Types...>, typename Traits::target_t>::value>::type >
	inline BasicPtVariant(T&& val) noexcept(std::is_nothrow_constructible<typename Traits::target_t, T&&>::value)
		: m_index(Traits::reverseIndex)
	{
		new (&m_data) typename Traits::target_t(std::forward<T>(val));
	}

	constexpr index_t index() const noexcept {return m_index;}

private:
	using firstType = typename std::tuple_element<0, types>::type;

	enum size : std::size_t
	{
		k_size = detail::staticMax<sizeof(Types)...>::value
	};

	index_t m_index{ detail::invalidValue };

	alignas(Types...) unsigned char m_data[size::k_size];

}; // class BasicPtVariant

template<typename T, typename ...Types>
constexpr bool holds_alternative(const basic_pt_variant::BasicPtVariant<Types...>& v) noexcept
{
	return v.index() == detail::ValueTraits<T, Types...>::reverseIndex;
}
} // namespace basic_pt_variant
#endif //VARIANT__VARIANT_H

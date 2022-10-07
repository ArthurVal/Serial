#pragma once

#include <cassert>
#include <type_traits>
#include <utility>

namespace serial {

/**
 * \brief Serializer class, used to implement serialization function
 */
template <class T>
struct Serializer {
  template <class Context>
  constexpr auto Serialize(T const&, Context&)
      -> decltype(std::declval<Context&>().Output());
};

/**
 * \brief Struct used to store the argument and the serializer associate to it
 */
template <class T, class U>
struct SerialArg {
  using ArgType = std::decay_t<T>;
  T arg;

  using SerializerType = std::decay_t<U>;
  U serializer;
};

/**
 * \brief Create a SerialArg from the copy of the argument and serializer input
 *
 * \tparam T Argument type
 * \tparam U Serializer type
 *
 * \param[in] arg
 * \param[in] serializer
 *
 * \return SerialArg<T, U>
 */
template <class T, class U>
constexpr auto MakeSerialArg(T&& arg, U&& serializer) -> SerialArg<T, U> {
  return SerialArg<T, U>{std::forward<T>(arg), std::forward<U>(serializer)};
}

/**
 * \brief Create a SerialArg by forwarding the argument and serializer input
 *
 * \tparam T Argument type
 * \tparam U Serializer type
 *
 * \param[in] arg TODO
 * \param[in] serializer TODO
 *
 * \return SerialArg<T&&, U&&>
 */
template <class T, class U>
constexpr auto ForwardAsSerialArg(T&& arg, U&& serializer)
    -> SerialArg<T&&, U&&> {
  return SerialArg<T&&, U&&>{std::forward<T>(arg), std::forward<U>(serializer)};
}

namespace traits {

template <class T>
struct IsSerialArg : std::false_type {};

template <class T, class U>
struct IsSerialArg<SerialArg<T, U>> : std::true_type {};

template <class T>
constexpr bool IsSerialArg_v = IsSerialArg<T>::value;

}  // namespace traits

/**
 * \brief Promote arg to a SerialArg, with its associated Serializer only if T
 * is not already a SerialArg.
 *
 * \param[in] arg TODO
 * \return SerialArg TODO
 */
template <class T>
auto PromoteToSerialArg(T&& arg) -> decltype(auto) {
  using DecayedType = std::decay_t<T>;
  if constexpr (traits::IsSerialArg_v<DecayedType>) {
    return std::forward<T>(arg);
  } else {
    return ForwardAsSerialArg(std::forward<T>(arg), Serializer<DecayedType>{});
  }
}

/**
 * \brief Sequencially serialize all input arguments into the provided Context
 *
 * From a given output context, providing at minimum `.Output() -> OutputIt` and
 * `.AdvanceTo(OutputIt)`, we serialize ALL inputs argument given into this
 * context.
 * Serialization happens by default constructing a given serializer of type
 * ::hsm::serial::Serializer<T>{}, for an argument of type T, and then calling
 * the serializer `.Serialize(ctx, arg) -> OutputIt` function.
 * Then the returned iterator is forwarded to the OutputContext throught the
 * `.AdvanceTo(OutputIt)` method on the context.
 *
 * The requirement for this to work is then to:
 * - Declare the Serializer<T> class for a given argument type T;
 * - Specialize the `Serializer<T>.Serialize<Context>(const T& arg, Context&
 *   ctx) -> It` method in order to perform the serialization of a given type T;
 *
 * You can also provide a custom serializer, associated to an argument (not
 * necessary associated to the type T), by using the SerialArg<T,U>{} data
 * structure. This way you can construct the Serializer by your own, if it
 * must/can be constructed using arguments (instead of the default constructor
 * used when not using SerialArg<T,U>), and forwards it to the serialize
 * function. You can also provide a serializer Serializer<U> associated to and
 * argument of type T, as long as `Serializer<U>.Serialize(T, Context)` is
 * defined.
 *
 * \tparam OutputContext Provides `.Output()->OutputIt` & `.AdvanceTo(OutputIt)`
 * \tparam ...T Argument types to serialize.
 *
 * \param[inout] ctx The output context used during the serialization
 * \param[in] args... All arguments to serialize into ctx
 */
template <class OutputContext, class... T>
constexpr auto Serialize(OutputContext& ctx, T&&... args) -> void {
  constexpr auto NumberOfArgs = sizeof...(T);
  if constexpr (NumberOfArgs == 1) {
    auto&& [value, serializer] = PromoteToSerialArg(args...);
    ctx.AdvanceTo(
        serializer.Serialize(std::forward<decltype(value)>(value), ctx));

  } else if constexpr (NumberOfArgs > 1) {
    (Serialize(ctx, std::forward<T>(args)), ...);
  } else {
    // NumberOfArgs == 0
  }
}

/**
 * \brief Basic Output Context that simply decorate an Output Iterator
 */
template <class _It>
struct BasicOutputContext {
  using Iterator = _It;

  constexpr BasicOutputContext() = delete;
  constexpr explicit BasicOutputContext(Iterator d_first) : m_out(d_first) {}

  constexpr auto Output() const -> Iterator { return m_out; }
  constexpr auto AdvanceTo(Iterator it) -> Iterator { return m_out = it; }

 private:
  Iterator m_out;
};

/**
 * \brief Sequencialy serialize all input arguments into the provided Output It
 *
 * Sepcialization of the `Serialize()` function using a
 * BasicOutputContext{d_first} as output context.
 *
 * \tparam OutputIt Must satisfy Output Iterators concepts
 * \tparam ...T Argument Types
 *
 * \param[out] d_first TODO
 * \param[in] args... TODO
 *
 * \return Iterator pointing one past the last element inserted into the
 *         destination range
 */
template <class OutputIt, class... T>
constexpr auto SerializeInto(OutputIt d_first, T&&... args) -> OutputIt {
  auto ctx = BasicOutputContext{d_first};
  Serialize(ctx, std::forward<T>(args)...);
  return ctx.Output();
}

}  // namespace hsm::serial

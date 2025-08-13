#ifndef _UTILS_H_
#define _UTILS_H_

#include <chrono>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>
// #include "logger.h"

using namespace std;
namespace utils {

template <typename T>
concept PrintableElement = requires(T t) {
  { cout << t } -> std::same_as<std::ostream &>;
}
&&!std::is_enum_v<T>;

template <typename T>
concept ContainerWithPrintableElement = requires(T c) {
  {c.begin()};
  { cout << (*(c.begin())) } -> std::same_as<std::ostream &>;
  {c.end()};
}
&&!PrintableElement<T>;

template <typename T>
concept NestedContainerWithPrintableElement = requires(T c) {
  {c.begin()};
  { cout << (*((*(c.begin())).begin())) } -> std::same_as<std::ostream &>;
  {c.end()};
}
&&!PrintableElement<T> && !ContainerWithPrintableElement<T>;

template <typename T>
concept AnyInput = PrintableElement<T> || ContainerWithPrintableElement<T> ||
    NestedContainerWithPrintableElement<T>;

template <PrintableElement... Args> void Print(Args... args) {
  ((cout << std::setprecision(20) << args << " "), ...) << "";
}

inline void Println() { cout << "\n"; }

// print all element in a container
void Print(ContainerWithPrintableElement auto c) {
  Print("[");
  for (const auto &val : c) {
    Print(val);
  }
  Print("]");
}

// print all element in a nested container
void Print(NestedContainerWithPrintableElement auto c) {
  if (c.size() == 0) {
    Print("[]");
    return;
  } else {
    Print("[");
    for (const auto &sc : c) {
      Print(sc);
      Print(",");
    }
    cout << "\x08\x08"; // 退格
    Print("]");
  }
}

// print anything element if it is printable
inline void println() { cout << "\n"; }

template <AnyInput... T1> void print(T1... any_inputs) {
  (Print(any_inputs), ...);
}
template <AnyInput... T1> void println(T1... any_inputs) {
  (Print(any_inputs), ...);
  cout << "\n";
}

template <typename T>
concept Arithmetic = std::is_arithmetic_v<std::decay_t<T>>;

#if 1 // both pass!

template <typename T>
concept ContainerWithArithmeticElement = requires(T c) {
  {*c.begin()};
}
&&Arithmetic<decltype(*(declval<T>().begin()))>;

template <typename T>
concept NestedContainerWithArithmeticElement = requires(T c) {
  {*c.begin()};
}
&&ContainerWithArithmeticElement<decltype(*(declval<T>().begin()))>;

#else

template <typename T>
concept ContainerWithArithmeticElement = requires(T c) {
  {*c.begin()};
  requires Arithmetic<decltype(*(declval<T>().begin()))>;
};

template <typename T>
concept NestedContainerWithArithmeticElement = requires(T c) {
  {*c.begin()};
  requires ContainerWithArithmeticElement<decltype(*(declval<T>().begin()))>;
};

#endif

// 接收一个容器,将其中的数据全部加起来
template <ContainerWithArithmeticElement C>
typename std::decay_t<decltype(*(declval<C>().begin()))> sum(C &&c) {
  using return_type = typename std::decay_t<decltype(*(declval<C>().begin()))>;
  return_type sumval = return_type(0);
  for (const auto &val : c) {
    sumval += val;
  }
  return sumval;
}

template <NestedContainerWithArithmeticElement C>
typename std::decay_t<
    decltype(*(declval<decltype(*(declval<C>().begin()))>().begin()))>
sum(C &&c) {
  using return_type = typename std::decay_t<decltype(*(
      declval<decltype(*(declval<C>().begin()))>().begin()))>;
  return_type sumval = return_type(0);
  for (const auto &sub_container : c) {
    sumval += sum(sub_container);
  }
  return sumval;
}

// 接收一个容器,将其中的数据全部乘起来
template <ContainerWithArithmeticElement C>
typename std::decay_t<decltype(*(declval<C>().begin()))> prod(C &&c) {
  if (c.size() == 0) {
    return 0;
  }
  using return_type = typename std::decay_t<decltype(*(declval<C>().begin()))>;
  return_type prodval = return_type(1);
  for (const auto &val : c) {
    prodval *= val;
  }
  return prodval;
}

template <NestedContainerWithArithmeticElement C>
typename std::decay_t<
    decltype(*(declval<decltype(*(declval<C>().begin()))>().begin()))>
prod(C &&c) {
  using return_type = typename std::decay_t<decltype(*(
      declval<decltype(*(declval<C>().begin()))>().begin()))>;
  return_type prodval = return_type(1);
  for (const auto &sub_container : c) {
    prodval *= prod(sub_container);
  }
  return prodval;
}

// 计算普通容器的元素乘积
template <ContainerWithArithmeticElement C>
std::decay_t<decltype(*std::declval<C>().begin())> numel(C &&c) {
  if (c.size() == 0) {
    return 0;
  }
  using return_type = std::decay_t<decltype(*std::declval<C>().begin())>;
  return_type prodval = return_type{1};
  for (const auto &val : c) {
    prodval *= val;
  }
  return prodval;
}

// 计算嵌套容器的元素乘积
template <NestedContainerWithArithmeticElement C>
typename std::decay_t<decltype(*(*declval<C>().begin()).begin())> numel(C &&c) {
  if (c.size() == 0) {
    return 0;
  }
  using return_type =
      typename std::decay_t<decltype(*(*declval<C>().begin()).begin())>;
  return_type result = return_type{0};
  for (const auto &inner_container : c) {
    result += numel(inner_container);
  }
  return result;
}

// map/transform
template <typename F, typename C>
requires requires(C c) {
  {c.begin()};
  {c.end()};
} && std::invocable<F,
                    typename std::decay_t<decltype(*std::declval<C>().begin())>>
auto map(F &&f, C &&c) {
  using InputType = typename std::decay_t<decltype(*c.begin())>;
  using ResultType = std::decay_t<decltype(f(std::declval<InputType>()))>;
  std::vector<ResultType> result;
  result.reserve(c.size());
  for (const auto &val : c) {
    result.push_back(f(val));
  }
  return result;
}

inline string shape_to_string(ContainerWithPrintableElement auto &&shape) {
  std::stringstream ss;
  ss << "[";
  for (auto it = shape.begin(); it != shape.end(); it++) {
    ss << (*it);
    if (it + 1 != shape.end()) {
      ss << ",";
    }
  }
  ss << "]";
  return ss.str();
}

inline string
shape_to_string(NestedContainerWithPrintableElement auto &&shape) {
  std::stringstream ss;
  ss << "[";
  for (auto it = shape.begin(); it != shape.end(); it++) {
    ss << "[";
    for (auto sub_it = (*it).begin(); sub_it != (*it).end(); sub_it++) {
      ss << (*sub_it);
      if (sub_it + 1 != (*it).end()) {
        ss << ",";
      }
      // else {
      //   ss << "]";
      // }
    }
    ss << "]";
    if (it + 1 != shape.end()) {
      ss << ",";
    }
  }
  ss << "]";
  return ss.str();
}

template <typename Iterator1, typename Iterator2> class zipiterator {
public:
  zipiterator(Iterator1 it1, Iterator1 it1_end, Iterator2 it2,
              Iterator2 it2_end)
      : _it1(it1), _it2(it2), _it1_end(it1_end), _it2_end(it2_end) {
    this->has_end_iter = true;
  };
  zipiterator(Iterator1 it1, Iterator2 it2) : _it1(it1), _it2(it2) {
    this->has_end_iter = false;
  };
  using value_type = std::tuple<typename Iterator1::value_type,
                                typename Iterator2::value_type>;
  value_type operator*() { return value_type(*(this->_it1), *(this->_it2)); }
  zipiterator &operator++() {
    //        if(this->_it1 == this->_it1_end){
    //            return *
    //        }
    this->_it1++;
    this->_it2++;
    return *this;
  }

  bool operator==(const zipiterator &other) const {
    bool first_eq = (this->_it1 == other._it1);
    bool second_eq = (this->_it2 == other._it2);
    if (first_eq && second_eq) {
      return true;
    } else {
      return false;
    }
  }

  bool operator!=(const zipiterator &other) const {
    //        return this->_it1 != other._it1 || this->_it2 != other._it2;
    // 判断是否迭代到末尾的条件,注意上面这种写法是错误的
    // 因为要同时判断两个长度不一定相等的迭代器是否有某一个到达了末尾
    // 逻辑应该是只要有一个到了末尾,另一个就直接停止迭代,因为再往下迭代会导致较短的一个迭代器崩溃
    // 类似于python的zip语法
    // 还要写一个警告条件,两个迭代器要么都等于end要么都不等于end
    bool warning_cond =
        ((this->has_end_iter) &&
         (other
              .has_end_iter)) && /*与自身进行比较的迭代器中要记住end迭代器，否则无法比较*/
        (((this->_it1 == other._it1_end) &&
          (this->_it2 !=
           other._it2_end)) || /*第一个迭代器停止了，第二个还没停*/
         ((this->_it1 != other._it1_end) &&
          (this->_it2 == other._it2_end))); /*第二个迭代器停止了，第一个还没停*/
    if (warning_cond) {
      // println("warning:utils::zip length not match");
    }

    return !(this->_it1 == other._it1 || this->_it2 == other._it2);
  }

  Iterator1 _it1;
  Iterator2 _it2;
  Iterator1 _it1_end;
  Iterator2 _it2_end;
  bool has_end_iter;
  //    uint32_t count;
};

template <typename ContainerType1, typename ContainerType2> class zip {
public:
  using IteratorType = zipiterator<typename ContainerType1::iterator,
                                   typename ContainerType2::iterator>;
  zip(const ContainerType1 &c1, const ContainerType2 &c2) : _c1(c1), _c2(c2){};
  IteratorType begin() {
    //        return IteratorType(this->_c1.begin(),this->_c2.begin());
    return IteratorType(this->_c1.begin(), this->_c1.end(), this->_c2.begin(),
                        this->_c2.end());
  }

  IteratorType end() {
    return IteratorType(this->_c1.end(), this->_c1.end(), this->_c2.end(),
                        this->_c2.end());
  }

  ContainerType1 _c1;
  ContainerType2 _c2;
};

template <typename T> struct enumerate_iterator {
public:
  enumerate_iterator(uint64_t initial_count, typename T::iterator it)
      : count(initial_count), _it(it){};
  using value_type = std::tuple<uint64_t, typename T::value_type>;
  value_type operator*() { return value_type{this->count, *(this->_it)}; }
  enumerate_iterator &operator++() {
    this->count++;
    this->_it++;
    return *this;
  }
  bool operator==(enumerate_iterator &other) {
    return (this->_it == other._it);
  }
  bool operator!=(enumerate_iterator &other) {
    return !this->operator==(other);
  }
  uint64_t count;
  typename T::iterator _it;
};

template <typename T> struct enumerate {
public:
  using iterator = enumerate_iterator<T>;
  using value_type = typename enumerate_iterator<T>::value_type;
  enumerate(T &c) : _c(c){};
  iterator begin() { return iterator{this->count, this->_c.begin()}; }
  iterator end() {
    return iterator{
        // 99999999,
        this->_c.size(),
        this->_c.end()}; // 因为要做到编译期实现，所以这里只能先写死一个数
  }
  T &_c;
  uint64_t count = 0;
};

template <std::size_t... Is>
constexpr auto make_index_tuple(std::index_sequence<Is...>) {
  return std::make_tuple(Is...); // 生成包含索引值的元组
}

template <std::size_t N> auto vunpack(ContainerWithArithmeticElement auto &&c) {
  if constexpr (N <= 0) {
    throw std::runtime_error("vunpack: N must be greater than 0");
  }
  if (c.size() < N) {
    throw std::runtime_error("vunpack: container size less than N");
  }
  return [&c]<std::size_t... Is>(std::index_sequence<Is...>) {
    return std::make_tuple(c[Is]...);
  }
  (std::make_index_sequence<N>{});
  // return std::apply([](auto... args){return std::make_tuple(args...);},c);
  // auto indices = make_index_tuple(std::make_index_sequence<N>{});
  // return std::apply([&c](auto... Is) {
  //   return std::make_tuple(c[Is]...);
  // }, indices);
}

template <std::size_t N>
auto vunpack(NestedContainerWithArithmeticElement auto &&c) {
  if constexpr (N <= 0) {
    throw std::runtime_error("vunpack: N must be greater than 0");
  }
  if (c.size() < N) {
    throw std::runtime_error("vunpack: container size less than N");
  }
  return [&c]<std::size_t... Is>(std::index_sequence<Is...>) {
    return std::make_tuple(c[Is]...);
  }
  (std::make_index_sequence<N>{});
}

Arithmetic auto max(const Arithmetic auto &a, const Arithmetic auto &b) {
  return a > b ? a : b;
}

template <Arithmetic... Args>
Arithmetic auto max(const Arithmetic auto &a, const Args &...args) {
  return utils::max(a, utils::max(args...));
}

template <std::size_t N>
auto max(const ContainerWithArithmeticElement auto &a) {
  auto unpacks = vunpack<N>(a);
  return std::apply([](auto... args) { return utils::max(args...); }, unpacks);
}

Arithmetic auto min(const Arithmetic auto &a, const Arithmetic auto &b) {
  return a <= b ? a : b;
}

template <Arithmetic... Args>
Arithmetic auto min(const Arithmetic auto &a, const Args &...args) {
  return utils::min(a, utils::min(args...));
}

template <std::size_t N>
auto min(const ContainerWithArithmeticElement auto &a) {
  auto unpacks = vunpack<N>(a);
  return std::apply([](auto... args) { return utils::min(args...); }, unpacks);
}

template <ContainerWithArithmeticElement C, typename Predicate>
auto select(C &&c, Predicate &&pred) {
  using ValueType = typename std::decay_t<decltype(*c.begin())>;
  std::vector<ValueType> result;
  for (const auto &val : c) {
    if (pred(val)) {
      result.push_back(val);
    }
  }
  return result;
}

template <Arithmetic T> auto range(T start, T end, T step = 1) {
  std::vector<T> result;
  for (T i = start; i < end; i += step) {
    result.push_back(i);
  }
  return result;
}

template <ContainerWithArithmeticElement C>
auto slice(C &&c, size_t start, size_t end, size_t step = 1) {
  using ValueType = typename std::decay_t<decltype(*c.begin())>;
  std::vector<ValueType> result;
  for (size_t i = start; i < end && i < c.size(); i += step) {
    result.push_back(c[i]);
  }
  return result;
}

template <ContainerWithArithmeticElement C1, ContainerWithArithmeticElement C2>
bool equals(C1 &&c1, C2 &&c2) {
  if (c1.size() != c2.size())
    return false;
  for (auto &&[a, b] : zip(c1, c2)) {
    if (a != b)
      return false;
  }
  return true;
}

template <typename F, typename... Args> void timeit(F &&f, Args &&...args) {
  constexpr int FIXED_RUNS = 10;
  std::vector<std::chrono::microseconds> durations;
  durations.reserve(FIXED_RUNS);

  for (int i = 0; i < FIXED_RUNS; ++i) {
    auto start = std::chrono::high_resolution_clock::now();
    std::forward<F>(f)(std::forward<Args>(args)...);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    durations.emplace_back(duration);
    std::cout << "[Run " << (i + 1) << "] " << duration.count() << " μs\n";
  }

  long long total = 0;
  for (const auto &d : durations)
    total += d.count();
  double average = static_cast<double>(total) / FIXED_RUNS;
  std::cout << "\nAverage time: " << average << " μs\n";
}

/*
fold(f,x1,x2,x3,x4,x5...)=f(...f(f(f(f(x1,x2),x3),x4),x5)...)
*/
template <typename Func, typename First, typename Second>
auto fold(Func &&f, First &&first, Second &&second) {
  return std::forward<Func>(f)(std::forward<First>(first),
                               std::forward<Second>(second));
}

template <typename Func, typename First, typename Second, typename... Rest>
auto fold(Func &&f, First &&first, Second &&second, Rest &&...rest) {
  return fold(
      std::forward<Func>(f),
      // fold(std::forward<Func>(f),std::forward<First>(first),std::forward<Second>(second)),
      // // both ok
      std::forward<Func>(f)(std::forward<First>(first),
                            std::forward<Second>(second)),
      std::forward<Rest>(rest)...);
}

/*
nest(f,x0,n) = f(...f(f(f(f(x0))))...) // n nest time
*/
template <typename Func, typename Start>
auto nest(Func &&f, Start &&start, size_t depth) {
  if (depth == 0) {
    return std::forward<Func>(f)(std::forward<Start>(start));
  } else {
    return nest(std::forward<Func>(f),
                std::forward<Func>(f)(std::forward<Start>(start)), depth - 1);
  }
}

} // end namespace utils

#endif

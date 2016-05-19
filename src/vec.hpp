#ifndef VEC_HPP
#define VEC_HPP

#include "command.h"
#include <algorithm>
#include <array>
#include <boost/optional.hpp>
#include <cmath>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <numeric>

template <typename T, size_t dim>
class Vec {
 public:
  Vec() {}
  Vec(std::initializer_list<T> elems) {
    std::copy(elems.begin(), elems.end(), _elems.begin());
  }

  Vec(Command direction) {
    at(0) = 0;
    at(1) = 0;
    if (direction == Command::west || direction == Command::northWest ||
        direction == Command::southWest) {
      at(0) = -1;
    } else if (direction == Command::east || direction == Command::northEast ||
               direction == Command::southEast) {
      at(0) = 1;
    }

    if (direction == Command::north || direction == Command::northWest ||
        direction == Command::northEast) {
      at(1) = -1;
    } else if (direction == Command::south || direction == Command::southWest ||
               direction == Command::southEast) {
      at(1) = 1;
    }
  }

  T norm() const { return std::sqrt(*this * (*this)); }

  T& at(size_t pos) { return _elems.at(pos); }
  T const& at(size_t pos) const { return _elems.at(pos); }
  T& operator[](size_t pos) { return _elems[pos]; }
  T const& operator[](size_t pos) const { return _elems[pos]; }

  typename std::array<T, dim>::iterator begin() { return _elems.begin(); }
  typename std::array<T, dim>::const_iterator cbegin() const {
    return _elems.cbegin();
  }
  typename std::array<T, dim>::iterator end() { return _elems.end(); }
  typename std::array<T, dim>::const_iterator cend() const {
    return _elems.cend();
  }

  Vec<T, dim> normalize() const { return *this / this->norm(); }

  bool operator==(Vec<T, dim> const& rhs) const {
    return _elems == rhs._elems;
  }

  boost::optional<Command> asDirection() const {
    if (at(0) < 0 && at(1) < 0) {
      return Command::northWest;
    } else if (at(0) == 0 && at(1) < 0) {
      return Command::north;
    } else if (at(0) > 0 && at(1) < 0) {
      return Command::northEast;
    } else if (at(0) > 0 && at(1) == 0) {
      return Command::east;
    } else if (at(0) > 0 && at(1) > 0) {
      return Command::southEast;
    } else if (at(0) == 0 && at(1) > 0) {
      return Command::south;
    } else if (at(0) < 0 && at(1) > 0) {
      return Command::southWest;
    } else if (at(0) < 0 && at(1) == 0) {
      return Command::west;
    } else {
      return boost::none;
    }
  }

 private:
  std::array<T, dim> _elems;
};

using Position = Vec<int, 2>;

template <typename T, size_t dim>
Vec<T, dim>& operator+=(Vec<T, dim>& lhs, Vec<T, dim> const& rhs) {
  for (size_t i = 0; i < dim; i++)
    lhs.at(i) += rhs.at(i);

  return lhs;
}

template <typename T, size_t dim>
Vec<T, dim>& operator-=(Vec<T, dim>& lhs, Vec<T, dim> const& rhs) {
  for (size_t i = 0; i < dim; i++)
    lhs.at(i) -= rhs.at(i);

  return lhs;
}

template <typename T, size_t dim>
Vec<T, dim> operator+(Vec<T, dim> const& lhs, Vec<T, dim> const& rhs) {
  auto res = lhs;
  res += rhs;

  return res;
}

template <typename T, size_t dim>
Vec<T, dim> operator-(Vec<T, dim> const& lhs, Vec<T, dim> const& rhs) {
  auto res = lhs;
  res -= rhs;

  return res;
}

template <typename T, size_t dim>
Vec<T, dim> operator-(Vec<T, dim> const& v) {
  Vec<T, dim> res;
  std::transform(v.begin(), v.end(), res.begin(), [](T x) { return -x; });
  return res;
}

template <typename T, size_t dim>
Vec<T, dim>& operator*=(Vec<T, dim>& v, T const& a) {
  std::transform(v.begin(), v.end(), v.begin(), [a](T x) { return a * x; });
  return v;
}

template <typename T, size_t dim>
Vec<T, dim> operator*(Vec<T, dim> const& v, T const& a) {
  auto res = v;
  return res *= a;
}

template <typename T, size_t dim>
Vec<T, dim> operator*(T const& a, Vec<T, dim> const& v) {
  return v * a;
}

template <typename T, size_t dim>
T operator*(Vec<T, dim> const& v, Vec<T, dim> const& w) {
  return std::inner_product(v.cbegin(), v.cend(), w.cbegin(), 0);
}

template <typename T, size_t dim>
Vec<T, dim>& operator/=(Vec<T, dim>& v, T const& a) {
  std::transform(v.begin(), v.end(), v.begin(), [a](T x) { return x / a; });
  return v;
}

template <typename T, size_t dim>
Vec<T, dim> operator/(Vec<T, dim> const& v, T const& a) {
  auto res = v;
  return res /= a;
}

template <typename T, size_t dim>
std::ostream& operator<<(std::ostream& out, Vec<T, dim> const& v) {
  out << '(' << *v.cbegin();
  std::for_each(v.cbegin() + 1, v.cend(), [&out](T x) { out << ", " << x; });
  out << ')';

  return out;
}

#endif

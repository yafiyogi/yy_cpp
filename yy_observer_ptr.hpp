// Based on https://github.com/CppCon/CppCon2019/blob/master/Presentations/the_smart_pointers_i_wish_i_had/the_smart_pointers_i_wish_i_had__matthew_fleming__cppcon_2019.pdf
// page 17

#pragma once

namespace yafiyogi::yy_data {

template <typename T>
class observer_ptr
{
  public:
    using value_type = T;
    using pointer = std::add_pointer_t<T>;

    constexpr explicit observer_ptr(pointer p) noexcept:
      m_raw_ptr(p)
    {
    }

    constexpr explicit observer_ptr() noexcept = default;
    constexpr observer_ptr(const observer_ptr &) noexcept = default;
    constexpr observer_ptr(observer_ptr &&) noexcept = default;

    constexpr ~observer_ptr() noexcept
    {
      reset();
    }

    constexpr observer_ptr & operator=(const observer_ptr &) noexcept = default;
    constexpr observer_ptr & operator=(observer_ptr &&) noexcept = default;
    constexpr observer_ptr & operator=(const pointer other)
    {
      m_raw_ptr = other;

      return *this;
    }

    constexpr bool operator==(const observer_ptr & other) const noexcept
    {
      return other.m_raw_ptr == m_raw_ptr;
    }

    constexpr bool operator==(const pointer other) const noexcept
    {
      return other == m_raw_ptr;
    }

    constexpr bool operator<(const observer_ptr & other) const noexcept
    {
      return other.m_raw_ptr < m_raw_ptr;
    }

    constexpr bool operator<(const pointer other) const noexcept
    {
      return other == m_raw_ptr;
    }

    constexpr pointer release() noexcept
    {
      auto ptr = m_raw_ptr;

      reset();

      return ptr;
    }

    constexpr void reset() noexcept
    {
      m_raw_ptr = nullptr;
    }

    constexpr explicit operator bool() const noexcept
    {
      return nullptr == m_raw_ptr;
    }

    constexpr pointer get() const noexcept
    {
      return m_raw_ptr;
    }

    constexpr pointer operator->() const noexcept
    {
      return m_raw_ptr;
    }

    constexpr T & operator*() const noexcept
    {
      return *m_raw_ptr;
    }

    constexpr void swap(observer_ptr & other) noexcept
    {
      std::swap(m_raw_ptr, other.m_raw_ptr);
    }

  private:
    pointer m_raw_ptr = nullptr;
};

} // namespace yafiyogi::yy_data

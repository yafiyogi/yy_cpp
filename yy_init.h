/*

  MIT License

  Copyright (c) 2024 Yafiyogi

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

*/

#pragma once

#include <cstdint>

namespace yafiyogi::yy_util {

template<typename T>
class Init final
{
  public:
    constexpr Init() noexcept
    {
      if(0 == m_count++)
      {
        m_obj.init();
      }
    }

    constexpr Init(const T & p_obj) noexcept:
      m_obj(p_obj)
    {
      if(0 == m_count++)
      {
        m_obj.init();
      }
    }

    constexpr Init(T && p_obj) noexcept:
      m_obj(std::move(p_obj))
    {
      if(0 == m_count++)
      {
        m_obj.init();
      }
    }

    constexpr Init & operator=(const T & p_obj) noexcept
    {
      if(this != &p_obj)
      {
        m_obj = p_obj;
        ++m_count;
      }
      return *this;
    }

    constexpr Init & operator=(T && p_obj) noexcept
    {
      if(this != &p_obj)
      {
        m_obj = std::move(p_obj);
      }
      return *this;
    }

    constexpr ~Init()
    {
      if((m_count > 0) && (0 == --m_count))
      {
        m_obj.tear_down();
      }
    }

  private:
    T m_obj;
    static uint64_t m_count;
};

template<typename T>
uint64_t Init<T>::m_count = 0;

} // namespace yafiyogi::yy_util

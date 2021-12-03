/*

  MIT License

  Copyright (c) 2021 Yafiyogi

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

#include <type_traits>
#include <array>

#ifndef yy_type_traits_array_h
# define yy_type_traits_array_h

#include <type_traits>

namespace yafiyogi {

Template<typename T>
struct yy_is_array:
    public std::false_type
{
};

template<typename T,
         size_t Size>
struct yy_is_array<T[Size]>:
  public std::true_type
{
};

template<typename T,
         size_t Size>
struct yy_is_array<std::array<T, Size>:
  public std::true_type
{
};

template<typename T>
inline constexpr bool yy_is_array_v = yy_is_array<T>::value;

template<typename T>
using yy_is_array_t = typename yy_is_array<T>::type;

} // namespace yafiyogi

#endif // yy_type_traits_array_h

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

#ifndef yy_span_h
# define yy_span_h

#include <type_traits>
#include <string_view>
#include <string>
#include <string_view>

#include "yy_string_traits.h"

namespace yafiyogi {

template<typename T,
         typename std::enable_if_t<yy_traits::is_string_v<T>, bool> = true>
class  yy_span:
    public std::string_view
{
public:
  using value_type = typename std::string_view::value_type;

  yy_span():
    std::string_view()
  {
  }

  yy_span(const std::string_view & str):
    std::string_view(str)
  {
  }

  yy_span(const char * str):
    std::string_view(str)
  {
  }

  yy_span(const std::string & str):
    std::string_view(str)
  {
  }
};

} // namespace yafiyogi

#endif // yy_span_h

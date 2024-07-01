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

#include <cstdlib>
#include <locale>
#include <locale.h>
#include <mutex>
#include <string>

#include "boost/locale/generator.hpp"
#include "spdlog/spdlog.h"

namespace yafiyogi::yy_locale {

namespace {

std::once_flag g_locale_set_flag;
std::string g_locale_name;
std::locale g_locale;

} // anonymous namespace

void set_locale()
{
  std::call_once(g_locale_set_flag, []()
  {
    auto locale_name = std::getenv("LC_ALL");
    if(nullptr != locale_name)
    {
      g_locale_name = locale_name;
    }
    else
    {
      g_locale_name = g_locale.name();
    }

    if(auto l_locale =  std::setlocale(LC_ALL, g_locale_name.c_str());
       nullptr == l_locale)
    {
      spdlog::warn("Can't set locale to [{}].", g_locale_name);

      g_locale_name = "C";

      assert(std::setlocale(LC_ALL, g_locale_name.c_str()));
    }

    spdlog::info("Setting locale to [{}].", g_locale_name);
    boost::locale::generator gen{};
    g_locale = gen(g_locale_name);
  });
}

void set_locale(const std::string_view locale)
{
  std::call_once(g_locale_set_flag, [locale]()
  {
    g_locale_name = locale;
    if(auto l_locale =  std::setlocale(LC_ALL, g_locale_name.c_str());
       nullptr == l_locale)
    {
      spdlog::warn("Can't set locale to [{}].", g_locale_name);

      g_locale_name = "C";

      assert(std::setlocale(LC_ALL, g_locale_name.c_str()));
    }

    spdlog::info("Setting locale to [{}].", g_locale_name);
    boost::locale::generator gen{};
    g_locale = gen(g_locale_name);
  });
}

const std::locale & get_locale()
{
  set_locale();
  return g_locale;
}

const std::string & get_locale_name()
{
  set_locale();
  return g_locale_name;
}

}

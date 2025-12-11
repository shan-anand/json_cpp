/**
 * @file convert.cpp
 * @brief Generic conversion functions defined in convert.hpp
 */

/*
LICENSE: BEGIN
===============================================================================
@author Shan Anand
@email anand.gs@gmail.com
@source https://github.com/shan-anand
@brief List of common functions in C++
===============================================================================
MIT License

Copyright (c) 2017 Shanmuga (Anand) Gunasekaran

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
===============================================================================
LICENSE: END
*/

#include "convert.h"
#include <iostream>

// C++ includes
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <utility>
#include <cmath>
#include <limits>

// curses and terminal IO includes
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <termios.h>
#include <curses.h>

using namespace std;

#define WHITESPACE " \t\r\n"

template <>
long double json::to_num<long double>(const char* _nptr)
{
  long double res = 0;
  try
  {
    if ( !_nptr ) throw EINVAL;
    while ( *_nptr == ' ' ) _nptr++;
    if ( *_nptr == '\0' ) throw EINVAL;
    char* endptr = nullptr;
    errno = 0;
    res = ::strtold(_nptr, &endptr);
    if ( errno != 0 )
      throw errno;
    if ( res == 0.0 && endptr == _nptr )
      throw EINVAL;
    /*
    if ( res == HUGE_VALF || res == HUGE_VALL )
    {
      if ( errno == ERANGE )
	      throw errno;
    }
    */
  }
  catch (const std::exception&) { throw; }
  catch (int iError)
  {
    errno = iError;
    if ( iError == EINVAL )
      throw std::invalid_argument(json::to_errno_str(iError, std::string("Failed to convert [") + ((_nptr)? _nptr:"NULL") + "] to long double value"));
    throw std::runtime_error(json::to_errno_str(iError, std::string("Failed to convert [") + ((_nptr)? _nptr:"NULL") + "] to long double value"));
  }
  catch (...)
  {
    throw std::runtime_error(std::string("An unhandled exception occurred while converting to long double: ") + ((_nptr)? _nptr:"NULL"));
  }
  return res;
}

template <>
long double json::to_num<long double>(const std::string& _csVal)
{
  return to_num<long double>(_csVal.c_str());
}

template <>
bool json::to_num<long double>(const char* _nptr, /*out*/ long double& _outVal, /*out*/ std::string* _pcsError/* = nullptr*/)
{
  try { _outVal = to_num<long double>(_nptr); return true; }
  catch (const std::exception& e) { if ( _pcsError ) *_pcsError = e.what(); }
  return false;
}

template <>
bool json::to_num<long double>(const std::string& _csVal, /*out*/ long double& _outVal, /*out*/ std::string* _pcsError/* = nullptr*/)
{
  return to_num<long double>(_csVal.c_str(), _outVal, _pcsError);
}

std::string json::to_str(const long double& _number)
{
  std::ostringstream out;
  out << std::setprecision(std::numeric_limits<long double>::digits10) << _number;
  return out.str();
}

int json::is_binary(int c) { return (c == '0' || c == '1')? 1 : 0; }

int json::is_octal(int c) { return (c >= '0' && c <= '7')? 1 : 0; }

int json::is_decimal(int c) { return (c >= '0' && c <= '9')? 1 : 0; }

int json::is_hex(int c) { return isxdigit(c); }

std::string json::trim(const std::string& _input)
{
  std::string csOutput;

  std::string::size_type iPos = _input.find_first_not_of(WHITESPACE, 0);

  if ( iPos != std::string::npos )
  {
    int iStartPos = iPos;
    iPos = _input.find_last_not_of(WHITESPACE, string::npos);
    if ( iPos == std::string::npos )
      csOutput = _input.substr(iStartPos, _input.length() - iStartPos);
    else
      csOutput = _input.substr(iStartPos, iPos - iStartPos + 1);
  }

  return csOutput;
}

std::string json::get_sep(uint64_t _number)
{
  std::string out = std::to_string(_number);
  for ( int i = out.length()-3; i > 0; i -= 3 )
    out.insert(i, ",");
  return out;
}

std::string json::to_lower(const std::string& _input)
{
  std::string csOutput(_input);
  for ( char& ch : csOutput )
    ch = (char) ::tolower(ch);
  return csOutput;
}

std::string json::to_upper(const std::string& _input)
{
  std::string csOutput(_input);
  for ( char& ch : csOutput )
    ch = (char) ::toupper(ch);
  return csOutput;
}

std::string json::to_errno_str(const std::string& _prefix/* = std::string()*/)
{
  return json::to_errno_str(errno, _prefix);
}

std::string json::to_errno_str(int _errno, const std::string& _prefix/* = std::string()*/)
{
  std::ostringstream out;
  char buff[512] = {0};
  if ( ! _prefix.empty() )
    out << _prefix << ", ";
  out << "(" << _errno << ") " << ::strerror_r(_errno, buff, sizeof(buff)-1);
  return out.str();
}

//////////////////////////////////////////////////////////////////////////////////
//
// Boolean conversion functions
//
std::string json::to_str(const bool& _bVal, const match_case _matchCase /*= match_case::exact*/)
{
  std::string val = (_bVal)? "true":"false";
  switch ( _matchCase )
  {
  case match_case::exact:
  case match_case::any:
  case match_case::lower:
    // The default of all the above cases is lower-case
    break;
  case match_case::upper:
    val = json::to_upper(val);
    break;
  case match_case::camel:
    // Convert the entry to camel case by changing the first character to uppercase
    val[0] = (char) toupper(val[0]);
    break;
  }
  return val;
}

bool json::to_bool(const std::string& _input, const match_case _matchCase)
{
  const char* valid_values[5] = {"true", "false", nullptr};
  for ( size_t i = 0; valid_values[i] != nullptr; i++ )
  {
    bool found = true;
    const char* entry = valid_values[i];
    switch ( _matchCase )
    {
    case match_case::exact:
      found = ( _input == entry );
      break;
    case match_case::any:
      found = ( ::strcasecmp(_input.c_str(), entry) == 0 );
      break;
    case match_case::lower:
      found = ( _input == json::to_lower(entry) );
      break;
    case match_case::upper:
      found = ( _input == json::to_upper(entry) );
      break;
    case match_case::camel:
      // Convert the entry to camel case
      found = ( ::toupper(entry[0]) == _input[0] && ::strcmp(&entry[1], _input.c_str()+1) == 0 );
      break;
    }
    if ( found ) return (i == 0);
  }
  throw std::invalid_argument(std::string("Invalid value \"") + _input + "\". Must be true or false");
}

bool json::to_bool(const std::string& _input, const bool& _defValueOnError) noexcept
{
  return json::to_bool(_input, match_case::exact, _defValueOnError);
}

bool json::to_bool(const std::string& _input, const match_case _matchCase, const bool& _defValueOnError) noexcept
{
  bool outVal = _defValueOnError;
  json::to_bool(_input, _matchCase, outVal);
  return outVal;
}

bool json::to_bool(const std::string& _input, /*out*/ bool& _outVal, /*out*/ std::string* _pcsError /*= nullptr*/) noexcept
{
  return json::to_bool(_input, match_case::exact, _outVal, _pcsError);
}

bool json::to_bool(const std::string& _input, const match_case _matchCase, /*out*/ bool& _outVal, /*out*/ std::string* _pcsError /*= nullptr*/) noexcept
{
  try { _outVal = json::to_bool(_input, _matchCase); }
  catch (const std::exception& e) { if ( _pcsError ) *_pcsError = e.what(); return false; }
  return true;
}

//////////////////////////////////////////////////////////////
void _internal_split_add_string(std::vector<std::string>& out, const std::string& val) { out.push_back(val); }
void _internal_split_add_string(std::set<std::string>& out, const std::string& val) { out.insert(val); }

/**
 * @fn size_t split(T& out, const std::string& input, const char sep, int splitFlag);
 * @brief Splits the given string into tokens and stores it in one of the following containers
 *        (1) std::vector<std::string>
 *        (2) std::set<std::string>
 *
 * @param [OUT] out : One of the above containers
 * @param [IN] input : Input string that needs to be split
 * @param [IN] sep : Character separator
 * @param [IN] splitFlag : Split control flags. See SPLIT_xxx
 *
 * @return The number of values added to the container
 *       
 */
template <class T> size_t _internal_split(T& out, const std::string& input, const char sep, int splitFlag)
{
  size_t count = 0;
  if ( !input.empty() )
  {
    std::string temp;
    size_t p2, p1 = 0;
    do
    {
      p2 = input.find(sep, p1);
      if ( p2 == std::string::npos )
        temp = input.substr(p1);
      else
      {
        temp = input.substr(p1, p2-p1);
        p1 = p2+1;
      }
      if ( splitFlag & SPLIT_TRIM ) json::trim(temp);
      if ( ! ((splitFlag & SPLIT_SKIP_EMPTY) && temp.empty()) )
      {
        _internal_split_add_string(out, temp);
        ++count;
      }
    }
    while ( p2 != std::string::npos );
  }
  return count;
}

size_t json::split(std::vector<std::string>& _out, const std::string& _input, const char _sep, int _splitFlag)
{
  return _internal_split(_out, _input, _sep, _splitFlag);
}

size_t json::split(std::set<std::string>& _out, const std::string& _input, const char _sep, int _splitFlag)
{
  return _internal_split(_out, _input, _sep, _splitFlag);
}

/**
 * @brief split the string to an array of strings using the given separator
 */
int json::split(std::vector<std::string>& _result, const string& _input, const char& _sep, string::size_type _start)
{
  std::string::size_type pos, newPos;
  for ( pos = _start; (newPos = _input.find(_sep, pos)) != std::string::npos ; pos = newPos+1 )
  {
    _result.push_back(_input.substr(pos, newPos-pos));
  }
  _result.push_back(_input.substr(pos));
  return _result.size();
}

template <typename T> std::string _internal_join(const T& _input, const char& _sep, int _joinFlag)
{
  std::ostringstream out;
  const char sep[3] {_sep, (_sep != ' ' && (_joinFlag & JOIN_WITH_SPACE))? ' ' : '\0', '\0'};
  bool firstTime = true;
  for ( const auto& entry : _input )
  {
    if ( ! entry.empty()  || !(_joinFlag & JOIN_SKIP_EMPTY) )
    {
      if ( firstTime )
	firstTime = false;
      else
	out << sep;
      out << entry;
    }
  }
  return out.str();
}

std::string json::join(const std::vector<std::string>& _input, const char& _sep/* = ','*/, int _joinFlag/* = 0*/)
{
  return _internal_join(_input, _sep, _joinFlag);
}

std::string json::join(const std::set<std::string>& _input, const char& _sep/* = ','*/, int _joinFlag/* = 0*/)
{
  return _internal_join(_input, _sep, _joinFlag);
}

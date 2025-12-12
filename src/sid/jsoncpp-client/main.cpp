/*
LICENSE: BEGIN
===============================================================================
@author Shan Anand
@email anand.gs@gmail.com
@source https://github.com/shan-anand
@brief Json handling using c++
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

#include "jsoncpp/json.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <stdexcept>

using namespace std;
using namespace sid;
using namespace sid::json;

namespace sid::json::local
{
  std::string get_file_contents(const std::string& _filePath);
}

int main(int argc, char* argv[])
{
  int retVal = 0;
  try
  {
    if ( argc < 2 )
      throw std::invalid_argument("Need filename as an argument");

    json::parser_input  in;
    std::string param, key, value;
    std::optional<json::format> outputFmt;
    bool showOutput = false;
    bool useMmap = true;
    for ( int i = 2; i < argc; i++ )
    {
      param = argv[i];
      if ( param[0] != '-' )
        throw std::invalid_argument("Invalid parameter at position " + std::to_string(i));
      size_t pos = param.find('=');
      if ( pos == std::string::npos )
      {
        key = param;
        value.clear();
      }
      else
      {
        key = param.substr(0, pos);
        value = param.substr(pos+1);
      }
      if ( key == "--dup" || key == "--duplicate" )
      {
        if ( value == "accept" )
          in.ctrl.dupKey = json::parser_control::dup_key::accept;
        else if ( value == "ignore" )
          in.ctrl.dupKey = json::parser_control::dup_key::ignore;
        else if ( value == "append" )
          in.ctrl.dupKey = json::parser_control::dup_key::append;
        else if ( value == "reject" )
          in.ctrl.dupKey = json::parser_control::dup_key::reject;
        else if ( ! value.empty() )
          throw std::invalid_argument(key + " can only be accept|ignore|append|reject");
      }
      else if ( key == "--allow-flex-keys" || key == "--allow-flexible-keys" )
        in.ctrl.mode.allowFlexibleKeys = 1;
      else if ( key == "--allow-flex-strings" || key == "--allow-flexible-strings" )
        in.ctrl.mode.allowFlexibleStrings = 1;
      else if ( key == "--allow-nocase" || key == "--allow-nocase-values" )
        in.ctrl.mode.allowNocaseValues = 1;
      else if ( key == "--show-output" )
      {
        showOutput = true;
        if ( ! value.empty() )
        {
          if ( value == "false" || value == "no" )
            showOutput = false;
          else
            outputFmt = json::format::get(value);
        }
      }
      else if ( key == "--use" )
      {
        if ( value == "mmap" )
          useMmap = true;
        else if ( value == "data" || value == "string" ) 
          useMmap = false;
        else
          throw std::invalid_argument(key + " values can only be mmap|data|string");
      }
      else
        throw std::invalid_argument("Invalid key: " + key);
    } // for loop
 
    // Complete preparing the input object by filling the input type and input
    if ( useMmap )
    {
      cerr << "Using mmap for parsing...." << endl;
      in.set(json::input_type::file_path, argv[1]);
    }
    else
    {
      std::string jsonStr = local::get_file_contents(argv[1]);
      cerr << "Using string data for parsing...." << endl;
      in.set(json::input_type::data, jsonStr);
    }

    parser_output out;
    json::value::parse(in, out);
    if ( showOutput )
    {
      cout << (outputFmt.has_value()? out.jroot.to_str(outputFmt.value()) : out.jroot.to_str()) << endl;
    }
    cerr << out.stats.to_str() << endl;
    retVal = 0;
  }
  catch(const std::exception& e)
  {
    cerr << e.what() << endl;
    retVal = -1;
  }
  
  return retVal;
}

std::string local::get_file_contents(const std::string& _filePath)
{
  std::ifstream in;
  in.open(_filePath);
  if ( ! in.is_open() )
    throw std::system_error(errno, std::system_category(), "Failed to open file: " + _filePath);
  char buf[8096] = {0};
  std::string jsonStr;
  while ( ! in.eof() )
  {
    std::memset(buf, 0, sizeof(buf));
    in.read(buf, sizeof(buf)-1);
    if ( in.bad() )
      throw std::system_error(errno, std::system_category(), "Failed to read data");
    jsonStr += buf;
  }
  return jsonStr;
}

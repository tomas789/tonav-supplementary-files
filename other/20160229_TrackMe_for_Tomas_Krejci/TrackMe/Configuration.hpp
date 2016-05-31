#ifndef CONFIGURATION_HPP__
#define CONFIGURATION_HPP__

#include <string>
#include <fstream>
#include <iostream>
#include <cctype>
#include <clocale>
#include <algorithm>
#include <unordered_map>
#include <vector>

/**
 * @brief Simple configuration class. Supports reading from file and parsing
 *        command line arguments.
 */
template <typename String = std::string>
class Configuration
{
  public:
    // SET
    void loadFile(const std::string &fileName);
    void parseArgs(int argc, char *argv[]);
    void parseString(const String &string);
    void set(const String &param, const String &value);

    // READ
    unsigned long ul(const String &param, unsigned long def = 0) const;
    double d(const String &param, double def = 0) const;
    String s(const String &param, const String &def = String{}) const;

    // CONFIG
    void setDelim(const String &delim);
    void setComment(const String &comment);

  private:
    String trim(String str);
    std::unordered_map<String, String> params_;
    String delim_{"="};
    String comment_{"#"};
};

/**
 * @brief Tries to open a file and read the configuration from it.
 *        If the file cannot be opened, it does nothing.
 *        Lines beginning with # are ignored.
 */
template <typename String>
void Configuration<String>::loadFile(const std::string &fileName)
{
  std::basic_ifstream<typename String::value_type> file(fileName);
  for (String line; std::getline(file, line); parseString(line));
}

/**
 * @brief Stores all configuration parameters from command line arguments.
 */
template <typename String>
void Configuration<String>::parseArgs(int argc, char *argv[])
{
  std::vector<String> args(argv, argv+argc);
  for (auto &arg : args)
    parseString(arg);
}

/**
 * @brief Stores a configuration parameter from a string.
 */
template <typename String>
void Configuration<String>::parseString(const String &str)
{
  if (str.substr(std::min(str.size(), comment_.size())) == comment_)
    return;

  String param, value;
  auto pos = str.find(delim_);
  if (pos == String::npos)
    return;
  param = trim(str.substr(0, pos));
  value = trim(str.substr(pos + delim_.length()));
  set(param, value);
}

/**
 * @brief Assigns a value to a configuration parameter
 */
template <typename String>
void Configuration<String>::set(const String &param, const String &value)
{
  params_[param] = value;
}

/**
 * @brief Reads an unsigned long configuration parameter using std::stoul
 *        with "C" locale.
 */
template <typename String>
unsigned long
  Configuration<String>::ul(const String &param, unsigned long def) const
{
  auto iter = params_.find(param);
  if (iter == params_.end()) {
#ifdef DEBUG
    std::cerr << "Using default unsigned long value " << def
              << " for " << param << std::endl;
#endif
    return def;
  }

  auto lc_all = std::setlocale(LC_ALL, NULL);
  std::setlocale(LC_ALL, "C");
  return std::stoul(iter->second);
  std::setlocale(LC_ALL, lc_all);
}

/**
 * @brief Reads a double configuration parameter using std::stod with
 *        "C" locale.
 */
template <typename String>
double Configuration<String>::d(const String &param, double def) const
{

  auto iter = params_.find(param);
  if (iter == params_.end()) {
#ifdef DEBUG
    std::cerr << "Using default double value " << def
              << " for " << param << std::endl;
#endif
    return def;
  }

  auto lc_all = std::setlocale(LC_ALL, NULL);
  std::setlocale(LC_ALL, "C");
  return std::stod(iter->second);
  std::setlocale(LC_ALL, lc_all);
}

/**
 * @brief Reads a string configuration parameter.
 */
template <typename String>
String Configuration<String>::s(const String &param, const String &def) const
{
  auto iter = params_.find(param);
  if (iter == params_.end()) {
#ifdef DEBUG
    std::cerr << "Using default string value " << def
              << " for " << param << std::endl;
#endif
    return def;
  }
  return iter->second;
}

/**
 * @brief Sets the delimiter between a name and a value of parsed strings.
 */
template <typename String>
void Configuration<String>::setDelim(const String &delim)
{
  delim_ = delim;
}

/**
 * @brief Sets a comment string. Strings beginning with the comment string
 *        are ignored.
 */
template <typename String>
void Configuration<String>::setComment(const String &comment)
{
  comment_ = comment;
}

/**
 * @brief Trims a string (removes trailing spaces).
 */
template <typename String>
String Configuration<String>::trim(String str)
{
  auto notspace = [](int c){ return !std::isspace(c); };
  str.erase(str.begin(), std::find_if(str.begin(), str.end(), notspace));
  str.erase(std::find_if(str.rbegin(), str.rend(), notspace).base(), str.end());
  return str;
}


#endif

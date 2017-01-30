#pragma once
#include <exception>
#include <string>

class Error : public std::exception
{
  private:
    std::string msg;

  public:
    Error(const char *msg) : msg(msg) {}
    Error(const std::string &msg) : msg(msg) {}
    Error(std::string &&msg) { this->msg = std::move(msg); }
    const char *what() const noexcept { return msg.c_str(); }
};

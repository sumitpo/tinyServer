#ifndef HTTP_BASE_H
#define HTTP_BASE_H

#include <map>
#include <string>

#include "http_code.h"

enum Method {
  GET,
  HEAD,
  POST,
  PUT,
  TRACE,
  OPTIONS,
  DELETE,
};

const std::map<Method, std::string> method2str = {
    {GET, "GET"},     {HEAD, "HEAD"},       {POST, "POST"},     {PUT, "PUT"},
    {TRACE, "TRACE"}, {OPTIONS, "OPTIONS"}, {DELETE, "DELETE"},
};

class HttpRespBase {
private:
  int _majorVersion;
  int _minorVersion;
  int _code;
  std::string _msg;
  std::map<std::string, std::string> _header;
  std::string _data;

public:
  virtual void decode(const std::string &data);
  virtual std::string encode() {
    std::string resp = "HTTP/" + std::to_string(_majorVersion) + "." +
                       std::to_string(_minorVersion) + " " +
                       std::to_string(_code) + _msg + "\n";
    for (auto const &x : _header)
      resp += x.first + ": " + x.second + "\n";
    resp += "\n" + _data;
    return resp;
  }
  virtual void set_version(int major, int minor) {
    _majorVersion = major;
    _minorVersion = minor;
  }
  virtual void set_code(int code) {
    auto it = http_code2msg.find(code);
    if (it == http_code2msg.end()) {
      _code = 500;
      _msg = http_code2msg.find(_code)->second;
    } else {
      _code = it->first;
      _msg = it->second;
    }
  }
};

class HttpReqBase {
private:
  Method _method;
  std::string _url;
  int _majorVersion;
  int _minorVersion;
  std::map<std::string, std::string> _header;

public:
  void default_ret(HttpRespBase &resp) {
    // return 501 default
    resp.set_version(_majorVersion, _minorVersion);
    resp.set_code(501);
  }
  virtual void decode(const std::string &data);
  virtual std::string encode() {
    std::string req = method2str.find(_method)->second + " " + _url + " HTTP/" +
                      std::to_string(_majorVersion) + "." +
                      std::to_string(_minorVersion) + "\n";
    for (auto const &x : _header)
      req += x.first + ": " + x.second + "\n";
    req += "\n";
    return req;
  }
  virtual void get(HttpRespBase &resp) { default_ret(resp); }
  virtual void head(HttpRespBase &resp) { default_ret(resp); }
  virtual void post(HttpRespBase &resp) { default_ret(resp); }
  virtual void put(HttpRespBase &resp) { default_ret(resp); }
  virtual void trace(HttpRespBase &resp) { default_ret(resp); }
  virtual void options(HttpRespBase &resp) { default_ret(resp); }
  virtual void del(HttpRespBase &resp) { default_ret(resp); }
};

#endif

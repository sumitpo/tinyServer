#ifndef HTTP_0_9_H
#define HTTP_0_9_H

#include "http_base.h"
class HttpResp0_9 : public HttpRespBase {
  private:
    virtual void get() override ;
  public:
};
class HttpReq0_9 : public HttpReqBase {
  private:
  public:
};

#endif

#ifndef BACKEND_HPP
#define BACKEND_HPP

#include <nlohmann/json.hpp>
#include <vector>
#include <map>
#include <condition_variable>
#include <thread>

#include <unistd.h>
#include "HAEntity.hpp"
#include "WSConn.hpp"

using std::string;

class Backend
{
};

class HABackend : Backend
{
public:
  HABackend();
  bool Connect(string url, string token);
  bool Start();
  string CreateLongToken(string name);
  std::shared_ptr<HAEntity> GetEntityByName(const std::string& name);
  map<string, std::shared_ptr<HAEntity>> GetEntities();
  void WSConnSend(json& msg);

private:
  bool loaded;
  std::mutex load_lock;
  std::condition_variable load_cv;
  WSConn* wc = nullptr;
  std::thread ha;
  void threadrunner();

  map<string, std::shared_ptr<HAEntity>> states; // FIXME: should this be called entities?
  std::mutex stateslock;
  map<string, std::shared_ptr<HADomain>> domains;
  std::mutex domainslock;
};

#endif
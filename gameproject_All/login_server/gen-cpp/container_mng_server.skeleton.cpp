// This autogenerated skeleton file illustrates how to build a server.
// You should copy it to another filename to avoid overwriting it.

#include "container_mng.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

class container_mngHandler : virtual public container_mngIf {
 public:
  container_mngHandler() {
    // Your initialization goes here
  }

  int32_t create_room(const int32_t no) {
    // Your implementation goes here
 	int32_t ret = 0;
	std::string cmd = "./create_room.sh room_no" + std::to_string(no);
	auto pf = popen(cmd.c_str(), "r");
	if(pf != NULL)
	{
		char buff[1024];
		fread(buff, 1, sizeof(buff), pf);
		ret = atoi(buff);
		pclose(pf);
	}
	return ret;
  }

};

int main(int argc, char **argv) {
  int port = 9090;
  ::apache::thrift::stdcxx::shared_ptr<container_mngHandler> handler(new container_mngHandler());
  ::apache::thrift::stdcxx::shared_ptr<TProcessor> processor(new container_mngProcessor(handler));
  ::apache::thrift::stdcxx::shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  ::apache::thrift::stdcxx::shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  ::apache::thrift::stdcxx::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
  server.serve();
  return 0;
}

#pragma once

#include <string>

#include <boost\asio.hpp>
#include <boost\thread\thread.hpp>
#include <boost\bind.hpp>

using namespace std;
//using namespace boost;
//using namespace boost::asio;
//using namespace boost::asio::ip;


class ClientMain {
private:
	static ClientMain* instance;

protected:
	ClientMain();

public:
	static ClientMain* getInstance();
	bool PasswordCheck(string user, string pass);
};
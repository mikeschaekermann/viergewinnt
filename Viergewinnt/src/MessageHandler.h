#pragma once

#include "cinder/app/App.h"
#include <string>

using namespace ci::app;

class MessageHandler
{
public:
	virtual void		handle(std::string message, boost::asio::ip::udp::endpoint from) = 0;
};


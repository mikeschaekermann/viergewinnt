#pragma once

#include "cinder/app/App.h"
#include "boost/array.hpp"
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/json_parser.hpp"
#include "boost/asio.hpp"
#include "boost/thread.hpp"
#include "MessageHandler.h"
#include <set>

using namespace std;
using namespace ci::app;
using boost::property_tree::ptree;
using namespace boost::asio;

#define LISTENING_PORT 32442
#define MAX_MESSAGE_LENGTH_IN_BYTES 5000
#define TERMINATION_MESSAGE "__________TERMINATE__________"

class NetworkManager
{
public:
	class Message
	{
	public:
		Message(NetworkManager & networkManager);

		Message &						add(string const & key, string const & value);
		Message &						add(string const & key, int value);
		
		Message &						makeJSON();

		void							unicast();
		void							broadcast();
	private:
		std::string						message;
		boost::property_tree::ptree		propertyTree;
		NetworkManager &				networkManager;
	};

	NetworkManager(void);
	~NetworkManager(void);

	Message &				createMessage();

	void					unicast(string const & message);
	void					broadcast(string const & message);
	void					send(string const & message, const ip::udp::endpoint & endpoint);
	
	void					listen();
	void					numb();

	void					subscribe(MessageHandler & handler);
	void					unsubscribe(MessageHandler & handler);

private:
	/// threaded method listening for incoming messages and passing them to the handlers registered
	void					listenThreadFunction();
	
	boost::thread			listenThread;
	io_service				ioService;
	ip::udp::socket			socket;

	ip::udp::endpoint		loopbackEndpoint;
	ip::udp::endpoint		unicastEndpoint;
	ip::udp::endpoint		broadcastEndpoint;
	
	/// list of message handlers that are called upon message reception
	std::set
	<
		MessageHandler *
	>						handlers;
};


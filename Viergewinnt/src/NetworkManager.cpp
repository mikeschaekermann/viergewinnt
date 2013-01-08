#include "NetworkManager.h"

/************************************************************************/
/* NetworkManager::Message                                              */
/************************************************************************/

NetworkManager::Message::Message(NetworkManager & networkManager) :
	networkManager(networkManager),
	message("undefined message")
{
}

NetworkManager::Message & NetworkManager::Message::add(string const & key, string const & value)
{
	propertyTree.add(key, value);

	return *this;
}

NetworkManager::Message & NetworkManager::Message::add(string const & key, int value)
{
	propertyTree.add(key, value);

	return *this;
}

NetworkManager::Message & NetworkManager::Message::makeJSON()
{
	std::stringstream messageStream;
	boost::property_tree::json_parser::write_json(messageStream, propertyTree);

	message = messageStream.str();

	return *this;
}

void NetworkManager::Message::unicast()
{
	networkManager.unicast(message);

	delete this;
}

void NetworkManager::Message::broadcast()
{
	networkManager.broadcast(message);

	delete this;
}

void NetworkManager::Message::send(ip::udp::endpoint & endpoint)
{
	networkManager.send(message, endpoint);

	delete this;
}

/************************************************************************/
/* NETWORK MANAGER                                                      */
/************************************************************************/

NetworkManager::NetworkManager(void) :
	ioService(),
	loopbackEndpoint(ip::address_v4::loopback(), LISTENING_PORT),
	unicastEndpoint(ip::address_v4::loopback(), LISTENING_PORT),
	broadcastEndpoint(ip::address_v4::broadcast(), LISTENING_PORT),
	socket(ioService, ip::udp::endpoint(ip::udp::v4(), LISTENING_PORT))
{
	socket.set_option(socket_base::broadcast(true));

	boost::asio::ip::udp::resolver resolver(ioService);
	boost::asio::ip::udp::resolver::query query(boost::asio::ip::host_name(), "");
	boost::asio::ip::udp::resolver::iterator end;
	boost::system::error_code error = boost::asio::error::host_not_found;

	for (auto it = resolver.resolve(query); error && it != end; ++it)
	{
		localEndpoints.push_back((*it).endpoint());
	}
}


NetworkManager::~NetworkManager(void)
{
	numb();
}

NetworkManager::Message & NetworkManager::createMessage()
{
	auto message = new Message(*this);
	message->add("version", VERSION_NUMBER);
	message->add("clienttype", CLIENT_TYPE);
	
	return (*message);
}

void NetworkManager::setUnicastEndpoint(const ip::udp::endpoint & endpoint)
{
	unicastEndpoint = endpoint;
}

void NetworkManager::unicast(string const & message)
{
	send(message, unicastEndpoint);
}

void NetworkManager::broadcast(string const & message)
{
	send(message, broadcastEndpoint);
}

void NetworkManager::send(string const & message, const ip::udp::endpoint & endpoint)
{
	boost::system::error_code error;

	socket.send_to(buffer(message.c_str(), message.size()), endpoint, 0, error);

	if (error && error != boost::asio::error::message_size)
	{
		console() << error.message() << std::endl;
	}
}

void NetworkManager::listen()
{
	listenThread = boost::thread(boost::bind(&NetworkManager::listenThreadFunction, this));
}

void NetworkManager::numb()
{
	listenThread.interrupt();
	send(TERMINATION_MESSAGE, loopbackEndpoint);
	listenThread.join();
}

void NetworkManager::listenThreadFunction()
{
	while (true)
	{
		boost::array<char, MAX_MESSAGE_LENGTH_IN_BYTES> buffer;

		for (unsigned int i = 0; i < MAX_MESSAGE_LENGTH_IN_BYTES; ++i)
		{
			buffer[i] = 0;
		}

		ip::udp::endpoint remote_endpoint;
		boost::system::error_code error;

		/// wait for new message
		socket.receive_from(boost::asio::buffer(buffer), remote_endpoint, 0, error);
		
		if (error && error != boost::asio::error::message_size)
		{
			console() << error.message() << std::endl;
			continue;
		}

		string message(buffer.c_array());

		if (message == TERMINATION_MESSAGE)
		{
			console() << "received termination message --> terminating thread ..." << std::endl;
			boost::this_thread::interruption_point();
		}
		else
		{
			bool fromLocalEndpoint = false;

			for (auto it = localEndpoints.begin(); it != localEndpoints.end(); ++it)
			{
				if (it->address() == remote_endpoint.address())
				{
					fromLocalEndpoint = true;
				}
			}

			if (!fromLocalEndpoint)
			{
				console()	<< "new message: " << std::endl
							<< buffer.c_array() << std::endl;

				handlerMutex.lock();

				for (auto it = handlers.begin(); it != handlers.end(); ++it)
				{
					(*it)->handle(message, remote_endpoint);
				}

				handlerMutex.unlock();
			}
		}
	}
}

void NetworkManager::subscribe(MessageHandler & handler)
{
	handlerMutex.lock();

	handlers.insert(&handler);

	handlerMutex.unlock();
}

void NetworkManager::unsubscribe(MessageHandler & handler)
{
	handlerMutex.lock();

	handlers.erase(&handler);

	handlerMutex.unlock();
}

void NetworkManager::unsubscribeAll()
{
	handlerMutex.lock();

	handlers.clear();

	handlerMutex.unlock();
}

void NetworkManager::subscribeExclusively(MessageHandler & handler)
{
	unsubscribeAll();
	subscribe(handler);
}
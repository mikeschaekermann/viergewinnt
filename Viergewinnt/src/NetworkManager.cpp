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

/************************************************************************/
/* NETWORK MANAGER                                                      */
/************************************************************************/

NetworkManager::NetworkManager(void) :
	ioService(),
	loopbackEndpoint(ip::address_v4::loopback(), LISTENING_PORT),
	unicastEndpoint(ip::address_v4::loopback(), LISTENING_PORT),
	broadcastEndpoint(ip::address_v4::broadcast(), LISTENING_PORT),
	socket(ioService, unicastEndpoint)
{
	boost::asio::socket_base::broadcast option(true);
	socket.set_option(option);
}


NetworkManager::~NetworkManager(void)
{
	numb();
}

NetworkManager::Message & NetworkManager::createMessage()
{
	return (*new Message(*this));
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

	socket.send_to(buffer(message.c_str(), message.size() + 1), endpoint, 0, error);

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
	boost::array<char, MAX_MESSAGE_LENGTH_IN_BYTES> buffer;
	while (true)
	{
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
			console()	<< "new message: " << std::endl
						<< buffer.c_array() << std::endl;

			for (auto it = handlers.begin(); it != handlers.end(); ++it)
			{
				(*it)->handle(message);
			}
		}
	}
}

void NetworkManager::subscribe(MessageHandler & handler)
{
	handlers.insert(&handler);
}

void NetworkManager::unsubscribe(MessageHandler & handler)
{
	handlers.erase(&handler);
}

#pragma once

#include "MessageHandler.h"
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/json_parser.hpp"

class MessageHandlerJSON :
	public MessageHandler
{
public:
	virtual void		handle(std::string message);
	virtual void		handle(boost::property_tree::ptree propertyTree) = 0;
};

void MessageHandlerJSON::handle(std::string message)
{
	try
	{
		std::stringstream inputStream;
		inputStream << message;

		boost::property_tree::ptree propertyTree;
		boost::property_tree::read_json(inputStream, propertyTree);

		handle(propertyTree);
	}
	catch (std::exception const& e)
	{
		console()	<< "Error while reading JSON message:" << std::endl
					<< e.what() << std::endl;
	}
}


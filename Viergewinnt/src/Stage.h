#pragma once

#include "MessageHandlerJSON.h"
#include "cinder/gl/gl.h"
#include "cinder/Timer.h"
#include "NetworkManager.h"

using namespace cinder;
using namespace cinder::gl;

class Stage :
	public MessageHandlerJSON
{
public:
	Stage(NetworkManager & networkManager, unsigned int stageIndex) :
		networkManager(networkManager),
		stageIndex(stageIndex),
		nextStage(nullptr),
		cancelled(false)
	{
		stageTimer.start();
	}

	virtual void mouseDown(MouseEvent event) = 0;
	virtual void keyDown(KeyEvent event) = 0;
	virtual void update() = 0;
	virtual void draw() = 0;
	void handle(boost::property_tree::ptree message, boost::asio::ip::udp::endpoint from)
	{
		if (belongsToStage(message))
		{
			handleStageMessage(message, from);
		}
	}
	bool isComplete() const
	{
		return (nextStage != nullptr);
	}
	bool isCancelled() const
	{
		return cancelled;
	}
	Stage * getNextStage()
	{
		return nextStage;
	}
protected:
	NetworkManager &		networkManager;
	Timer					stageTimer;
	const unsigned int		stageIndex;
	Stage *					nextStage;
	bool					cancelled;

	bool belongsToStage(boost::property_tree::ptree message)
	{
		return (message.get<int>("stage") == stageIndex);
	}
	virtual void handleStageMessage(boost::property_tree::ptree message, boost::asio::ip::udp::endpoint from)
	{
	}
};
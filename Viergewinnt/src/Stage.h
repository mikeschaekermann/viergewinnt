#pragma once

#include "MessageHandlerJSON.h"
#include "cinder/gl/gl.h"
#include "cinder/Timer.h"
#include "NetworkManager.h"
#include "cinder/params/Params.h"

#define ABORTION_STAGE 99

using namespace cinder;
using namespace cinder::gl;

class Stage :
	public MessageHandlerJSON
{
public:
	Stage(NetworkManager & networkManager, params::InterfaceGl & parameterMenu, unsigned int stageIndex, bool opponentAborted = false) :
		networkManager(networkManager),
		parameterMenu(parameterMenu),
		stageIndex(stageIndex),
		nextStage(nullptr),
		aborted(false),
		opponentAborted(opponentAborted),
		textFont("Arial", 18),
		textColor(1, 1, 1, 1)
	{
		stageTimer.start();
	}

	virtual ~Stage()
	{
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
	
	bool isAborted() const
	{
		return aborted;
	}
	
	bool wasAbortedByOpponent() const
	{
		return opponentAborted;
	}
	
	Stage * getNextStage()
	{
		return nextStage;
	}

protected:
	NetworkManager &		networkManager;
	params::InterfaceGl	&	parameterMenu;
	Timer					stageTimer;
	const unsigned int		stageIndex;
	Stage *					nextStage;
	bool					aborted,
							opponentAborted;
	Font					textFont;
	ColorA					textColor;

	bool belongsToStage(boost::property_tree::ptree message)
	{
		return (message.get<int>("stage") == stageIndex);
	}

	virtual void handleStageMessage(boost::property_tree::ptree message, boost::asio::ip::udp::endpoint from)
	{
	}

	void abortGame(bool pOpponentAborted)
	{
		opponentAborted = pOpponentAborted;
		aborted = true;
	}
};
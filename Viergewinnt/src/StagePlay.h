#pragma once

#include "Stage.h"
#include "Field.h"

class StagePlay :
	public Stage
{
public:
	StagePlay(NetworkManager & networkManager, bool begins, string myName, string opponentName, boost::asio::ip::udp::endpoint opponentEndpoint) :
		Stage(networkManager, 3),
		myTurn(begins),
		hasBegun(false),
		myName(myName),
		opponentName(opponentName),
		secondsToWaitForNextMessage(2)
	{
		networkManager.setUnicastEndpoint(opponentEndpoint);
		field.init();
	}

	virtual void mouseDown(MouseEvent event)
	{

	}

	virtual void keyDown(KeyEvent event)
	{
		switch(event.getCode())
		{
		case KeyEvent::KEY_LEFT:
			break;
		case KeyEvent::KEY_RIGHT:
			break;
		case KeyEvent::KEY_RETURN:
			break;
		}
	}

	virtual void update()
	{
		if (stageTimer.getSeconds() >= secondsToWaitForNextMessage)
		{
			if (!hasBegun)
			{
				sendReadinessMessage();
			}

			stageTimer.stop();
			stageTimer.start();
		}
	}

	virtual void draw()
	{
		field.draw();
	}

private:
	const double				secondsToWaitForNextMessage;
	string						myName,
								opponentName;
	bool						myTurn,
								hasBegun;
	Field						field;

	void handleStageMessage(boost::property_tree::ptree message, boost::asio::ip::udp::endpoint from)
	{
	}

	void sendReadinessMessage()
	{
		networkManager
			.createMessage()
			.add("stage", stageIndex)
			.makeJSON()
			.unicast();
	}

	void abortGame()
	{
		cancelled = true;
	}
};
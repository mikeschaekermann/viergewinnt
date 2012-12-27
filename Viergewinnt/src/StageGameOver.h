#pragma once

#include "Stage.h"
#include "StageFindOpponent.h"

class StageGameOver :
	public Stage
{
public:
	StageGameOver(NetworkManager & networkManager, params::InterfaceGl & parameterMenu, bool isDraw, string winnerName) :
		Stage(networkManager, parameterMenu, 5),
		isDraw(isDraw),
		winnerName(winnerName),
		secondsToWaitForNextMessage(2),
		instruction("Press any key to return to the player list!"),
		textPosition(20, 20)
	{
	}

	virtual void mouseDown(MouseEvent event)
	{

	}

	virtual void keyDown(KeyEvent event)
	{
		abortGame(true);
	}

	virtual void update()
	{
		if (stageTimer.getSeconds() >= secondsToWaitForNextMessage)
		{
			sendGameOverMessage();
		}
	}

	virtual void draw()
	{
		if (isDraw)
		{
			drawString("This was a draw! " + instruction, textPosition, textColor, textFont);
		}
		else
		{
			drawString("And the winner is ... " + winnerName + "! Congratulations! " + instruction, textPosition, textColor, textFont);
		}
	}

private:
	const double				secondsToWaitForNextMessage;
	string						winnerName,
								instruction;
	bool						isDraw;
	Vec2i						textPosition;

	void sendGameOverMessage()
	{
		networkManager
			.createMessage()
			.add("stage", stageIndex)
			.makeJSON()
			.unicast();
	}
};
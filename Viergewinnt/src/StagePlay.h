#pragma once

#include "Stage.h"
#include "Field.h"
#include "StageGameOver.h"

class StagePlay :
	public Stage
{
public:
	StagePlay(NetworkManager & networkManager, params::InterfaceGl & parameterMenu, bool begins, string myName, string opponentName, boost::asio::ip::udp::endpoint opponentEndpoint) :
		Stage(networkManager, parameterMenu, 3),
		myTurn(begins),
		receiveMoves(!myTurn),
		hasBegun(false),
		keepSendingReadinessMessages(true),
		myName(myName),
		myLastMove(1),
		winner(-1),
		instruction("Move: arrow keys; Choose: enter; Tipp: "),
		opponentName(opponentName),
		secondsToWaitForNextMessage(2),
		secondsToWaitForNextCommunication(10),
		secondsToWaitForNextMove(30),
		secondsToWaitToReceiveMovesAfterOwnMove(2.5 * secondsToWaitForNextMessage),
		secondsToWaitBeforeGameOverStage(2.5 * secondsToWaitForNextMessage)
	{
		networkManager.setUnicastEndpoint(opponentEndpoint);
		noMoveTimer.start();
		
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
			field.activatePrev();
			break;
		case KeyEvent::KEY_RIGHT:
			field.activateNext();
			break;
		case KeyEvent::KEY_RETURN:
			if (hasBegun)
				if (myTurn)
				{
					if (field.insert(1))
					{
						myTurn = false;
						receiveMoves = false;

						noCommunicationTimer.stop();

						noMoveTimer.stop();
						noMoveTimer.start();

						noMoveReceptionTimer.stop();
						noMoveReceptionTimer.start();

						myLastMove = field.getActiveColumn();
						console() << "COLUMN " << myLastMove << " WAS CHOSEN!" << std::endl;

						checkForWinner();
					}
					else
					{
						console() << "FIELD IS OCCUPIED! PLEASE CHOOSE ANOTHER ONE!" << std::endl;
					}
				}
				else
				{
					console() << "WAIT FOR OTHER PLAYER'S TURN!" << std::endl;
				}
			else
			{
				console() << "WAIT FOR OTHER PLAYER TO GET READY!" << std::endl;
			}
			break;
		case KeyEvent::KEY_ESCAPE:
			abortGame(false);
			break;
		}
	}

	virtual void update()
	{
		if (noMoveReceptionTimer.getSeconds() >= secondsToWaitToReceiveMovesAfterOwnMove)
		{
			receiveMoves = true;
		}

		if (stageTimer.getSeconds() >= secondsToWaitForNextMessage)
		{
			if (keepSendingReadinessMessages)
			{
				sendReadinessMessage();
			}

			if (hasBegun && !myTurn)
			{
				sendMoveMessage(myLastMove);
			}

			stageTimer.stop();
			stageTimer.start();
		}

		if (hasTimedOut())
		{
			abortGame(false);
		}

		if (gameOverTimer.getSeconds() >= secondsToWaitBeforeGameOverStage)
		{
			nextStage = new StageGameOver(networkManager, parameterMenu, isDraw, winnerName);
		}
	}

	virtual void draw()
	{
		string infoText;

		if (!hasBegun)
		{
			infoText = "Waiting for opponent ...";
		}
		else if (winner == -1)
		{
			if (myTurn)
			{
				infoText = "It's your turn!";
			}
			else
			{
				infoText = "It's " + opponentName + "'s turn!";
			}
		}
		else
		{
			infoText = "Game over! Wait ...";
		}

		drawString(instruction + infoText, Vec2i(95, 20), textColor, textFont);

		field.draw();
	}

private:
	const double				secondsToWaitForNextMessage,
								secondsToWaitForNextCommunication,
								secondsToWaitForNextMove,
								secondsToWaitToReceiveMovesAfterOwnMove,
								secondsToWaitBeforeGameOverStage;
	cinder::Timer				noCommunicationTimer,
								noMoveTimer,
								noMoveReceptionTimer,
								gameOverTimer;
	string						myName,
								opponentName,
								instruction;
	bool						hasBegun,
								myTurn,
								receiveMoves,
								keepSendingReadinessMessages;
	Field						field;
	unsigned int				myLastMove;
	int							winner;
	string						winnerName;
	bool						isDraw;

	void handle(boost::property_tree::ptree message, boost::asio::ip::udp::endpoint from)
	{
		auto stage = message.get<int>("stage");

		/// received readiness message
		if (stage == stageIndex)
		{
			hasBegun = true;
		}
		/// received potential move message
		else if (stage == stageIndex + 1)
		{
			keepSendingReadinessMessages = false;

			int column = message.get("column", -1);

			if (column == -1)
			{
			}
			/// received move message
			else if (!myTurn)
			{
				if (receiveMoves)
				{
					if (column >= 1 && column <= FIELD_WIDTH && receiveMoves)
					{
						if (field.insert(column, 0))
						{
							console() << "PLAYER " << opponentName << " MADE HIS MOVE IN COLUMN " << column << "!" << std::endl;
							myTurn = true;

							noCommunicationTimer.start();

							receiveMoves = false;

							checkForWinner();
						}
						else
						{
							console() << "PLAYER " << opponentName << " TRIED TO USE COLUMN " << column << " ALTHOUGH IT IS FULL!" << std::endl;
						}
					}
					else
					{
						console() << "PLAYER " << opponentName << " TRIED TO USE INVALID COLUMN " << column << "!" << std::endl;
					}
				}
				else
				{
					console() << "PLAYER " << opponentName << " SENT MOVE ON COLUMN " << column << " >> IGNORED!" << std::endl;
				}
			}
			else
			{
				noCommunicationTimer.stop();
				noCommunicationTimer.start();

				console() << "PLAYER " << opponentName << " SENT KEEP-ALIVE MESSAGE!" << std::endl;
			}
		}
		/// received abortion message
		else if (stage == ABORTION_STAGE)
		{
			abortGame(true);
		}
	}

	void checkForWinner()
	{
		winner = field.getWinner();
		
		if (field.isFull() || winner != -1)
		{
			isDraw = (field.isFull() && winner == -1);

			if (winner == 0)
			{
				winnerName = opponentName;
			}
			else
			{
				winnerName = myName;
			}

			gameOverTimer.start();
		}
	}

	bool hasTimedOut() const
	{
		bool timedOut = (
			hasBegun &&
			(
				(myTurn && noCommunicationTimer.getSeconds() > secondsToWaitForNextCommunication) ||
				(!myTurn && noMoveTimer.getSeconds() > secondsToWaitForNextMove)
			)
		);

		if (timedOut)
		{
			console() << "Other player has timed out!" << std::endl;
		}

		return timedOut;
	}

	void sendReadinessMessage()
	{
		networkManager
			.createMessage()
			.add("stage", stageIndex)
			.makeJSON()
			.unicast();
	}

	void sendMoveMessage(unsigned int column)
	{
		assert(column <= FIELD_WIDTH);

		networkManager
			.createMessage()
			.add("stage", stageIndex + 1)
			.add("column", column)
			.makeJSON()
			.unicast();
	}
};
#pragma once

#include "Stage.h"
#include "cinder/params/Params.h"
#include "StagePlay.h"

class StageFindOpponent :
	public Stage
{
public:
	struct Player
	{
		Player(const string & name, const boost::asio::ip::udp::endpoint & endpoint) :
			name(name),
			endpoint(endpoint),
			invited(false),
			inviting(false),
			secondsToWaitForInvitationReply(10)
		{
			playerTimer.start();
		}

		void invite()
		{
			if (!invited)
			{
				invited = true;
				invitedTimer.start();
			}
		}

		bool isInvited()
		{
			if (invited && invitedTimer.getSeconds() > secondsToWaitForInvitationReply)
			{
				invited = false;
				invitedTimer.stop();
			}

			return invited;
		}

		string getStatus() const
		{
			string status;

			if (invited || inviting) status += " ( ";
			if (invited) status += "invited";
			if (invited && inviting) status += " / ";
			if (inviting) status += "inviting";
			if (invited || inviting) status += " )";

			return status;
		}

		string toString() const
		{
			return (name + getStatus());
		}

		void refresh(Player const & newPlayer)
		{
			name = newPlayer.name;

			playerTimer.stop();
			playerTimer.start();
		}

		bool isOlderThan(double seconds) const
		{
			return (playerTimer.getSeconds() > seconds);
		}

		bool operator==(const Player & otherPlayer)
		{
			return (endpoint == otherPlayer.endpoint);
		}

		cinder::Timer					playerTimer,
										invitedTimer;
		string							name;
		boost::asio::ip::udp::endpoint	endpoint;
		bool							invited,
										inviting;
		const double					secondsToWaitForInvitationReply;
	};

	StageFindOpponent(NetworkManager & networkManager) :
		Stage(networkManager, 1),
		secondsToWaitForNextBroadcast(2),
		secondsToLive(10),
		playerName("Mike"),
		activeOpponent(opponents.end()),
		parameterMenu("Please enter your ...", Vec2i( 200, 100 ))
	{
		sendReadinessMessage();
		parameterMenu.addParam("name", &playerName);
	}

	virtual void mouseDown(MouseEvent event)
	{

	}

	virtual void keyDown(KeyEvent event)
	{
		switch(event.getCode())
		{
		case KeyEvent::KEY_DOWN:
			if (opponents.size() > 0)
			{
				if (activeOpponent == opponents.end())
				{
					activeOpponent = opponents.begin();
				}
				else
				{
					++activeOpponent;
				}

				if (activeOpponent == opponents.end())
				{
					activeOpponent = opponents.begin();
				}
			}
			break;
		case KeyEvent::KEY_UP:
			if (opponents.size() > 0)
			{
				if (activeOpponent == opponents.begin())
				{
					activeOpponent = opponents.end();
				}

				--activeOpponent;
			}
			break;
		case KeyEvent::KEY_RETURN:
			if (activeOpponent != opponents.end())
			{
				if (activeOpponent->inviting)
				{
					sendInvitationReplyMessage(activeOpponent->endpoint);
					console() << "YOU ACCEPTED AN INVITATION!" << std::endl;
					nextStage = new StagePlay(networkManager, false, playerName, activeOpponent->name, activeOpponent->endpoint);
				}
				else if (!activeOpponent->isInvited())
				{
					activeOpponent->invite();
					sendInvitationMessage(activeOpponent->endpoint);
				}
			}
			break;
		}
	}

	virtual void update()
	{
		opponentsMutex.lock();

		for (auto it = opponents.begin(); it != opponents.end();)
		{
			if (it->isOlderThan(secondsToLive))
			{
				if (it == activeOpponent)
				{
					activeOpponent = opponents.end();
				}

				opponents.erase(it);
			}
			else
			{
				++it;
			}
		}

		opponentsMutex.unlock();

		if (stageTimer.getSeconds() >= secondsToWaitForNextBroadcast)
		{
			sendReadinessMessage();
			
			stageTimer.stop();
			stageTimer.start();

			for (auto it = opponents.begin(); it != opponents.end(); ++it)
			{
				if (it->isInvited())
				{
					sendInvitationMessage(it->endpoint);
				}
			}
		}
	}

	virtual void draw()
	{
		Vec2f textPosition(230, 20);

		drawString("List of players (navigate with arrow keys, then press return!)", textPosition);

		textPosition.y += 50;

		for (auto it = opponents.begin(); it != opponents.end(); ++it)
		{
			ColorA color;

			if (it == activeOpponent)
			{
				color = ColorA(1, 0, 0, 1);
			}
			else
			{
				color = ColorA(1, 1, 1, 1);
			}

			drawString(it->toString(), textPosition, color);
			textPosition.y += 30;
		}

		parameterMenu.draw();
	}

private:
	boost::mutex				opponentsMutex;
	list<Player>				opponents;
	list<Player>::iterator		activeOpponent;
	params::InterfaceGl			parameterMenu;
	const double				secondsToLive,
								secondsToWaitForNextBroadcast;
	string						playerName;

	void handle(boost::property_tree::ptree message, boost::asio::ip::udp::endpoint from)
	{
		Player opponent(message.get<string>("clientname"), from);

		auto existingOpponent = find(opponents.begin(), opponents.end(), opponent);

		auto stage = message.get<int>("stage");
		
		if (stage == stageIndex)
		{
			opponentsMutex.lock();

			if (existingOpponent != opponents.end())
			{
				existingOpponent->refresh(opponent);
			}
			else
			{
				opponents.push_back(opponent);
			}

			opponentsMutex.unlock();
		}
		else if (stage == stageIndex + 1)
		{
			if (existingOpponent != opponents.end())
			{
				if (existingOpponent->isInvited())
				{
					console() << "YOUR INVITATION WAS ACCEPTED!" << std::endl;
					nextStage = new StagePlay(networkManager, true, playerName, existingOpponent->name, existingOpponent->endpoint);
				}
				else
				{
					existingOpponent->inviting = true;
				}
			}
			else
			{
				opponent.inviting = true;
				opponents.push_back(opponent);
			}
		}
	}

	void sendReadinessMessage()
	{
		networkManager
			.createMessage()
			.add("stage", stageIndex)
			.add("clientname", playerName)
			.makeJSON()
			.broadcast();
	}

	void sendInvitationMessage(boost::asio::ip::udp::endpoint endpoint)
	{
		networkManager
			.createMessage()
			.add("stage", (stageIndex + 1))
			.add("clientname", playerName)
			.makeJSON()
			.send(endpoint);
	}

	void sendInvitationReplyMessage(boost::asio::ip::udp::endpoint endpoint)
	{
		networkManager
			.createMessage()
			.add("stage", (stageIndex + 1))
			.makeJSON()
			.send(endpoint);
	}
};
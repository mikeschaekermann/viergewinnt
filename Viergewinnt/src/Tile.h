#pragma once
#include <string>

#include "boost/shared_ptr.hpp"

class Tile;
typedef boost::shared_ptr<Tile> TilePtr;

class Tile
{
private:
	bool m_isEmpty;
	int m_player;
public:
	Tile(void);
	~Tile(void);

	bool isEmpty() const;
	void setOccupied();

	void setPlayer(int n);

	void draw();
	std::string toString() const;
};


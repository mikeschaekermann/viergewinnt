#pragma once
#include <string>

#include "boost/shared_ptr.hpp"
#include "cinder/Color.h"

#define TILE_WIDTH 20
#define TILE_HEIGHT 20


class Tile;
typedef boost::shared_ptr<Tile> TilePtr;

class Tile
{
private:
	const static ci::Color m_emptyColor;
	const static ci::Color m_p0Color;
	const static ci::Color m_p1Color;
	bool m_isEmpty;
	int m_player;

public:
	Tile(void);
	~Tile(void);

	bool isEmpty() const;
	void setOccupied();

	void setPlayer(int n);

	void draw();
	void drawAt(int x, int y);
	std::string toString() const;
};


#pragma once
#include <string>

#include "boost/shared_ptr.hpp"
#include "cinder/Color.h"

#define TILE_WIDTH 70
#define TILE_HEIGHT 70
#define OFFSET_LEFT 75
#define OFFSET_TOP 60


class Tile;
typedef boost::shared_ptr<Tile> TilePtr;

class Tile
{
private:
	const static ci::Color m_emptyColor;
	const static ci::Color m_p0Color;
	const static ci::Color m_p1Color;
	const static ci::Color m_selectionColor;
	bool m_isEmpty;
	int m_player;

public:
	Tile(void);
	~Tile(void);

	bool isEmpty() const;
	void setOccupied();

	void setPlayer(int n);
	bool belongsToPlayer(int n) const;

	void draw();
	void drawAt(int x, int y, bool active);
	std::string toString() const;
};


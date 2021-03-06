#pragma once
#include <string>	

#include "Tile.h"

#define FIELD_WIDTH 7
#define FIELD_HEIGHT 6

class Field
{
public:
	Field(void);
	~Field(void);

	void init();

	bool isFull(int col) const;
	bool isFull() const;
	int getWinner() const;

	void activateNext();
	void activatePrev();
	unsigned int getActiveColumn();
	
	bool insert(int player);
	bool insert(int col, int player);

	void draw();
	void print() const;

private:
	TilePtr m_tiles[FIELD_WIDTH][FIELD_HEIGHT];
	int activeColumn;
};


#include "Field.h"

#include "cinder/app/App.h"

Field::Field(void) :
	activeColumn(0)
{

}


Field::~Field(void)
{
	init();
}

void Field::init()
{
	for(int x=0; x<FIELD_WIDTH; ++x)
	{
		for(int y=0; y<FIELD_HEIGHT; ++y)
		{
			m_tiles[x][y] = TilePtr(new Tile());
		}
	}
}

bool Field::isFull( int col ) const
{
	for(int y=0; y<FIELD_HEIGHT; ++y)
	{
		if(m_tiles[col][y]->isEmpty())
		{
			return false;
		}
	}
	return true;
}

bool Field::isFull() const
{
	for (int x = 0; x < FIELD_WIDTH; ++x)
	{
		if (!isFull(x)) return false;
	}

	return true;
}

void Field::activateNext()
{
	++activeColumn;

	if (activeColumn > FIELD_WIDTH - 1)
	{
		activeColumn = 0;
	}
}

void Field::activatePrev()
{
	--activeColumn;
	
	if (activeColumn < 0)
	{
		activeColumn = FIELD_WIDTH - 1;
	}
}

unsigned int Field::getActiveColumn()
{
	return (activeColumn + 1);
}

bool Field::insert(int player)
{
	return insert(activeColumn + 1, player);
}

bool Field::insert(int col, int player)
{
	int column = col - 1;

	for(int y=0; y<FIELD_HEIGHT; ++y)
	{
		if(m_tiles[column][y]->isEmpty())
		{
			m_tiles[column][y]->setPlayer(player);
			return true;
		}
	}
	return false;
	
}

void Field::draw()
{
	for(int y=FIELD_HEIGHT-1; y>=0; --y)
	{
		for(int x=0; x<FIELD_WIDTH; ++x)
		{
			m_tiles[x][y]->drawAt(x, FIELD_HEIGHT-1-y, x == activeColumn);
		}
	}
}

void Field::print() const
{
	for(int y=FIELD_HEIGHT-1; y>=0; --y)
	{
		for(int x=0; x<FIELD_WIDTH; ++x)
		{
			ci::app::console() << m_tiles[x][y]->toString() << "\t|";
		}
		ci::app::console() << std::endl;
	}
}

int Field::getWinner() const
{
	for (int p = 0; p <= 1; ++p)
	{
		for (int sx = 0; sx < FIELD_WIDTH; ++sx)
		{
			for (int sy = 0; sy < FIELD_HEIGHT; ++sy)
			{
				/// check for horizontal row

				int counter = 0;

				for (int x = sx; x < FIELD_WIDTH; ++x)
				{
					if (m_tiles[x][sy]->belongsToPlayer(p) && counter < 4)
					{
						++counter;
					}
					else
					{
						break;
					}
				}

				if (counter == 4) return p;

				/// check for vertical row

				counter = 0;

				for (int y = sy; y < FIELD_HEIGHT; ++y)
				{
					if (m_tiles[sx][y]->belongsToPlayer(p) && counter < 4)
					{
						++counter;
					}
					else
					{
						break;
					}
				}

				if (counter == 4) return p;

				/// check for diagonal row from top left to bottom right

				counter = 0;

				for (int y = sy, x = sx; y >= 0 && x < FIELD_WIDTH; --y, ++x)
				{
					if (m_tiles[x][y]->belongsToPlayer(p) && counter < 4)
					{
						++counter;
					}
					else
					{
						break;
					}
				}

				if (counter == 4) return p;

				/// check for diagonal row from bottom left to up right

				counter = 0;

				for (int y = sy, x = sx; y < FIELD_HEIGHT && x < FIELD_WIDTH; ++y, ++x)
				{
					if (m_tiles[x][y]->belongsToPlayer(p) && counter < 4)
					{
						++counter;
					}
					else
					{
						break;
					}
				}

				if (counter == 4) return p;
			}
		}
	}

	return -1;
}
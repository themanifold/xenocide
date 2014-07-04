#include "mem_check.h"

#include "tiles.h"
#include "system.h"
#include <stdlib.h>
#include <string>


CTile :: CTile()
{
	ClassName="TILE";
	ChangePosition(-1,-1);
	color = 0;
	invisible = false;
	positionX=-1;
	positionY=-1;
}

CTile :: ~CTile()
{
}

bool CTile::ChangePosition(int x, int y)
{
	positionX = x;
	positionY = y;
	return true;
}

void CTile :: display()
{
  draw_at(pX(),pY());
}

void CTile :: draw_at(int x, int y)
{
	set_color(color);
	print_character(x,y,tile);
}


void CTile :: rename(string text)
{
   name=text;
}


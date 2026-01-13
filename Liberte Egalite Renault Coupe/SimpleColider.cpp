#include "SimpleColider.h"


bool SimpleColider::DetectCollison(Player player, Enemy enemy)
{
	if ((player.position - enemy.position).Length() < 150)
		return true;
	return false;
}
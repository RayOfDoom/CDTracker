#include "Objects.h"
#include <iostream>

float Spell::getCoolDown(float gameTime) {
	return readyAt > gameTime ? readyAt - gameTime : 0;
}

Vector2 Game::GetHpBarPos(Champion& champ) {
	Vector3 pos = champ.pos.clone();

	pos.y += champ.healthBarHeight;

	Vector2 w2s = renderer->WorldToScreen(pos);
	w2s.y -= (renderer->height * 0.00083333335f * champ.healthBarHeight);

	return w2s;
}
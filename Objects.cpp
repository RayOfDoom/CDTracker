#include "Objects.h"
#include <iostream>

Game::Game() {};

Champion::Champion() {};

Spell::Spell() {};

float Spell::getCoolDown(float gameTime) {
	return readyAt > gameTime ? readyAt - gameTime : 0;
}
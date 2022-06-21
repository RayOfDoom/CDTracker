#include <iostream>
#include "Windows.h"
#include "Utils.h"
#include "conio.h"
#include "MemoryReader.h"
#include "Overlay.h"
#include "Input.h"
#include "GameRenderer.h"

void mainLoop(Overlay& overlay, MemoryReader& reader, std::map<std::string, float> ChampionData) {
	Game game;
	bool rehook = true;
	printf("Waiting for league process\n");

	while (true) {
		bool isLeagueWindowActive = reader.IsActive();
		if (overlay.IsVisible()) {
			if (Input::WasKeyPressed(HKey::F8)) {
				overlay.ToggleTransparent();
			}
			if (Input::WasKeyPressed(HKey::F7)) {
				overlay.drawUI = !overlay.drawUI;
			}
			if (Input::WasKeyPressed(HKey::F6)) {
				overlay.drawOverlay = !overlay.drawOverlay;
			}
			if (!isLeagueWindowActive) {
				overlay.Hide();
			}
		}
		else if (isLeagueWindowActive) {
			overlay.Show();
		}

		try {
			overlay.StartFrame();

			if (rehook) {
				reader.Hook();
				rehook = false;
				game = Game();
				if(!overlay.isTransparent) overlay.ToggleTransparent();
				printf("Successfully hooked\n");
			}
			else {
				if (!reader.IsHooked()) {
					rehook = true;
					printf("League process ended. Waiting...\n");
				}

				reader.UpdateGameTime(game);
				reader.ReadChamps(game, ChampionData);
				reader.ReadRenderer(game);

				if (game.gameTime > 2.f) {
					overlay.Update(game);
				}
			}
		}
		catch (WinApiException) {
			rehook = true;
		}
		catch (std::runtime_error exception) {
			printf("An unexpected error occured. %s", exception.what());
			break;
		}
		overlay.RenderFrame();
	}
}

int main() {
	Overlay overlay = Overlay();
	MemoryReader reader = MemoryReader();

	try {
		printf("Getting champion data\n");
		std::map<std::string, float> ChampionData = Json::GetChampionData(); //gets health bar height

		printf("Initializing UI\n");
		overlay.Init();

		mainLoop(overlay, reader, ChampionData);
	}
	catch (std::runtime_error exception) {
		std::cout << exception.what() << std::endl;
	}
	printf("Press any key to exit...");
	_getch();
}
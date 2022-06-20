#include <iostream>
#include "Windows.h"
#include "Utils.h"
#include "conio.h"
#include "MemoryReader.h"
#include "Overlay.h"
#include "Input.h"

void mainLoop(Overlay& overlay, MemoryReader& reader) {
	Game game;
	bool rehook = true;
	printf("Waiting for league process\n");

	while (true) {
		bool isLeagueWindowActive = reader.IsActive();
		if (overlay.IsVisible()) {
			if (Input::WasKeyPressed(HKey::F8)) {
				overlay.ToggleTransparent();
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
				printf("Successfully hooked\n");
			}
			else {
				if (!reader.IsHooked()) {
					rehook = true;
					printf("League process ended. Waiting...\n");
				}

				reader.UpdateGameTime(game);
				reader.ReadChamps(game);

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
		Sleep(50);
	}
}

int main() {
	Overlay overlay = Overlay();
	MemoryReader reader = MemoryReader();

	try {
		overlay.Init();
		mainLoop(overlay, reader);
	}
	catch (std::runtime_error exception) {
		std::cout << exception.what() << std::endl;
	}
	printf("Press any key to exit...");
	_getch();
}
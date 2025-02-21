
#include "Chip8.h"
#include "Platform.h"


int main(int argc, char** argv)
{
	std::cout << "Program started!" << std::endl;

	if (argc != 4)
	{
		std::cerr << "Usage: " << argv[0] << " <Scale> <Delay> <ROM>\n";
		std::cerr << "Error: Invalid number of arguments!" << std::endl;
		std::system("Usage");
		std::exit(EXIT_FAILURE);
	}

	std::cout << "Arguments received: " << argv[1] << ", " << argv[2] << ", " << argv[3] << std::endl;

	int videoScale = std::stoi(argv[1]);
	int cycleDelay = std::stoi(argv[2]);
	char const* romFilename = argv[3];


	std::cout << "Trying to load ROM: " << argv[3] << std::endl;
	Chip8 chip8;
	chip8.LoadROM(romFilename);

	std::cout << "ROM loaded successfully" << std::endl;

	Platform platform("CHIP-8 Emulator", chip8.VIDEO_WIDTH * videoScale, chip8.VIDEO_HEIGHT * videoScale, chip8.VIDEO_WIDTH, chip8.VIDEO_HEIGHT);

	int videoPitch = sizeof(chip8.video[0]) * chip8.VIDEO_WIDTH; 

	auto lastCycleTime = std::chrono::high_resolution_clock::now();
	bool quit = false;

	while (!quit)
	{
		quit = platform.ProcessInput(chip8.keypad);

		auto currentTime = std::chrono::high_resolution_clock::now();
		float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

		if (dt > cycleDelay)
		{
			lastCycleTime = currentTime;

			chip8.Cycle();

			platform.Update(chip8.video, videoPitch);
		}
	}
	return 0;
}
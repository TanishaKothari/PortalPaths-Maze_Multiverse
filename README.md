### PortalPaths: Multiverse Mazes
A dynamic maze game featuring multiple themed environments, powerups, and both timed and untimed gameplay modes.

## Description
PortalPaths is an innovative maze game where players navigate through procedurally generated mazes across three distinct themes:
- <b>Space Theme</b>: Navigate a spaceship through asteroid fields towards a mysterious portal
- <b>Jungle Theme</b>: Guide a monkey through dense vegetation to collect bananas
- <b>Desert Theme</b>: Lead a camel through an array of cacti and tumbleweeds to reach an oasis

## Features
- <b>Multiple Game Modes</b>:
    - Timed Mode: Race against the clock
    - Untimed Mode: Explore at your own pace
- <b>Difficulty Levels</b>:
    - Easy: Larger cells, fewer obstacles
    - Medium: Balanced challenge with speed boosts
    - Hard: Smaller cells, more obstacles, strategic powerups
- <b>Powerups</b>:
    - Time Bonuses: Add precious seconds in timed mode
    - Speed Boosts: Temporarily increase movement speed
- <b>Dynamic Elements</b>:
    - Theme-specific visuals and obstacles
    - Particle effects for portal
    - Ambient background music
    - Sound effects for all interactions
- <b>Additional Features</b>:
    - High score system with persistent storage
    - Pause menu with sound settings
    - Smooth character movement and rotation
    - OpenMP parallelization for maze generation

## Controls
- Movement: Arrow Keys or WASD
- Pause/Settings: P
- Menu Navigation: Number keys (1-4)

## Dependencies
- [raylib](https://www.raylib.com/) - A simple and easy-to-use game programming library
- C++17 compatible compiler
- OpenMP support

## Building
The project includes a comprehensive Makefile supporting Windows, macOS, and Linux:
```sh
# Debug build
make PLATFORM=PLATFORM_DESKTOP BUILD_MODE=DEBUG

# Release build
make PLATFORM=PLATFORM_DESKTOP
```

## Directory Structure
```
.
├── Assets/           # Game textures and sprites
├── sounds/          # Audio files
├── .vscode/         # VS Code configuration
├── main.cpp         # Main game source code
├── Makefile        # Build configuration
└── README.md       # This file
```

## Credits
- Game developed using [raylib](https://www.raylib.com/)
- Audio and visual assets used with appropriate licensing

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
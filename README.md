# Astral

A lightweight C++17 library for 2D/3D sound built on top of SDL_mixer.

## Building

*Astral* is a C++17 header-only library. To use it in your project, add the source files to the projects include directory and put `#include "Astral.h"` atop every file you wish to use Astral in.  
Astral requires [SDL2](https://www.libsdl.org/) and [SDL mixer](https://www.libsdl.org/projects/SDL_mixer/). To make sure Astral works properly, build and include SDL2 and SDL mixer in your project.

## Features

- [Initialisation](#Initialisation)
- [Playing sounds](#Playing-Sounds)
- [Playing music](#Playing-Music)
- [Positional audio](#Positional-Audio)
- [Other utilities](#Other-Utilities)

### Initialisation

To initialise Astral, first initialise SDL2. Astral then can be initialised with the `astral::Initialise()` function.  
This function can take an optional `astral::InitialiseData` struct containing optional configuration information.  

	requiredFileTypes - a list of astral::RequiredFileTypes of additional file types that are needed to be loaded (OGG, MP3, et cetera).
	formatInfo.isSigned - whether the output sample format is signed or not (default: true).
	formatInfo.bitCount - the number of bits per output sample (default: 16).
	formatInfo.endianness - the endianness of the output samples (default: astral::Endianness::System).
	frequency - the frequency of the output samples per second (Hz) (default: 44,100).
	chunkSize - the number of bytes to allocate for each output sample (default: 4,096).
	allocatedChannels - the number of sound channels to allocate for mixing (default: 16).
	maxDistance - the max number of units a sound can be heard from (default: 100.0f).
	reverseStereo - whether to reverse the stereo channels or not (default: false).

In order to be able to use the required file types, the appropriate DLLs from SDL mixer will need to be included with the executable.  
WAV files are supported by default.  

**Initialising Astral**
```cpp
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS | SDL_INIT_AUDIO) != 0)
	{
		// SDL failed to initialise...
	}

	try
	{
		astral::InitialiseData initData;
		{
			initData.requiredFileTypes = { astral::RequiredFileTypes::OGG, astral::RequiredFileTypes::MP3 };

			initData.formatInfo.isSigned = true;
			initData.formatInfo.bitCount = 16;
			initData.formatInfo.endianness = astral::Endianness::System;

			initData.frequency = 44'100;
			initData.chunkSize = 4'096;

			initData.allocatedChannels = 16;
			initData.maxDistance = 200.0f;
			initData.reverseStereo = true;
		}

		astral::Initialise(initData);
	}
	catch (const astral::Error&)
	{
		// Astral failed to initialise...
	}
```

Astral needs to be terminated before SDL is as well.  
**Quitting Astral**
```cpp
	astral::Quit();
	SDL_Quit();
```

### Playing Sounds

**Loading a sound from a file**
```cpp
	astral::Sound sound("audio/test.wav");
```

**Playing a sound**
```cpp
	// Play the sound once
	sound.Play();

	// Wait for the sound to finish playing
	while (sound.IsPlaying()) { }

	// Play the sound a certain number of times
	const unsigned int playTimes = 10;
	sound.Play(playTimes);

	// Play the sound forever
	sound.Play(astral::LoopForever);

	// Pause and resume the sound
	sound.Pause();

	if (sound.IsPaused())
	{
		sound.Resume();
	}

	// Stop the sound
	sound.Stop();

	// Play the sound with fade-in and fade-out times (100ms and 200ms respectively)
	sound.Play(1, 100, 200);

```

**Changing the volume of a sound**  
The sound's volume is a clamped floating point number from 0.0 to 1.0 - where 0.0 is no sound and 1.0 is max volume.  
Volume does not work on playback with fade-in or fade-out times.
```cpp
	sound.SetVolume(0.5f);

	std::cout << sound.GetVolume() << "\n"; // Prints 0.5
```

### Playing Music

Playing music is very similar to playing a sound. However, only one piece of music can be played at a time.

**Loading and playing a music file**
```cpp
	// Load the music file
	astral::Music music("audio/test_music.mp3");

	// Set and get the volume of the music
	music.SetVolume(0.6f);
	std::cout << music.GetVolume() << "\n"; // Prints 0.6

	// Play the music once
	music.Play();

	// Test if the music is paying and stop the music
	if (music.IsPlaying())
	{
		music.Stop();
	}

	// Play the music a certain number of times
	const unsigned int playTimes = 3;
	music.Play(playTimes);

	// Play the music with a fade-in millisecond time
	const unsigned int fadeInMilliseconds = 2'000;
	music.Play(1, fadeInMilliseconds);

	// Play the music forever
	music.Play(astral::LoopForever);

	// Pause and resume the music
	music.Pause();

	if (music.IsPaused())
	{
		music.Resume();
	}

	// Rewind the music to the beginning
	music.Rewind();
```

There are also several global music functions that work on all music.  
**Static music functions**
```cpp
	// Test if there is music playing
	astral::Music::IsMusicPlaying();

	// Test if the current music is paused
	astral::Music::IsMusicPaused();

	// Stop all music from playing
	astral::Music::StopAllMusic();

	// Stop all music from playing with a fade-out millisecond time
	astral::Music::StopAllMusic(500); // In this case it will fade out after 500 milliseconds
```

### Positional Audio

Astral has limited support for 3D audio. Sound objects have a position variable that can be used for positional audio. Music does not and always plays at the listener.  
**Setting the position of sounds**
```cpp
	sound.SetPosition({ 100.0f, 75.0f, -90.0f });

	const astral::Vec3 position = sound.GetPosition();
	std::cout << position.x << ", " << position.y << ", " << position.z << "\n"; // Prints 100.0, 75.0, -90.0
```

The position and orientation of the listener can also be altered.  
**Changing the properties of the listener**
```cpp
	// Change the listener's position
	astral::Listener::SetPosition({ 10.0f, -20.0f, 30.0f });

	// Change the listener's look at direction (the position the listener is looking at)
	astral::Listener::SetLookAtVector({ 0.0f, 0.0f, -1.0f });

	// Change the listener's up vector (default: { 0.0f, 1.0f, 0.0f })
	// It is best to keep this vector normalised
	astral::Listener::SetUpVector({ 0.0f, -1.0f, 0.0f });

	// Likewise, there also exists getter functions for these properties
```

The max distance is a setting that is the maximum distance from the listener that a sound can be heard from.  
**Setting the maximum distance**
```cpp
	// Make it so that sounds can be heard from up to 200.0 units from the listener
	astral::Sound::SetMaxDistance(200.0f);

	std::cout << astral::Sound::GetMaxDistance() << "\n"; // Prints 200.0
```

### Other Utilities

Reversing the stereo will swap the left and right speaker outputs.  
**Reversing the stereo outputs**
```cpp
	// Reverse the stereo outputs
	astral::ReverseStereo(true);

	// Undo the reversed stereo
	astral::ReverseStereo(false);
```

The version of Astral can be queried. The result is a struct containing the major, minor, and patch of the current Astral version.  
**Querying the version of Astral**
```cpp
	const auto[major, minor, patch] = astral::Version;
```

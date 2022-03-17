// internal
#include "common.hpp"
#include "tiny_ecs.hpp"
#include <unordered_map>

enum MUSIC_LIST {
	IN_GAME_BACKGROUND
};

enum SOUND_EFFECTS {
	CAT_SCREAM,
	GUNSHOT,
	WIN
};

class AudioManager {

public:
	
	std::unordered_map<int, Mix_Music*> music_list;

	std::unordered_map<int, Mix_Chunk*> sound_effects;

	AudioManager() {

		if (SDL_Init(SDL_INIT_AUDIO) < 0) {
			fprintf(stderr, "Failed to initialize SDL Audio");
			return;
		}
		if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
			fprintf(stderr, "Failed to open audio device");
			return;
		}

		Mix_AllocateChannels(5);

		//Audio references
		Mix_Music* background_music = Mix_LoadMUS(audio_path("background-music.wav").c_str());
		music_list.insert({ IN_GAME_BACKGROUND, background_music });

		Mix_Chunk* cat_scream = Mix_LoadWAV(audio_path("cat_scream.wav").c_str());
		sound_effects.insert({ CAT_SCREAM, cat_scream });

		Mix_Chunk* gunshot = Mix_LoadWAV(audio_path("gunshot.wav").c_str());
		sound_effects.insert({ GUNSHOT, gunshot });

		Mix_Chunk* win = Mix_LoadWAV(audio_path("win.wav").c_str());
		sound_effects.insert({ WIN, win });

		if (background_music == nullptr) {
			fprintf(stderr, "Failed to load sounds\n %s\n %s\n %s\n make sure the data directory is present",
				audio_path("background-music.wav").c_str());
			return;
		}

		Mix_VolumeMusic(MIX_MAX_VOLUME / 12);
	}

	~AudioManager() {
		// Destroy music components
		if (music_list.at(IN_GAME_BACKGROUND) != nullptr)
			Mix_FreeMusic(music_list.at(IN_GAME_BACKGROUND));
		Mix_CloseAudio();
	}

	void play_music(MUSIC_LIST music) {
		Mix_PlayMusic(music_list.at(music), -1);
		fprintf(stderr, "Loaded music\n");
	}

	void play_sfx(SOUND_EFFECTS sound) {
		Mix_PlayChannel(-1, sound_effects.at(sound), 0);
	}

};

extern AudioManager audio;
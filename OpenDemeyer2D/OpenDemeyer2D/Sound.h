#pragma once
#include <SDL_mixer.h>
#include <unordered_map>

class Sound final
{
	friend class ResourceManager;
	friend class Engine;

public:
	Sound(Mix_Chunk* pSound)
		: m_Sound{ pSound }
	{
	}

	Sound(const Sound&) = delete;
	Sound(Sound&&) = delete;
	Sound& operator=(const Sound&) = delete;
	Sound& operator=(Sound&&) = delete;

	~Sound()
	{
		if (m_Sound)
			Mix_FreeChunk(m_Sound);
	}

	void Pause() const
	{
		if (IsChannelValid())
			Mix_Pause(m_channel);
	}

	void Play(int loops = -1, int fadeInTimeMs = 0, int playTimeMs = -1)
	{
		m_channel = Mix_FadeInChannelTimed(-1, m_Sound, loops, fadeInTimeMs, playTimeMs);
		PlayingSounds[m_channel] = this;
	}

	void Resume() const
	{
		if (IsChannelValid())
			Mix_Resume(m_channel);
	}

	void Stop(int fadeOutTimeMs = 0) const
	{
		if (IsChannelValid())
			Mix_FadeOutChannel(m_channel, fadeOutTimeMs);
	}

	bool IsPlaying() const
	{
		if (!IsChannelValid()) return false;
		return Mix_Playing(m_channel);
	}

	float GetVolume() const
	{
		return m_volume;
	}

	void SetVolume(float volume)
	{
		m_volume = volume;
		if (IsChannelValid())
			Mix_Volume(m_channel, int(m_volume * MIX_MAX_VOLUME));
	}

	bool IsChannelValid() const
	{
		return m_channel != -1;
	}

	const std::string& GetFilePath() const { return m_sourceFile; }


private:

	Mix_Chunk* m_Sound{};
	int m_channel{ -1 };
	float m_volume{ 1.f };

	std::string m_sourceFile{};

	inline static std::unordered_map<int, Sound*> PlayingSounds{};

	static void ChannelFinishedCallback(int channel)
	{
		if (PlayingSounds[channel])
			PlayingSounds[channel]->m_channel = -1;
	}

};

class Music final
{
	friend class ResourceManager;
public:

	Music(Mix_Music* pMusic)
		: m_Music{ pMusic }
	{}

	Music(const Music&) = delete;
	Music(Music&&) = delete;
	Music& operator=(const Music&) = delete;
	Music& operator=(Music&&) = delete;

	~Music()
	{
		if (m_Music)
			Mix_FreeMusic(m_Music);
	}

	/**
	* Plays the Music
	* @param loops: times the sound loops, -1 is forever
	* @param fadeInTimeMs: time it takes for the music to fade in, 0 if no fade in
	* @param position: The position from where the music should start playing
	*/
	void PlayMusic(int loops = -1, int fadeInTimeMs = 0, double position = 0) const
	{
		CurrentlyPlayingMusic = m_Music;
		Mix_FadeInMusicPos(m_Music, loops, fadeInTimeMs, position);
		Mix_VolumeMusic(int(m_volume * MIX_MAX_VOLUME));
	}

	/**
	* Sets the volume of the music track
	*/
	void SetMusicVolume(float volume)
	{
		if (IsPlayingMusic())
			Mix_VolumeMusic(int(volume * MIX_MAX_VOLUME));
		m_volume = volume;
	}

	float GetMusicVolume() const
	{
		return m_volume;
	}

	void PauseMusic() const
	{
		if (IsPlayingMusic())
			Mix_PauseMusic();
	}

	void ResumeMusic() const
	{
		if (IsPlayingMusic())
			Mix_ResumeMusic();
	}

	void RewindMusic() const
	{
		if (IsPlayingMusic())
			Mix_RewindMusic();
	}

	void StopMusic(int fadeOutTimeMs = 0) const
	{
		if (IsPlayingMusic())
			Mix_FadeOutMusic(fadeOutTimeMs);
	}

	bool IsPlayingMusic() const
	{
		return m_Music == CurrentlyPlayingMusic;
	}

	const std::string& GetFilePath() const { return m_sourceFile; }


private:

	Mix_Music* m_Music{};
	float m_volume{1.f};

	std::string m_sourceFile{};
	
	inline static Mix_Music* CurrentlyPlayingMusic{};

};
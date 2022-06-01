#pragma once
#include <SDL_mixer.h>
#include <filesystem>

class Sound final
{
	friend class ResourceManager;
	friend class Engine;

public:
	Sound() = default;

	Sound(Mix_Chunk* pSound)
		: m_Sound{ pSound }
	{
	}

	Sound(const Sound&) = delete;
	Sound& operator=(const Sound&) = delete;

	Sound(Sound&& other) noexcept
		: m_Sound{ other.m_Sound }
		, m_channel{ other.m_channel }
		, m_volume{ other.m_volume }
		, m_sourceFile{ std::move(other.m_sourceFile) }
	{
		other.m_Sound = nullptr;
	}

	Sound& operator=(Sound&& other) noexcept
	{
		m_Sound = other.m_Sound;
		other.m_Sound = nullptr;
		m_channel = other.m_channel;
		m_volume = other.m_volume;
		m_sourceFile = std::move(other.m_sourceFile);
	}

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
		Mix_Volume(m_channel, int(m_volume * MIX_MAX_VOLUME));
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

	const std::filesystem::path& GetFilePath() const { return m_sourceFile; }

	inline bool IsValid() { return m_Sound; }
	inline operator bool() { return IsValid(); }

private:

	Mix_Chunk* m_Sound{};
	int m_channel{ -1 };
	float m_volume{ 1.f };

	std::filesystem::path m_sourceFile{};

};

class Music final
{
	friend class ResourceManager;
public:

	Music() = default;

	Music(Mix_Music* pMusic)
		: m_Music{ pMusic }
	{}

	Music(const Music&) = delete;
	Music& operator=(const Music&) = delete;

	Music(Music&& other) noexcept
		: m_Music{ other.m_Music }
		, m_volume{ other.m_volume }
		, m_sourceFile{ std::move(other.m_sourceFile) }
	{
		other.m_Music = nullptr;
	}

	Music& operator=(Music&& other) noexcept
	{
		m_Music = other.m_Music;
		other.m_Music = nullptr;
		m_sourceFile = std::move(other.m_sourceFile);
		m_volume = other.m_volume;
		return *this;
	}

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
		if (m_Music)
		{
			CurrentlyPlayingMusic = m_Music;
			Mix_FadeInMusicPos(m_Music, loops, fadeInTimeMs, position);
			Mix_VolumeMusic(int(m_volume * MIX_MAX_VOLUME));
		}
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

	const std::filesystem::path& GetFilePath() const { return m_sourceFile; }

	inline bool IsValid() { return m_Music; }
	inline operator bool() { return IsValid(); }

private:

	Mix_Music* m_Music{};
	float m_volume{1.f};

	std::filesystem::path m_sourceFile{};
	
	inline static Mix_Music* CurrentlyPlayingMusic{};

};
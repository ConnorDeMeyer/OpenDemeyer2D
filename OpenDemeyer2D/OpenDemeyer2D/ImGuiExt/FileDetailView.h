#pragma once

#include <memory>
#include <filesystem>

class Texture2D;
class Sound;
class Music;
class GameObject;
class Scene;
class Prefab;

class FileDetailView
{
public:

	FileDetailView(const std::filesystem::path& path)
		: m_Path{ path }
	{}
	virtual ~FileDetailView() = default;

	virtual void RenderDetails() = 0;
	virtual constexpr std::string_view GetFileClass() = 0;
	virtual void* GetPackage() = 0;
	virtual size_t GetPackageSize() = 0;

	std::string GetFileName() const
	{
		return m_Path.filename().string();
	}

	const std::filesystem::path& GetPath() const
	{
		return m_Path;
	}

	bool RenderImGuiFileName();

	static FileDetailView* FileDetailFactory(const std::filesystem::path& path);

protected:

	std::filesystem::path m_Path;
};

class EmptyFileDetailView final : public FileDetailView
{
public:
	EmptyFileDetailView(const std::filesystem::path& path)
		: FileDetailView(path)
	{}

	virtual ~EmptyFileDetailView() = default;

	void RenderDetails() override {}
	constexpr std::string_view GetFileClass() override { return ""; }
	void* GetPackage() override { return nullptr; }
	size_t GetPackageSize() override { return 0; }
};

class ImageDetailView final : public FileDetailView
{
public:

	ImageDetailView(const std::filesystem::path& path, const std::shared_ptr<Texture2D>& texture)
		: FileDetailView(path)
		, m_Texture{ texture }
	{}

	virtual ~ImageDetailView() = default;

	void RenderDetails() override;
	constexpr std::string_view GetFileClass() override;
	void* GetPackage() override;
	size_t GetPackageSize() override;

private:

	std::shared_ptr<Texture2D> m_Texture;
};

class MusicDetailView final : public FileDetailView
{
public:

	MusicDetailView(const std::filesystem::path& path, const std::shared_ptr<Music>& music)
		: FileDetailView(path)
		, m_Music{ music }
	{}

	virtual ~MusicDetailView() = default;

	void RenderDetails() override;
	constexpr std::string_view GetFileClass() override;
	void* GetPackage() override;
	size_t GetPackageSize() override;

private:

	std::shared_ptr<Music> m_Music;

};

class SoundDetailView final : public FileDetailView
{
public:

	SoundDetailView(const std::filesystem::path& path, const std::shared_ptr<Sound>& sound)
		: FileDetailView(path)
		, m_Sound{ sound }
	{}

	virtual ~SoundDetailView() = default;

	void RenderDetails() override;
	constexpr std::string_view GetFileClass() override;
	void* GetPackage() override;
	size_t GetPackageSize() override;

private:

	std::shared_ptr<Sound> m_Sound;

};

class PrefabDetailView final : public FileDetailView
{
public:

	PrefabDetailView(const std::filesystem::path& path, const std::shared_ptr<Prefab>& prefab)
		: FileDetailView{ path }
		, m_Prefab{ prefab }
	{}

	virtual ~PrefabDetailView() = default;

	void RenderDetails() override;
	constexpr std::string_view GetFileClass() override;
	void* GetPackage() override;
	size_t GetPackageSize() override;

private:

	std::shared_ptr<Prefab> m_Prefab;

};

class SceneDetailView final : public FileDetailView
{
public:

	SceneDetailView(const std::filesystem::path& path);

	virtual ~SceneDetailView() = default;

	void RenderDetails() override;
	constexpr std::string_view GetFileClass() override;
	void* GetPackage() override;
	size_t GetPackageSize() override;

private:
	

};

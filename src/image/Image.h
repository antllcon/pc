#pragma once

#include <stdexcept>
#include <string>

class Image
{
public:
	explicit Image(const std::string& path);
	~Image();

	Image(const Image&) = delete;
	Image& operator=(const Image&) = delete;
	Image(Image&& other) noexcept;
	Image& operator=(Image&& other) noexcept;

	int GetWidth() const;
	int GetHeight() const;
	int GetChannels() const;

	const unsigned char* GetData() const;
	unsigned char* GetData();

private:
	void Load(const std::string& path);
	void Free() noexcept;

private:
	int m_width = 0;
	int m_height = 0;
	int m_channels = 0;
	unsigned char* m_data = nullptr;
};
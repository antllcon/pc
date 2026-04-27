#pragma once

#include <ostream>
#include <string>

class Image
{
public:
	struct View
	{
		const Image& image;
		unsigned int count;
	};

	explicit Image(const std::string& path);
	~Image();

	Image(const Image&) = delete;
	Image& operator=(const Image&) = delete;
	Image(Image&& other) noexcept;
	Image& operator=(Image&& other) noexcept;

	unsigned int GetWidth() const;
	unsigned int GetHeight() const;
	unsigned int GetChannels() const;

	const unsigned char* GetData() const;
	unsigned char* GetData();

	View operator()(unsigned int count) const;

private:
	void Load(const std::string& path);
	void Free() noexcept;

private:
	unsigned int m_width = 0;
	unsigned int m_height = 0;
	unsigned int m_channels = 0;
	unsigned char* m_data = nullptr;
};

std::ostream& operator<<(std::ostream& os, const Image::View& view);
std::ostream& operator<<(std::ostream& os, const Image& image);

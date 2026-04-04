#include "TrackParser.h"

#include <fstream>
#include <regex>

namespace
{
constexpr std::string_view END_MARKER = "END";
constexpr std::string_view WHITESPACE_CHARS = " \t\r\n";
constexpr std::string_view DECAY_MARKER = "-";
constexpr std::string_view NOTE_REGEX_PATTERN = R"(^([A-G])(#?)([0-8])([P\\W]?)(-?)$)";

constexpr char CHANNEL_SEPARATOR = '|';
constexpr char WAVEFORM_PULSE = 'P';
constexpr char WAVEFORM_SAWTOOTH = '\\';
constexpr char WAVEFORM_TRIANGLE = 'W';

constexpr double A4_FREQUENCY = 440.0;
constexpr int MIDI_A4_NOTE = 69;
constexpr double NOTES_IN_OCTAVE = 12.0;
constexpr int MIDI_OCTAVE_MULTIPLIER = 12;

void AssertFileExists(const std::filesystem::path& filePath)
{
	if (!std::filesystem::exists(filePath))
	{
		throw std::runtime_error("Указанный файл не найден");
	}
}

void AssertFileOpened(const std::ifstream& stream)
{
	if (!stream.is_open())
	{
		throw std::runtime_error("Не удалось открыть файл для чтения");
	}
}

void AssertIsValidTempo(const int tempo)
{
	if (tempo <= 0)
	{
		throw std::runtime_error("Темп должен быть положительным числом");
	}
}

void AssertRegexMatch(const bool matched)
{
	if (!matched)
	{
		throw std::runtime_error("Строка ноты не соответствует ожидаемому формату");
	}
}

std::string_view TrimString(std::string_view input)
{
	const auto first = input.find_first_not_of(WHITESPACE_CHARS);
	if (first == std::string_view::npos)
	{
		return {};
	}

	input.remove_prefix(first);
	const auto last = input.find_last_not_of(WHITESPACE_CHARS);
	input.remove_suffix(input.size() - (last + 1));

	return input;
}

int ParseInteger(const std::string_view str)
{
	int value = 0;
	const auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);

	if (ec != std::errc())
	{
		throw std::runtime_error("Ошибка парсинга числового значения");
	}

	return value;
}

std::vector<std::string_view> SplitChannels(std::string_view line)
{
	std::vector<std::string_view> channels;

	size_t start = 0;
	size_t end = line.find(CHANNEL_SEPARATOR);

	while (end != std::string_view::npos)
	{
		channels.push_back(TrimString(line.substr(start, end - start)));
		start = end + 1;
		end = line.find(CHANNEL_SEPARATOR, start);
	}
	channels.push_back(TrimString(line.substr(start)));

	return channels;
}

int GetNoteOffset(const char note)
{
	switch (note)
	{
	case 'C':
		return 0;
	case 'D':
		return 2;
	case 'E':
		return 4;
	case 'F':
		return 5;
	case 'G':
		return 7;
	case 'A':
		return 9;
	case 'B':
		return 11;
	default:
		throw std::runtime_error("Неизвестная базовая нота");
	}
}

double CalculateFrequency(const char baseNote, const bool isSharp, const int octave)
{
	int noteOffset = GetNoteOffset(baseNote);
	if (isSharp)
	{
		noteOffset += 1;
	}

	const int midiNote = (octave + 1) * MIDI_OCTAVE_MULTIPLIER + noteOffset;
	return std::pow(2.0, (midiNote - MIDI_A4_NOTE) / NOTES_IN_OCTAVE) * A4_FREQUENCY;
}

WaveformType DetermineWaveform(const char waveformType)
{
	switch (waveformType)
	{
	case WAVEFORM_PULSE:
		return WaveformType::Pulse;
	case WAVEFORM_SAWTOOTH:
		return WaveformType::Sawtooth;
	case WAVEFORM_TRIANGLE:
		return WaveformType::Triangle;
	default:
		return WaveformType::Sine;
	}
}

Note ParseNoteToken(const std::string_view token)
{
	Note note{};
	note.Frequency = 0.0;
	note.Waveform = WaveformType::Sine;
	note.HasDecay = false;
	note.IsRest = true;

	if (token.empty())
	{
		return note;
	}

	if (token == DECAY_MARKER)
	{
		note.HasDecay = true;
		return note;
	}

	static const std::regex noteRegex(NOTE_REGEX_PATTERN.data());
	std::match_results<std::string_view::const_iterator> match;

	AssertRegexMatch(std::regex_match(token.cbegin(), token.cend(), match, noteRegex));

	const char baseNote = *match[1].first;
	const bool isSharp = match[2].matched;

	const std::string_view octaveStr(&*match[3].first, match[3].length());
	const int octave = ParseInteger(octaveStr);

	note.Frequency = CalculateFrequency(baseNote, isSharp, octave);

	if (match[4].matched)
	{
		note.Waveform = DetermineWaveform(*match[4].first);
	}

	note.HasDecay = match[5].matched;
	note.IsRest = false;

	return note;
}

void EnsureChannelCount(MusicComposition& track, const size_t count)
{
	while (track.Channels.size() < count)
	{
		track.Channels.emplace_back();
	}
}
} // namespace

MusicComposition TrackParser::Parse(const std::filesystem::path& filePath)
{
	AssertFileExists(filePath);

	std::ifstream file(filePath);
	AssertFileOpened(file);

	MusicComposition composition;
	std::string line;

	if (std::getline(file, line))
	{
		const auto rawTempo = ParseInteger(TrimString(line));
		AssertIsValidTempo(rawTempo);
		composition.Tempo = rawTempo;
	}

	while (std::getline(file, line))
	{
		const auto trimmedLine = TrimString(line);

		if (trimmedLine == END_MARKER)
		{
			break;
		}

		if (trimmedLine.empty())
		{
			continue;
		}

		auto channelTokens = SplitChannels(trimmedLine);
		EnsureChannelCount(composition, channelTokens.size());

		for (size_t i = 0; i < channelTokens.size(); ++i)
		{
			Note parsedNote = ParseNoteToken(channelTokens[i]);
			composition.Channels[i].Notes.push_back(parsedNote);
		}
	}

	return composition;
}
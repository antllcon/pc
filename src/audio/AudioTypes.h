#pragma once

#include <vector>

constexpr unsigned int STANDARD_SAMPLE_RATE = 44100;

enum class WaveformType
{
	Sine,
	Pulse,
	Sawtooth,
	Triangle
};

struct Note
{
	double Frequency;
	WaveformType Waveform;
	bool HasDecay;
	bool IsRest;
};

struct Channel
{
	std::vector<Note> Notes;
};

struct MusicComposition
{
	uint32_t Tempo;
	std::vector<Channel> Channels;
};
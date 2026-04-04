#include "Synthesizer.h"

#include <future>
#include <numbers>
#include <stdexcept>

namespace
{
constexpr double MAX_AMPLITUDE = 1.0;
constexpr double TWO_PI = 2.0 * std::numbers::pi;
constexpr double ATTACK_TIME_SECONDS = 0.01;
constexpr double RELEASE_TIME_SECONDS = 0.01;

void AssertHasChannels(const std::vector<Channel>& channels)
{
	if (channels.empty())
	{
		throw std::runtime_error("Композиция не содержит каналов для воспроизведения");
	}
}

double GenerateSine(const double frequency, const double time)
{
	return std::sin(TWO_PI * frequency * time);
}

double GeneratePulse(const double frequency, const double time)
{
	return GenerateSine(frequency, time) > 0.0 ? 1.0 : -1.0;
}

double GenerateSawtooth(const double frequency, const double time)
{
	const double phase = frequency * time;
	return 2.0 * (phase - std::floor(phase + 0.5));
}

double GenerateTriangle(const double frequency, const double time)
{
	return 2.0 * std::abs(GenerateSawtooth(frequency, time)) - 1.0;
}

double CalculateEnvelope(const double time, const double duration, const bool hasDecay)
{
	double amplitude = 1.0;

	if (time < ATTACK_TIME_SECONDS)
	{
		amplitude = time / ATTACK_TIME_SECONDS;
	}

	if (hasDecay)
	{
		const double decayFactor = 1.0 - time / duration;
		amplitude *= std::max(0.0, decayFactor);
	}
	else if (time > duration - RELEASE_TIME_SECONDS)
	{
		const double timeRemaining = duration - time;
		amplitude *= std::max(0.0, timeRemaining / RELEASE_TIME_SECONDS);
	}

	return amplitude;
}

double GenerateSample(const Note& note, const double time, const double duration)
{
	if (note.IsRest)
	{
		return 0.0;
	}

	double rawSample = 0.0;

	switch (note.Waveform)
	{
	case WaveformType::Sine:
		rawSample = GenerateSine(note.Frequency, time);
		break;
	case WaveformType::Pulse:
		rawSample = GeneratePulse(note.Frequency, time);
		break;
	case WaveformType::Sawtooth:
		rawSample = GenerateSawtooth(note.Frequency, time);
		break;
	case WaveformType::Triangle:
		rawSample = GenerateTriangle(note.Frequency, time);
		break;
	}

	const double envelope = CalculateEnvelope(time, duration, note.HasDecay);
	return rawSample * envelope;
}

double CalculateRowDuration(const uint32_t tempo)
{
	return 60.0 / static_cast<double>(tempo);
}

std::vector<double> RenderChannel(const Channel& channel, const double rowDuration)
{
	std::vector<double> buffer;
	const auto samplesPerRow = static_cast<size_t>(rowDuration * STANDARD_SAMPLE_RATE);
	buffer.reserve(channel.Notes.size() * samplesPerRow);

	for (const auto& note : channel.Notes)
	{
		for (size_t i = 0; i < samplesPerRow; ++i)
		{
			const double time = static_cast<double>(i) / STANDARD_SAMPLE_RATE;
			const double sample = GenerateSample(note, time, rowDuration);

			buffer.push_back(sample);
		}
	}

	return buffer;
}

std::vector<double> MixBuffers(const std::vector<std::vector<double>>& buffers)
{
	if (buffers.empty())
	{
		return {};
	}

	size_t maxLength = 0;
	for (const auto& buffer : buffers)
	{
		maxLength = std::max(maxLength, buffer.size());
	}

	std::vector<double> mixed(maxLength, 0.0);

	for (const auto& buffer : buffers)
	{
		for (size_t i = 0; i < buffer.size(); ++i)
		{
			mixed[i] += buffer[i];
		}
	}

	double peak = 0.0;
	for (const double sample : mixed)
	{
		peak = std::max(peak, std::abs(sample));
	}

	if (peak > MAX_AMPLITUDE)
	{
		for (double& sample : mixed)
		{
			sample /= peak;
		}
	}

	return mixed;
}
} // namespace

std::vector<double> Synthesizer::Render(const MusicComposition& composition)
{
	AssertHasChannels(composition.Channels);

	const double rowDuration = CalculateRowDuration(composition.Tempo);
	std::vector<std::future<std::vector<double>>> futures;

	for (const auto& channel : composition.Channels)
	{
		futures.push_back(std::async(std::launch::async, [&channel, rowDuration] {
			return RenderChannel(channel, rowDuration);
		}));
	}

	std::vector<std::vector<double>> channelBuffers;
	channelBuffers.reserve(futures.size());

	for (auto& future : futures)
	{
		channelBuffers.push_back(future.get());
	}

	return MixBuffers(channelBuffers);
}
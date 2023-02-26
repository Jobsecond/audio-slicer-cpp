#include <vector>
#include <tuple>
#include <cmath>
#include <stdexcept>
#include <algorithm>
#include <iostream>

#include "slicer.h"

template<class T>
inline T divIntRound(T n, T d);

template<class T>
inline uint64_t argmin_range_view(const std::vector<T>& v, uint64_t begin, uint64_t end);

template<class T>
inline std::vector<T> multichannel_to_mono(const std::vector<T>& v, unsigned int channels);

template<class T>
inline std::vector<double> get_rms(const std::vector<T>& arr, uint64_t frame_length = 2048, uint64_t hop_length = 512);


Slicer::Slicer(int sr, double threshold, uint64_t min_length, uint64_t min_interval, uint64_t hop_size, uint64_t max_sil_kept)
{
    this->threshold = std::pow(10, threshold / 20.0);
    this->hop_size = divIntRound<uint64_t>(hop_size * (uint64_t)sr, (uint64_t)1000);
    this->win_size = std::min(divIntRound<uint64_t>(min_interval * (uint64_t)sr, (uint64_t)1000), (uint64_t)4 * this->hop_size);
    this->min_length = divIntRound<uint64_t>(min_length * (uint64_t)sr, (uint64_t)1000 * this->hop_size);
    this->min_interval = divIntRound<uint64_t>(min_interval * (uint64_t)sr, (uint64_t)1000 * this->hop_size);
    this->max_sil_kept = divIntRound<uint64_t>(max_sil_kept * (uint64_t)sr, (uint64_t)1000 * this->hop_size);
}

std::vector<std::tuple<uint64_t, uint64_t>>
Slicer::slice(const std::vector<float>& waveform, unsigned int channels)
{
    if ((!((min_length >= min_interval) && (min_interval >= hop_size))) || (max_sil_kept < hop_size))
    {
        // The following condition must be satisfied: min_length >= min_interval >= hop_size
        // The following condition must be satisfied: max_sil_kept >= hop_size
        return {};
    }

    uint64_t frames = waveform.size() / channels;
    std::vector<float> samples = multichannel_to_mono<float>(waveform, channels);

    if (samples.size() <= this->min_length)
    {
        std::vector<std::tuple<uint64_t, uint64_t>> v {{ 0, frames }};
        return v;
    }

    std::vector<double> rms_list = get_rms<float>(samples, (uint64_t) this->win_size, (uint64_t) this->hop_size);

    std::vector<std::tuple<uint64_t, uint64_t>> sil_tags;
    uint64_t silence_start = 0;
    bool has_silence_start = false;
    uint64_t clip_start = 0;

    uint64_t pos = 0, pos_l = 0, pos_r = 0;

    for (uint64_t i = 0; i < rms_list.size(); i++)
    {
        double rms = rms_list[i];
        // Keep looping while frame is silent.
        if (rms < this->threshold)
        {
            // Record start of silent frames.
            if (!has_silence_start)
            {
                silence_start = i;
                has_silence_start = true;
            }
            continue;
        }
        // Keep looping while frame is not silent and silence start has not been recorded.
        if (!has_silence_start)
        {
            continue;
        }
        // Clear recorded silence start if interval is not enough or clip is too short
        bool is_leading_silence = ((silence_start == 0) && (i > this->max_sil_kept));
        bool need_slice_middle = (
                ( (i - silence_start) >= this->min_interval) &&
                ( (i - clip_start) >= this->min_length) );
        if ((!is_leading_silence) && (!need_slice_middle))
        {
            has_silence_start = false;
            continue;
        }

        // Need slicing. Record the range of silent frames to be removed.
        if ((i - silence_start) <= this->max_sil_kept)
        {
            pos = argmin_range_view<double>(rms_list, silence_start, i + 1) + silence_start;
            if (silence_start == 0)
            {
                sil_tags.emplace_back(0, pos);
            }
            else
            {
                sil_tags.emplace_back(pos, pos);
            }
            clip_start = pos;
        }
        else if ((i - silence_start) <= (this->max_sil_kept * 2))
        {
            pos = argmin_range_view<double>(rms_list, i - this->max_sil_kept, silence_start + this->max_sil_kept + 1);
            pos += i - this->max_sil_kept;
            pos_l = argmin_range_view<double>(rms_list, silence_start, silence_start + this->max_sil_kept + 1) + silence_start;
            pos_r = argmin_range_view<double>(rms_list, i - this->max_sil_kept, i + 1) + i - this->max_sil_kept;
            if (silence_start == 0)
            {
                clip_start = pos_r;
                sil_tags.emplace_back(0, clip_start);
            }
            else
            {
                clip_start = std::max(pos_r, pos);
                sil_tags.emplace_back(std::min(pos_l, pos), clip_start);
            }
        }
        else
        {
            pos_l = argmin_range_view<double>(rms_list, silence_start, silence_start + this->max_sil_kept + 1) + silence_start;
            pos_r = argmin_range_view<double>(rms_list, i - this->max_sil_kept, i + 1) + i - this->max_sil_kept;
            if (silence_start == 0)
            {
                sil_tags.emplace_back(0, pos_r);
            }
            else
            {
                sil_tags.emplace_back(pos_l, pos_r);
            }
            clip_start = pos_r;
        }
        has_silence_start = false;
    }
    // Deal with trailing silence.
    uint64_t total_frames = rms_list.size();
    if (has_silence_start && ((total_frames - silence_start) >= this->min_interval))
    {
        uint64_t silence_end = std::min(total_frames - 1, silence_start + this->max_sil_kept);
        pos = argmin_range_view<double>(rms_list, silence_start, silence_end + 1) + silence_start;
        sil_tags.emplace_back(pos, total_frames + 1);
    }
    // Apply and return slices.
    if (sil_tags.empty())
    {
        std::vector<std::tuple<uint64_t, uint64_t>> v {{ 0, frames }};
        return v;
    }
    else
    {
        std::vector<std::tuple<uint64_t, uint64_t>> chunks;
        uint64_t begin = 0, end = 0;
        uint64_t s0 = std::get<0>(sil_tags[0]);
        if (s0 > 0)
        {
            begin = 0;
            end = s0;
            chunks.emplace_back(begin * this->hop_size, std::min(frames, end * this->hop_size));
        }
        for (int i = 0; i < sil_tags.size() - 1; i++)
        {
            begin = std::get<1>(sil_tags[i]);
            end = std::get<0>(sil_tags[i + 1]);
            chunks.emplace_back(begin * this->hop_size, std::min(frames, end * this->hop_size));
        }
        if (std::get<1>(sil_tags.back()) < total_frames)
        {
            begin = std::get<1>(sil_tags.back());
            end = total_frames;
            chunks.emplace_back(begin * this->hop_size, std::min(frames, end * this->hop_size));
        }
        return chunks;
    }
}

template<class T>
inline std::vector<double> get_rms(const std::vector<T>& arr, uint64_t frame_length, uint64_t hop_length)
{
    uint64_t arr_length = arr.size();

    uint64_t padding = frame_length / 2;

    uint64_t rms_size = arr_length / hop_length + 1;

    std::vector<double> rms = std::vector<double>(rms_size);

    uint64_t left = 0;
    uint64_t right = 0;
    uint64_t hop_count = 0;

    uint64_t rms_index = 0;
    double val = 0;

    // Initial condition: the frame is at the beginning of padded array
    while ((right < padding) && (right < arr_length))
    {
        val += (double)arr[right] * arr[right];
        right++;
    }
    rms[rms_index++] = (std::sqrt(std::max(0.0, (double)val / (double)frame_length)));

    // Left side or right side of the frame has not touched the sides of original array
    while ((right < frame_length) && (right < arr_length) && (rms_index < rms_size))
    {
        val += (double)arr[right] * arr[right];
        hop_count++;
        if (hop_count == hop_length)
        {
            rms[rms_index++] = (std::sqrt(std::max(0.0, (double)val / (double)frame_length)));
            hop_count = 0;
        }
        right++;  // Move right 1 step at a time.
    }

    if (frame_length < arr_length)
    {
        while ((right < arr_length) && (rms_index < rms_size))
        {
            val += (double)arr[right] * arr[right] - (double)arr[left] * arr[left];
            hop_count++;
            if (hop_count == hop_length)
            {
                rms[rms_index++] = (std::sqrt(std::max(0.0, (double)val / (double)frame_length)));
                hop_count = 0;
            }
            left++;
            right++;
        }
    }
    else
    {
        while ((right < frame_length) && (rms_index < rms_size))
        {
            hop_count++;
            if (hop_count == hop_length)
            {
                rms[rms_index++] = (std::sqrt(std::max(0.0, (double)val / (double)frame_length)));
                hop_count = 0;
            }
            right++;
        }
    }

    while ((left < arr_length) && (rms_index < rms_size)/* && (right < arr_length + padding)*/)
    {
        val -= (double)arr[left] * arr[left];
        hop_count++;
        if (hop_count == hop_length)
        {
            rms[rms_index++] = (std::sqrt(std::max(0.0, (double)val / (double)frame_length)));
            hop_count = 0;
        }
        left++;
        right++;
    }

    return rms;
}

template<class T>
inline T divIntRound(T n, T d)
{
    /*
     * Integer division rounding to the closest integer, without converting to floating point numbers.
     */
    // T should be an integer type (int, int64_t, uint64_t, ...)
    return ((n < 0) ^ (d < 0)) ? \
        ((n - (d / 2)) / d) : \
        ((n + (d / 2)) / d);
}

template<class T>
inline uint64_t argmin_range_view(const std::vector<T>& v, uint64_t begin, uint64_t end)
{
    // Ensure vector access is not out of bound
    auto size = v.size();
    if (begin > size)  begin = size;
    if (end > size)    end = size;
    if (begin >= end)  return 0;

    auto min_it = std::min_element(v.begin() + begin, v.begin() + end);
    return std::distance(v.begin() + begin, min_it);
}

template<class T>
inline std::vector<T> multichannel_to_mono(const std::vector<T>& v, unsigned int channels)
{
    uint64_t frames = v.size() / channels;
    std::vector<T> out(frames);

    for (uint64_t i = 0; i < frames; i++)
    {
        T s = 0;
        for (unsigned int j = 0; j < channels; j++)
        {
            s += (T)v[i * channels + j] / (T)channels;
        }
        out[i] = s;
    }

    return out;
}
#include <vector>
#include <tuple>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <fstream>

#include <xtensor/xview.hpp>
#include <xtensor/xsort.hpp>

#include "slicer.h"

template<class T>
inline T divIntRound(T n, T d);

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

Slicer::Slicer(int sr, double threshold, int64_t min_length, int64_t min_interval, int64_t hop_size, int64_t max_sil_kept)
{
    if (!((min_length >= min_interval) && (min_interval >= hop_size)))
    {
        throw std::invalid_argument("The following condition must be satisfied: min_length >= min_interval >= hop_size");
    }
    if (max_sil_kept < hop_size)
    {
        throw std::invalid_argument("The following condition must be satisfied: max_sil_kept >= hop_size");
    }

    this->threshold = std::pow(10, threshold / 20.0);
    this->hop_size = divIntRound<int64_t>(hop_size * sr, 1000);
    this->win_size = std::min(divIntRound<int64_t>(min_interval * sr, 1000), 4 * this->hop_size);
    this->min_length = divIntRound<int64_t>(min_length * sr, 1000 * this->hop_size);
    this->min_interval = divIntRound<int64_t>(min_interval * sr, 1000 * this->hop_size);
    this->max_sil_kept = divIntRound<int64_t>(max_sil_kept * sr, 1000 * this->hop_size);
}

std::vector<xt::xarray<float>>
Slicer::slice(const xt::xarray<float>& waveform)
{
    xt::xarray<float> samples = xt::mean(waveform, 1);

    if (samples.shape(0) <= this->min_length)
    {
        std::vector<xt::xarray<float>> v {waveform};
        return v;
    }

    xt::xarray<float> rms_list = get_rms(samples, (std::size_t) this->win_size, (std::size_t) this->hop_size);

    std::vector<std::tuple<int64_t, int64_t>> sil_tags;
    int64_t silence_start = 0;
    bool has_silence_start = false;
    int64_t clip_start = 0;

    int64_t pos, pos_l, pos_r;

    for (int64_t i = 0; i < rms_list.size(); i++)
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
            pos = xt::argmin(
                    xt::view(rms_list, xt::range(silence_start, i + 1))
            )[0] + silence_start;
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
            pos = xt::argmin(
                    xt::view(rms_list, xt::range(i - this->max_sil_kept, silence_start + this->max_sil_kept + 1))
            )[0];
            pos += i - this->max_sil_kept;
            pos_l = xt::argmin(
                    xt::view(rms_list, xt::range(silence_start, silence_start + this->max_sil_kept + 1))
            )[0] + silence_start;
            pos_r = xt::argmin(
                    xt::view(rms_list, xt::range(i - this->max_sil_kept, i + 1))
            )[0] + i - this->max_sil_kept;
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
            pos_l = xt::argmin(
                    xt::view(rms_list, xt::range(silence_start, silence_start + this->max_sil_kept + 1))
            )[0] + silence_start;
            pos_r = xt::argmin(
                    xt::view(rms_list, xt::range(i - this->max_sil_kept, i + 1))
            )[0] + i - this->max_sil_kept;
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
    int64_t total_frames = rms_list.shape(0);
    if (has_silence_start && ((total_frames - silence_start) >= this->min_interval))
    {
        int64_t silence_end = std::min(total_frames, silence_start + this->max_sil_kept);
        pos = xt::argmin(
                xt::view(rms_list, xt::range(silence_start, silence_end + 1))
        )[0] + silence_start;
        sil_tags.emplace_back(pos, total_frames + 1);
    }
    // Apply and return slices.
    if (sil_tags.empty())
    {
        std::vector<xt::xarray<float>> v {waveform};
        return v;
    }
    else
    {
        std::vector<xt::xarray<float>> chunks;
        int64_t s0 = std::get<0>(sil_tags[0]);
        if (s0 > 0)
        {
            chunks.push_back(this->_apply_slice(waveform, 0, s0));
        }
        for (int i = 0; i < sil_tags.size() - 1; i++)
        {
            chunks.push_back(this->_apply_slice(waveform, std::get<1>(sil_tags[i]), std::get<0>(sil_tags[i + 1])));
        }
        if (std::get<1>(sil_tags.back()) < total_frames)
        {
            chunks.push_back(this->_apply_slice(waveform, std::get<1>(sil_tags.back()), total_frames));
        }
        return chunks;
    }
}

xt::xarray<float>
Slicer::_apply_slice(const xt::xarray<float> &waveform, int64_t begin, int64_t end)
{
    if (waveform.shape(0) > 1)
    {
        return xt::view(waveform, xt::range(begin * this->hop_size, std::min((int64_t)waveform.shape(0), end * this->hop_size)), xt::all());
    }
    return xt::view(waveform, xt::range(begin * this->hop_size, std::min((int64_t)waveform.shape(0), end * this->hop_size)));
}


xt::xarray<double> get_rms(const xt::xarray<double>& arr, std::size_t frame_length, std::size_t hop_length)
{
    std::size_t arr_length = arr.size();

    std::size_t padding = frame_length / 2;

    std::size_t rms_size = arr_length / hop_length + 1;

    xt::xarray<double> rms = xt::zeros<double>({rms_size});

    std::size_t left = 0;
    std::size_t right = 0;
    std::size_t hop_count = 0;

    std::size_t rms_index = 0;
    double val = 0;

    // Initial condition: the frame is at the beginning of padded array
    while ((right < padding) && (right < arr_length))
    {
        val += arr[right] * arr[right];
        right++;
    }
    rms[rms_index++] = (std::sqrt(std::max(0.0, (double)val / (double)frame_length)));

    // Left side or right side of the frame has not touched the sides of original array
    while ((right < frame_length) && (right < arr_length) && (rms_index < rms_size))
    {
        val += arr[right] * arr[right];
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
            val += arr[right] * arr[right] - arr[left] * arr[left];
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
        val -= arr[left] * arr[left];
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
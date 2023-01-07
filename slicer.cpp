#include <vector>
#include <tuple>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <fstream>

#include <xtensor/xview.hpp>
#include <xtensor/xsort.hpp>

#include "slicer.h"


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
    double min_interval_d = (double)min_interval * sr / 1000;
    this->threshold = std::pow(10, (double)threshold / 20.0);
    this->hop_size = (int64_t)(std::round((double)hop_size * sr / 1000));
    this->win_size = std::min((int64_t)std::round(min_interval_d), 4 * this->hop_size);
    this->min_length = (int64_t)std::round((double)min_length * sr / 1000 / (double)(this->hop_size));
    this->min_interval = (int64_t)std::round(min_interval_d / (double)(this->hop_size));
    this->max_sil_kept = (int64_t)std::round((double)max_sil_kept * sr / 1000 / (double)(this->hop_size));
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
    int64_t silence_start = -1;
    int64_t clip_start = 0;

    int64_t pos, pos_l, pos_r;

    for (int64_t i = 0; i < rms_list.size(); i++)
    {
        double rms = rms_list[i];
        // Keep looping while frame is silent.
        if (rms < this->threshold)
        {
            // Record start of silent frames.
            if (silence_start == -1)
            {
                silence_start = i;
            }
            continue;
        }
        // Keep looping while frame is not silent and silence start has not been recorded.
        if (silence_start == -1)
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
            silence_start = -1;
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
        silence_start = -1;
    }
    // Deal with trailing silence.
    int64_t total_frames = rms_list.shape(0);
    if ((silence_start != -1) && ((total_frames - silence_start) >= this->min_interval))
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
    rms[rms_index++] = (std::sqrt(std::max(0.0, (double)val / frame_length)));

    // Left side or right side of the frame has not touched the sides of original array
    while ((right < frame_length) && (right < arr_length) && (rms_index < rms_size))
    {
        val += arr[right] * arr[right];
        hop_count++;
        if (hop_count == hop_length)
        {
            rms[rms_index++] = (std::sqrt(std::max(0.0, (double)val / frame_length)));
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
                rms[rms_index++] = (std::sqrt(std::max(0.0, (double)val / frame_length)));
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
                rms[rms_index++] = (std::sqrt(std::max(0.0, (double)val / frame_length)));
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
            rms[rms_index++] = (std::sqrt(std::max(0.0, (double)val / frame_length)));
            hop_count = 0;
        }
        left++;
        right++;
    }

    return rms;
}

//
// Created by Kevin Zhang on 1/5/23.
//

#ifndef AUDIO_SLICER_SLICER_H
#define AUDIO_SLICER_SLICER_H

#include <xtensor/xarray.hpp>

class Slicer {
private:
    double threshold;
    uint64_t hop_size;
    uint64_t win_size;
    uint64_t min_length;
    uint64_t min_interval;
    uint64_t max_sil_kept;
private:
    xt::xarray<float> _apply_slice(const xt::xarray<float>& waveform, uint64_t begin, uint64_t end);
public:
    Slicer(int sr, double threshold = -40.0, uint64_t min_length = 5000, uint64_t min_interval = 300, uint64_t hop_size = 20, uint64_t max_sil_kept = 5000);
    std::vector<xt::xarray<float>> slice(const xt::xarray<float>& waveform);
};

xt::xarray<double> get_rms(const xt::xarray<double>& arr, uint64_t frame_length = 2048, uint64_t hop_length = 512);

#endif //AUDIO_SLICER_SLICER_H
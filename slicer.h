//
// Created by Kevin Zhang on 1/5/23.
//

#ifndef AUDIO_SLICER_SLICER_H
#define AUDIO_SLICER_SLICER_H

class Slicer {
private:
    double threshold;
    int64_t hop_size;
    int64_t win_size;
    int64_t min_length;
    int64_t min_interval;
    int64_t max_sil_kept;
private:
    xt::xarray<float> _apply_slice(const xt::xarray<float>& waveform, int64_t begin, int64_t end);
public:
    Slicer(int sr, double threshold, int64_t min_length, int64_t min_interval, int64_t hop_size, int64_t max_sil_kept);
    std::vector<xt::xarray<float>> slice(const xt::xarray<float>& waveform);
};

xt::xarray<double> get_rms(const xt::xarray<double>& arr, std::size_t frame_length = 2048, std::size_t hop_length = 512);

#endif //AUDIO_SLICER_SLICER_H

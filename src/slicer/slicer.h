//
// Created by Kevin Zhang on 1/5/23.
//

#ifndef AUDIO_SLICER_SLICER_H
#define AUDIO_SLICER_SLICER_H

#include <vector>
#include <tuple>

class Slicer {
private:
    double threshold;
    uint64_t hop_size;
    uint64_t win_size;
    uint64_t min_length;
    uint64_t min_interval;
    uint64_t max_sil_kept;

public:
    Slicer(int sr, double threshold = -40.0, uint64_t min_length = 5000, uint64_t min_interval = 300, uint64_t hop_size = 20, uint64_t max_sil_kept = 5000);
    std::vector<std::tuple<uint64_t, uint64_t>> slice(const std::vector<float>& waveform, unsigned int channels);
};

#endif //AUDIO_SLICER_SLICER_H
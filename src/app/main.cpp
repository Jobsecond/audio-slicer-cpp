#include <vector>
#include <tuple>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <filesystem>

#include <argparse/argparse.hpp>

#include <sndfile.hh>

#include "slicer.h"

int main(int argc, char **argv)
{
    argparse::ArgumentParser parser("audio_slicer");

    parser.add_argument("audio")
            .help("The audio to be sliced");
    parser.add_argument("--out")
            .default_value(std::string())
            .help("Output directory of the sliced audio clips");
    parser.add_argument("--db_thresh")
            .default_value((double)(-40.0))
            .help("The dB threshold for silence detection")
            .scan<'g', double>();
    parser.add_argument("--min_length")
            .default_value((uint64_t)(5000))
            .help("The minimum milliseconds required for each sliced audio clip")
            .scan<'i', uint64_t>();
    parser.add_argument("--min_interval")
            .default_value((uint64_t)(300))
            .help("The minimum milliseconds for a silence part to be sliced")
            .scan<'i', uint64_t>();
    parser.add_argument("--hop_size")
            .default_value((uint64_t)(10))
            .help("Frame length in milliseconds")
            .scan<'i', uint64_t>();
    parser.add_argument("--max_sil_kept")
            .default_value((uint64_t)(500))
            .help("The maximum silence length kept around the sliced clip, presented in milliseconds")
            .scan<'i', uint64_t>();

    try {
        parser.parse_args(argc, argv);
    }
    catch (const std::runtime_error& err) {
        // std::cerr << err.what() << std::endl;
        std::cerr << parser;
        std::exit(1);
    }

    auto out_str = parser.get("--out");
    auto filename = parser.get("audio");
    auto db_thresh = parser.get<double>("--db_thresh");
    auto min_length = parser.get<uint64_t>("--min_length");
    auto min_interval = parser.get<uint64_t>("--min_interval");
    auto hop_size = parser.get<uint64_t>("--hop_size");
    auto max_sil_kept = parser.get<uint64_t>("--max_sil_kept");

    auto path = std::filesystem::absolute(filename);
    auto out = out_str.empty() ? path.parent_path() : std::filesystem::path(out_str);

    SndfileHandle handle(path.string().data());
    int channels = handle.channels();
    int sr = handle.samplerate();
    int format = handle.format();
    auto frames = handle.frames();

    auto total_size = frames * channels;
    std::vector<float> audio = std::vector<float>(total_size);

    handle.read(audio.data(), total_size);

    Slicer slicer(sr, db_thresh, min_length, min_interval, hop_size, max_sil_kept);
    auto chunks = slicer.slice(audio, (unsigned int)channels);

    try
    {
        if (!std::filesystem::exists(out) && !out.has_parent_path())
        {
            std::filesystem::create_directories(out.string());
        }
    }
    catch (const std::filesystem::filesystem_error& err)
    {
        std::cerr << "Cannot write to directory " << out.string() << ": " << err.what() << '\n';
        std::exit(2);
    }

    int idx = 0;
    for (auto chunk : chunks)
    {
        auto begin_frame = std::get<0>(chunk) * channels;
        auto end_frame = std::get<1>(chunk) * channels;
        auto frame_count = (sf_count_t)(end_frame - begin_frame);
        if ((begin_frame == end_frame) || (begin_frame > total_size) || (end_frame > total_size))
        {
            continue;
        }
        std::stringstream ss;
        ss << std::filesystem::path(filename).stem().string() << "_" << idx << ".wav";
        std::filesystem::path out_file_path = out / ss.str();
        SndfileHandle wf = SndfileHandle(out_file_path.string().data(), SFM_WRITE, format, channels, sr);
        wf.write(audio.data() + begin_frame, frame_count);
        idx++;
    }

    return 0;
}
#include <vector>
#include <tuple>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <fstream>

#include <argparse/argparse.hpp>

#include <sndfile.hh>

#include <xtensor/xarray.hpp>

#include "slicer.h"

int main(int argc, char **argv)
{
    argparse::ArgumentParser parser("audio_slicer");

    parser.add_argument("audio")
        .help("The audio to be sliced");
    parser.add_argument("--out")
        .help("Output directory of the sliced audio clips");
    parser.add_argument("--db_thresh")
        .default_value(-40.0)
        .help("The dB threshold for silence detection")
        .scan<'g', double>();
    parser.add_argument("--min_length")
        .default_value(5000)
        .help("The minimum milliseconds required for each sliced audio clip")
        .scan<'i', int64_t>();
    parser.add_argument("--min_interval")
        .default_value(300)
        .help("The minimum milliseconds for a silence part to be sliced")
        .scan<'i', int64_t>();
    parser.add_argument("--hop_size")
        .default_value(10)
        .help("Frame length in milliseconds")
        .scan<'i', int64_t>();
    parser.add_argument("--max_sil_kept")
            .default_value(500)
            .help("The maximum silence length kept around the sliced clip, presented in milliseconds")
            .scan<'i', int64_t>();
    try {
        parser.parse_args(argc, argv);
    }
    catch (const std::runtime_error& err) {
        // std::cerr << err.what() << std::endl;
        std::cerr << parser;
        std::exit(1);
    }

    auto out_str = parser.get("out");
    auto filename = parser.get("audio");
    auto db_thresh = parser.get<double>("db_thresh");
    auto min_length = parser.get<int64_t>("min_length");
    auto min_interval = parser.get<int64_t>("min_interval");
    auto hop_size = parser.get<int64_t>("hop_size");
    auto max_sil_kept = parser.get<int64_t>("max_sil_kept");

    auto path = std::filesystem::absolute(filename);
    auto out = out_str.empty() ? path.parent_path() : std::filesystem::path(out_str);

    SndfileHandle handle(path.string().data());
    int channels = handle.channels();
    int sr = handle.samplerate();
    int format = handle.format();
    int64_t frames = handle.frames();


    xt::xarray<float> audio = xt::zeros<float>({(size_t)frames, (size_t)channels});

    handle.read(audio.data(), frames * channels);

    Slicer slicer(sr, db_thresh, min_length, min_interval, hop_size, max_sil_kept);
    auto chunks = slicer.slice(audio);

    std::filesystem::create_directory(out);

    int idx = 0;
    for (auto chunk : chunks)
    {
        if (chunk.size() == 0)
        {
            continue;
        }
        std::stringstream ss;
        ss << std::filesystem::path(filename).stem().string() << "_" << idx << ".wav";
        std::filesystem::path out_file_path = out / ss.str();
        SndfileHandle wf = SndfileHandle(out_file_path.string().data(), SFM_WRITE, format, channels, sr);
        wf.write(chunk.data(), chunk.size());
        idx++;
    }

    return 0;
}
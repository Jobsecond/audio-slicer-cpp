#include <utility>
#include <vector>
#include <tuple>
#include <string>
#include <filesystem>
#include <sstream>

#include <sndfile.hh>

#include <QDir>

#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32)) || (defined(UNICODE) || defined(_UNICODE))
#define USE_WIDE_CHAR
#endif

#include "workthread.h"
#include "slicer.h"

WorkThread::WorkThread(
        QString filename,
        QString out_path,
        double threshold,
        uint64_t min_length,
        uint64_t min_interval,
        uint64_t hop_size,
        uint64_t max_sil_kept)
        : m_filename(std::move(filename)),
          m_out_path(std::move(out_path)),
          m_threshold(threshold),
          m_min_length(min_length),
          m_min_interval(min_interval),
          m_hop_size(hop_size),
          m_max_sil_kept(max_sil_kept)
{}

void WorkThread::run()
{
    try
    {
#ifdef USE_WIDE_CHAR
        std::wstring filename = m_filename.toStdWString();

        auto path = std::filesystem::absolute(filename);
        auto out = m_out_path.isEmpty() ? path.parent_path() : std::filesystem::path(m_out_path.toStdWString());

        SndfileHandle handle(path.wstring().c_str());
#else
        std::string filename = m_filename.toStdString();

        auto path = std::filesystem::absolute(filename);
        auto out = m_out_path.isEmpty() ? path.parent_path() : std::filesystem::path(m_out_path.toStdString());

        SndfileHandle handle(path.string().c_str());
#endif
        if (handle.error() != SF_ERR_NO_ERROR)
        {
            emit oneError(QString("Sndfile error: %1").arg(handle.strError()));
            return;
        }
        int channels = handle.channels();
        int sr = handle.samplerate();
        int format = handle.format();
        auto frames = handle.frames();

        auto total_size = frames * channels;
        std::vector<float> audio = std::vector<float>(total_size);

        auto items_read = handle.read(audio.data(), total_size);

        if (items_read == 0)
        {
            emit oneError(QString("Zero items read: %1").arg(m_filename));
            return;
        }

        Slicer slicer(sr, m_threshold, m_min_length, m_min_interval, m_hop_size, m_max_sil_kept);
        auto chunks = slicer.slice(audio, (unsigned int)channels);

        if (chunks.empty())
        {
            QString errmsg = QString("ValueError: The following conditions must be satisfied: (min_length >= min_interval >= hop_size) and (max_sil_kept >= hop_size).");
            emit oneError(errmsg);
            return;
        }

        if (!std::filesystem::exists(out))
        {
            std::filesystem::create_directories(out);
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
#ifdef USE_WIDE_CHAR
            std::wstringstream ss;
            ss << std::filesystem::path(filename).stem().wstring() << L"_" << idx << L".wav";
            std::filesystem::path out_file_path = out / ss.str();
            std::wstring out_file_path_str = out_file_path.wstring();
#else
            std::stringstream ss;
            ss << std::filesystem::path(filename).stem().string() << "_" << idx << ".wav";
            std::filesystem::path out_file_path = out / ss.str();
            std::string out_file_path_str = out_file_path.string();
#endif
            SndfileHandle wf = SndfileHandle(out_file_path_str.c_str(), SFM_WRITE, format, channels, sr);
            wf.write(audio.data() + begin_frame, frame_count);
            idx++;
        }
    }
    catch (const std::filesystem::filesystem_error& err)
    {
        QString errmsg = QString("Filesystem error: %1").arg(err.what());
        emit oneError(errmsg);
        return;
    }

    emit oneFinished();
}

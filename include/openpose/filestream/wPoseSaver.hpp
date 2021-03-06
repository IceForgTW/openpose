#ifndef OPENPOSE_FILESTREAM_W_POSE_SAVER_HPP
#define OPENPOSE_FILESTREAM_W_POSE_SAVER_HPP

#include <memory> // std::shared_ptr
#include <string>
#include <openpose/thread/workerConsumer.hpp>
#include "enumClasses.hpp"
#include "keypointSaver.hpp"

namespace op
{
    template<typename TDatums>
    class WPoseSaver : public WorkerConsumer<TDatums>
    {
    public:
        explicit WPoseSaver(const std::shared_ptr<KeypointSaver>& keypointSaver);

        void initializationOnThread();

        void workConsumer(const TDatums& tDatums);

    private:
        const std::shared_ptr<KeypointSaver> spKeypointSaver;

        DELETE_COPY(WPoseSaver);
    };
}





// Implementation
#include <openpose/utilities/errorAndLog.hpp>
#include <openpose/utilities/macros.hpp>
#include <openpose/utilities/pointerContainer.hpp>
#include <openpose/utilities/profiler.hpp>
namespace op
{
    template<typename TDatums>
    WPoseSaver<TDatums>::WPoseSaver(const std::shared_ptr<KeypointSaver>& keypointSaver) :
        spKeypointSaver{keypointSaver}
    {
    }

    template<typename TDatums>
    void WPoseSaver<TDatums>::initializationOnThread()
    {
    }

    template<typename TDatums>
    void WPoseSaver<TDatums>::workConsumer(const TDatums& tDatums)
    {
        try
        {
            if (checkNoNullNorEmpty(tDatums))
            {
                // Debugging log
                dLog("", Priority::Low, __LINE__, __FUNCTION__, __FILE__);
                // Profiling speed
                const auto profilerKey = Profiler::timerInit(__LINE__, __FUNCTION__, __FILE__);
                // T* to T
                auto& tDatumsNoPtr = *tDatums;
                // Record people pose keypoint data
                std::vector<Array<float>> keypointVector(tDatumsNoPtr.size());
                for (auto i = 0; i < tDatumsNoPtr.size(); i++)
                    keypointVector[i] = tDatumsNoPtr[i].poseKeypoints;
                const auto fileName = (!tDatumsNoPtr[0].name.empty() ? tDatumsNoPtr[0].name : std::to_string(tDatumsNoPtr[0].id));
                spKeypointSaver->saveKeypoints(keypointVector, fileName, "pose");
                // Profiling speed
                Profiler::timerEnd(profilerKey);
                Profiler::printAveragedTimeMsOnIterationX(profilerKey, __LINE__, __FUNCTION__, __FILE__, Profiler::DEFAULT_X);
                // Debugging log
                dLog("", Priority::Low, __LINE__, __FUNCTION__, __FILE__);
            }
        }
        catch (const std::exception& e)
        {
            this->stop();
            error(e.what(), __LINE__, __FUNCTION__, __FILE__);
        }
    }

    COMPILE_TEMPLATE_DATUM(WPoseSaver);
}

#endif // OPENPOSE_FILESTREAM_W_POSE_SAVER_HPP

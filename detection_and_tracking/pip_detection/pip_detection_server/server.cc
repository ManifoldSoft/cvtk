#include <stdint.h>
#include <atomic>
#include <iostream>
#include <sstream>

#include "base.h"
#include "grpc_server_kess_launcher.h"
#include "utils.h"
#include "string_utils.h"
#include "protos/video_edge_detection.kess.grpc.pb.h"
#include "include/motion_area_detection.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

DEFINE_string(server_addr, "0.0.0.0", "server address");
DEFINE_int32(p, 9999, "server port");
DEFINE_int32(max_worker_threads, 8, "max number of thread");
DEFINE_string(service_name, "grpc_cvtk_motionAreaDetect", "service name");
DEFINE_string(shard_name, "s0", "shard name");

class EdgeDetectionServiceImpl final : public ::cvtk::img::kess::EdgeDetection::Service {
 public:
  EdgeDetectionServiceImpl() {}

  Status MotionAreaDetect(::grpc::ServerContext* context, const ::cvtk::img::MotionAreaDetectRequest* request,
                            ::cvtk::img::MotionAreaDetectResponse* response) {
    cvtk::StopWatch stopWatch;

    cvtk::detect::MotionAreaDetecion processor;
    cvtk::detect::Video video;
    video.id = request->id();
    std::vector<std::string> frames;
    for (auto& image : request->image()) {
      // add a new frame
      video.frames.emplace_back();
      auto& frame = video.frames.back();

      // for log
      frames.push_back(image.id());

      // frame id
      frame.id = image.id();

      // frame data
      const std::string& data = image.image();
      std::copy(data.begin(), data.end(), std::back_inserter(frame.image));
    }

    LOG(INFO) << "ID: " << video.id << " frame:" << cvtk::printContainer<std::string>(frames.begin(),
                                                                                     frames.end());

    try {
      response->set_id(request->id());
      if (processor.PipDetect(video, response->mutable_motion_area()) != 0) {
      //if (processor.MotionAreaDetect(video, response->mutable_motion_area()) != 0) {
        // 返回 0 是 error..
        response->set_status(1);
      }
    } catch (...) {
      response->set_status(0);
    }

    auto motion = response->motion_area();
    LOG(INFO) << "MotionAreaDetect , ID: " << request->id() << " frame:" << video.frames.size()
              << " width:" << motion.video_width() << " height:" << motion.video_height()
              << " left:" << motion.left() << " right:" << motion.right()
              << " top:" << motion.top() << " bottom:" << motion.bottom()
              << ", ret: " << response->status() << ", cost: " << stopWatch.elapsed();
    return Status::OK;
  }
};

void RunServer() {
  EdgeDetectionServiceImpl service;
  cvtk::launchKessServer(&service, FLAGS_server_addr, FLAGS_p, FLAGS_max_worker_threads, FLAGS_service_name,
                        FLAGS_shard_name);
}

int main(int argc, char** argv) {
  base::InitApp(&argc, &argv, "DetectPicAttribute Server");
  RunServer();
  return 0;
}

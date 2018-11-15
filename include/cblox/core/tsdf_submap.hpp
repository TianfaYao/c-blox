#ifndef CBLOX_CORE_TSDF_SUBMAP_H_
#define CBLOX_CORE_TSDF_SUBMAP_H_

#include <mutex>

#include <Eigen/Geometry>

#include <voxblox/core/tsdf_map.h>
#include <voxblox/mesh/mesh_layer.h>

//#include "./TsdfSubMap.pb.h"

#include "cblox/core/common.hpp"

namespace cblox {

using namespace voxblox;

// Class representing TSDF submap
class TsdfSubmap {
 public:
  typedef std::shared_ptr<TsdfSubmap> Ptr;
  typedef std::shared_ptr<const TsdfSubmap> ConstPtr;

  // Constructor
  TsdfSubmap(const Transformation& T_M_S, KFId keyframe_id,
             TsdfMap::Config config)
      : T_M_S_(T_M_S), keyframe_id_(keyframe_id) {
    tsdf_map_.reset(new TsdfMap(config));
  }

  ~TsdfSubmap() {
    if (!tsdf_map_.unique()) {
      std::cout << "Underlying tsdf map from KFId: " << keyframe_id_
                << " is NOT unique. Therefore its memory may leak."
                << std::endl;
    } else {
      std::cout << "TsdfSubmap " << keyframe_id_ << " is being deleted."
                << std::endl;
    }
  }

  // Returns the underlying map pointers
  TsdfMap::Ptr getTsdfMapPtr() { return tsdf_map_; }
  const TsdfMap& getTsdfMap() const { return *tsdf_map_; }

  // Submap pose interaction
  const Transformation getPose() const {
    std::unique_lock<std::mutex> lock(transformation_mutex);
    return T_M_S_;
  }

  void setPose(const Transformation& T_M_S) {
    std::unique_lock<std::mutex> lock(transformation_mutex);
    T_M_S_ = T_M_S;
  }

  KFId getKeyframeID() const { return keyframe_id_; }

  FloatingPoint block_size() const { return tsdf_map_->block_size(); }

  size_t getNumberAllocatedBlocks() const {
    return tsdf_map_->getTsdfLayer().getNumberOfAllocatedBlocks();
  };

  // Getting the proto for this submap
  //void getProto(TsdfSubMapProto* proto) const;

  // Save the submap to file
  //bool saveToStream(std::fstream* outfile_ptr) const;

 private:
  // The pose of this submap in the global map frame
  mutable std::mutex transformation_mutex;
  Transformation T_M_S_;
  // Linked keyframe ID
  KFId keyframe_id_;
  // Maps
  TsdfMap::Ptr tsdf_map_;

 public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

}  // namespace cblox

#endif /* CBLOX_CORE_TSDF_SUBMAP_H_ */
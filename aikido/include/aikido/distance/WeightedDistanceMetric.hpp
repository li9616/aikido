#ifndef AIKIDO_DISTANCE_WEIGHTEDDISTANCEMETRIC_HPP_
#define AIKIDO_DISTANCE_WEIGHTEDDISTANCEMETRIC_HPP_

#include "DistanceMetric.hpp"
#include "../statespace/CompoundStateSpace.hpp"

namespace aikido
{
namespace distance
{
/// Implements a distance metric on a CompoundStateSpace. This metric computes
/// the weighted
/// sum of distances on the individual components of the statespace.
class WeightedDistanceMetric : public DistanceMetric
{
public:
  /// Constructor. Default the weights applied to each subspace to 1.
  /// \param _space The state space
  /// \param _metrics A vector containing one element for every component of the
  /// CompoundStateSpace
  WeightedDistanceMetric(std::shared_ptr<statespace::CompoundStateSpace> _space,
                         std::vector<DistanceMetricPtr> _metrics);

  /// Constructor.
  /// \param _space The state space
  /// \param _metrics A vector containing one element for every component of the
  /// CompoundStateSpace. The first element of every pair in the vector is the
  /// metric and the second is the weight to be applied to the metric. The
  /// weights must all be positive.
  WeightedDistanceMetric(std::shared_ptr<statespace::CompoundStateSpace> _space,
                         std::vector<std::pair<DistanceMetricPtr, double>> _metrics);


  // Documentation inherited
  statespace::StateSpacePtr getStateSpace() const override;

  /// Computes distance between two states as the weighted sum
  ///  of distances between their matching subcomponents.
  double distance(const statespace::StateSpace::State* _state1,
                  const statespace::StateSpace::State* _state2) const override;

private:
  std::shared_ptr<statespace::CompoundStateSpace> mStateSpace;
  std::vector<std::pair<DistanceMetricPtr, double>> mMetrics;
};
}
}
#endif

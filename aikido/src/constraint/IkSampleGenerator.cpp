#include <aikido/constraint/IkSampleGenerator.hpp>
#include <aikido/statespace/SE3StateSpace.hpp>
#include <dart/common/Console.h>
#include <vector>
#include <stdexcept>

namespace aikido {
namespace constraint {

using util::RNG;
using statespace::MetaSkeletonStateSpace;
using statespace::SE3StateSpace;
using dart::dynamics::INVALID_INDEX;

//=============================================================================
IkSampleGenerator::IkSampleGenerator(
      statespace::MetaSkeletonStateSpacePtr _stateSpace,
      dart::dynamics::InverseKinematicsPtr _inverseKinematics,
      std::unique_ptr<SampleGenerator> _poseSampler,
      std::unique_ptr<SampleGenerator> _seedSampler,
      std::unique_ptr<util::RNG> _rng,
      int _maxNumTrials)
  : mStateSpace(std::move(_stateSpace))
  , mPoseStateSpace(
      std::dynamic_pointer_cast<SE3StateSpace>(_poseSampler->getStateSpace()))
  , mInverseKinematics(std::move(_inverseKinematics))
  , mPoseSampler(std::move(_poseSampler))
  , mSeedSampler(std::move(_seedSampler))
  , mRng(std::move(_rng))
  , mMaxNumTrials(_maxNumTrials)
{
  assert(mStateSpace);
  assert(mPoseStateSpace);
  assert(mInverseKinematics);
  assert(mPoseSampler);
  assert(mSeedSampler);
  assert(mSeedSampler->getStateSpace() == mStateSpace);
  assert(mRng);
  assert(mMaxNumTrials > 0);

  if (mPoseSampler->getNumSamples() != NO_LIMIT)
    dtwarn << "[IkSampleGenerator::constructor] IkSampleGenerator only tries"
              " to find one IK solution per Isometry3d sample. The provided"
              " pose SampleGenerator contains a finite set of "
           << mPoseSampler->getNumSamples()
           << " samples. We advise against using this class on a finite"
              " sets of poses because they may be quickly exhausted.\n";

  if (mSeedSampler->getNumSamples() != NO_LIMIT)
    dtwarn << "[IkSampleGenerator::constructor] IkSampleGenerator uses up to "
           <<  mMaxNumTrials << " seeds per pose sample. The provided seed"
              " SampleGenerator contains a finite set of "
           << mSeedSampler->getNumSamples()
           << " samples. We advise against using this class on a finite"
              " sets of seeds because they may be quickly exhausted.\n";
}

//=============================================================================
statespace::StateSpacePtr IkSampleGenerator::getStateSpace() const
{
  return mStateSpace;
}

//=============================================================================
bool IkSampleGenerator::sample(statespace::StateSpace::State* _state)
{
  if (!mSeedSampler->canSample() || !mPoseSampler->canSample())
    return false;

  auto seedState = mStateSpace->createState();
  auto poseState = mPoseStateSpace->createState();
  auto outputState = static_cast<MetaSkeletonStateSpace::State*>(_state);

  for(int i = 0; i < mMaxNumTrials; ++i)
  {
    // Sample a seed for the IK solver.
    // TODO: What should the retry logic look like if sampling a seed fails?
    if (!mSeedSampler->sample(seedState)){
      continue;
    }

    mStateSpace->setStateOnMetaSkeleton(seedState);

    // Sample a goal for the IK solver.
    if (!mPoseSampler->sample(poseState)){
      continue;
    }

    mInverseKinematics->getTarget()->setTransform(poseState.getIsometry());

    // Run the IK solver. If it succeeds, return the solution.
    if (mInverseKinematics->solve(true))
    {
      mStateSpace->getStateFromMetaSkeleton(outputState);
      return true;
    }
  }

  return false;
}

//=============================================================================
bool IkSampleGenerator::canSample() const
{
  return mSeedSampler->canSample() && mPoseSampler->canSample();
}

//=============================================================================
int IkSampleGenerator::getNumSamples() const
{
  return std::min(
    mSeedSampler->getNumSamples(), mPoseSampler->getNumSamples());
}

} // namespace constraint
} // namespace aikido

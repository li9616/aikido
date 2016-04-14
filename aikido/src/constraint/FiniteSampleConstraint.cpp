#include <aikido/constraint/FiniteSampleConstraint.hpp>

namespace aikido {
namespace constraint {

// For internal use only.
class FiniteSampleGenerator : public SampleGenerator
{
public:

  FiniteSampleGenerator(const FiniteSampleGenerator&) = delete;
  FiniteSampleGenerator(FiniteSampleGenerator&& other) = delete;

  FiniteSampleGenerator& operator=(
    const FiniteSampleGenerator& other) = delete;
  FiniteSampleGenerator& operator=(
    FiniteSampleGenerator&& other) = delete;

  virtual ~FiniteSampleGenerator() = default; 

  /// Documentation inherited.
  statespace::StateSpacePtr getStateSpace() const override;

  /// Documentation inherited.
  bool sample(statespace::StateSpace::State* _state) override;

  /// Documentation inherited.
  int getNumSamples() const override;

  /// Documentation inherited.
  bool canSample() const override;

private:

  // For internal use only.
  FiniteSampleGenerator(
    statespace::StateSpacePtr _stateSpace,
    std::vector<statespace::StateSpace::State*> _states);

  statespace::StateSpacePtr mStateSpace;
  std::vector<statespace::StateSpace::State*> mStates;
  int mIndex;

  friend class FiniteSampleConstraint;
};

//=============================================================================
 FiniteSampleGenerator::FiniteSampleGenerator(
  statespace::StateSpacePtr _stateSpace,
  std::vector<statespace::StateSpace::State*> _states)
: mStateSpace(_stateSpace)
, mStates(_states)
, mIndex(0)
{
  assert(mStateSpace);
  for(auto state: mStates)
  {
    assert(state);
  }
}


//=============================================================================
statespace::StateSpacePtr FiniteSampleGenerator::getStateSpace() const
{
  return mStateSpace;
}

//=============================================================================
bool FiniteSampleGenerator::sample(statespace::StateSpace::State* _state)
{
  if (mStates.size() <= mIndex)
    return false;

  mStateSpace->copyState(_state, mStates[mIndex]);
  ++mIndex;

  return true;
}

//=============================================================================
int FiniteSampleGenerator::getNumSamples() const
{
  return mStates.size() - mIndex;
}

//=============================================================================
bool FiniteSampleGenerator::canSample() const
{
  return mStates.size() > mIndex;
}

//=============================================================================
FiniteSampleConstraint::FiniteSampleConstraint(
  statespace::StateSpacePtr _stateSpace,
  statespace::StateSpace::State* _state)
: mStateSpace(_stateSpace)
{
  assert(mStateSpace);

  statespace::StateSpace::State* state = mStateSpace->allocateState();
  mStateSpace->copyState(state, _state);

  mStates.push_back(state);
}


//=============================================================================
FiniteSampleConstraint::FiniteSampleConstraint(
  statespace::StateSpacePtr _stateSpace,
  std::vector<const statespace::StateSpace::State*> _states)
: mStateSpace(_stateSpace)
{
  assert(mStateSpace);
  mStates.reserve(_states.size());

  for (auto state: _states)
  {
    statespace::StateSpace::State* newState = mStateSpace->allocateState();
    mStateSpace->copyState(newState, state);

    mStates.emplace_back(newState);
  }

}

//=============================================================================
FiniteSampleConstraint::~FiniteSampleConstraint()
{
  for (auto state: mStates)
  {
    mStateSpace->freeState(state);
  }
}

//=============================================================================
statespace::StateSpacePtr FiniteSampleConstraint::getStateSpace() const
{
  return mStateSpace;
}

//=============================================================================
std::unique_ptr<SampleGenerator> FiniteSampleConstraint::createSampleGenerator() const
{
  return std::unique_ptr<FiniteSampleGenerator>(new FiniteSampleGenerator(
    mStateSpace,
    mStates));
}

}
}

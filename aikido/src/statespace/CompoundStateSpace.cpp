#include <aikido/statespace/CompoundStateSpace.hpp>
#include <iostream>

namespace aikido {
namespace statespace {

//=============================================================================
CompoundStateSpace::State::State(
      const std::vector<StateSpace::State*>& _states)
  : mValue(_states)
{
}

//=============================================================================
CompoundStateSpace::State::~State()
{
  for (StateSpace::State* state : mValue)
    delete state;
}

//=============================================================================
size_t CompoundStateSpace::State::getNumStates() const
{
  return mValue.size();
}

//=============================================================================
StateSpace::State* CompoundStateSpace::State::getState(size_t _index)
{
  return mValue[_index];
}

//=============================================================================
const StateSpace::State* CompoundStateSpace::State::getState(
  size_t _index) const
{
  return mValue[_index];
}

//=============================================================================
const std::vector<StateSpace::State*>& CompoundStateSpace::State::getStates()
{
  return mValue;
}

//=============================================================================
std::vector<const StateSpace::State*>
  CompoundStateSpace::State::getStates() const
{
  std::vector<const StateSpace::State*> output;
  output.reserve(mValue.size());

  for (StateSpace::State* state : mValue)
    output.push_back(state);

  return output;
}

//=============================================================================
CompoundStateSpace::CompoundStateSpace(
      const std::vector<StateSpacePtr>& _subspaces)
  : mSubspaces(_subspaces)
{
  mRepresentationDimension = 0;
  for (auto supspace: mSubspaces)
  {
    mRepresentationDimension += supspace->getRepresentationDimension();
  }
}

//=============================================================================
int CompoundStateSpace::getRepresentationDimension() const
{
  return mRepresentationDimension;
}

//=============================================================================
void CompoundStateSpace::compose(
  const StateSpace::State& _state1, const StateSpace::State& _state2,
  StateSpace::State& _out) const
{
  const auto& state1 = static_cast<const State&>(_state1);
  const auto& state2 = static_cast<const State&>(_state2);
  auto& out = static_cast<State&>(_out);
  
  for (size_t i = 0; i < mSubspaces.size(); ++i)
  {
    mSubspaces[i]->compose(
      *state1.mValue[i], *state2.mValue[i], *out.mValue[i]);
  }
}

} // namespace statespace
} // namespace aikido
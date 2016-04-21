#include <aikido/statespace/SO3StateSpace.hpp>
#include <boost/math/constants/constants.hpp>
#include <dart/math/Geometry.h>
#include <iostream>

namespace aikido
{
namespace statespace
{
//=============================================================================
SO3StateSpace::SO3StateSpace::State::State()
    : mValue(1., 0., 0., 0.)
{
}

//=============================================================================
SO3StateSpace::SO3StateSpace::State::State(const Quaternion &_quaternion)
    : mValue(_quaternion)
{
  // TODO: Check if normalized.
}

//=============================================================================
auto SO3StateSpace::State::getQuaternion() const -> const Quaternion &
{
  return mValue;
}

//=============================================================================
void SO3StateSpace::State::setQuaternion(const Quaternion &_quaternion)
{
  // TODO: Check if normalized.
  mValue = _quaternion;
}

//=============================================================================
auto SO3StateSpace::createState() const -> ScopedState
{
  return ScopedState(this);
}

//=============================================================================
auto SO3StateSpace::getQuaternion(const State *_state) const -> const Quaternion
    &
{
  return _state->mValue;
}

//=============================================================================
void SO3StateSpace::setQuaternion(State *_state,
                                  const Quaternion &_quaternion) const
{
  _state->mValue = _quaternion;
}

//=============================================================================
size_t SO3StateSpace::getStateSizeInBytes() const { return sizeof(State); }

//=============================================================================
StateSpace::State *SO3StateSpace::allocateStateInBuffer(void *_buffer) const
{
  return new (_buffer) State;
}

//=============================================================================
void SO3StateSpace::freeStateInBuffer(StateSpace::State *_state) const
{
  static_cast<State *>(_state)->~State();
}

//=============================================================================
void SO3StateSpace::compose(const StateSpace::State *_state1,
                            const StateSpace::State *_state2,
                            StateSpace::State *_out) const
{
  auto state1 = static_cast<const State *>(_state1);
  auto state2 = static_cast<const State *>(_state2);
  auto out = static_cast<State *>(_out);

  out->mValue = state1->mValue * state2->mValue;
}

//=============================================================================
void SO3StateSpace::getIdentity(StateSpace::State *_out) const
{
  auto out = static_cast<State *>(_out);
  setQuaternion(out, Quaternion::Identity());
}

//=============================================================================
void SO3StateSpace::getInverse(const StateSpace::State *_in,
                               StateSpace::State *_out) const
{
  auto in = static_cast<const State *>(_in);
  auto out = static_cast<State *>(_out);

  setQuaternion(out, getQuaternion(in).inverse());
}

//=============================================================================
size_t SO3StateSpace::getDimension() const
{
  return 3;
}

//=============================================================================
void SO3StateSpace::copyState(StateSpace::State *_destination,
                              const StateSpace::State *_source) const
{
  auto destination = static_cast<State *>(_destination);
  auto source = static_cast<const State *>(_source);

  setQuaternion(destination, getQuaternion(source));
}

//=============================================================================
void SO3StateSpace::expMap(const Eigen::VectorXd &_tangent,
                           StateSpace::State *_out) const
{
  auto out = static_cast<State *>(_out);

  // TODO: Skip these checks in release mode.
  if (_tangent.rows() != 3) {
    std::stringstream msg;
    msg << "_tangent has incorrect size: expected 3"
        << ", got " << _tangent.rows() << ".\n";
    throw std::runtime_error(msg.str());
  }

  Eigen::Vector6d tangent(Eigen::Vector6d::Zero());
  tangent.topRows(3) = _tangent;

  Eigen::Isometry3d transform = dart::math::expMap(tangent);
  out->setQuaternion(Quaternion(transform.rotation()));
}

//=============================================================================
void SO3StateSpace::logMap(const StateSpace::State *_in,
                           Eigen::VectorXd &_tangent) const
{
  if (_tangent.rows() != 3) {
    _tangent.resize(3);
  }
  auto in = static_cast<const State *>(_in);

  // Compute rotation matrix from quaternion
  Eigen::Matrix3d rotMat = getQuaternion(in).toRotationMatrix();
  _tangent = dart::math::logMap(rotMat);
}

}  // namespace statespace
}  // namespace aikido

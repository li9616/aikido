#include "aikido/planner/SnapPlanner.hpp"

#include "aikido/common/VanDerCorput.hpp"
#include "aikido/constraint/Testable.hpp"
#include "aikido/statespace/StateSpace.hpp"

namespace aikido {
namespace planner {

//==============================================================================
SnapPlanner::PlanningFunctionMap SnapPlanner::mPlanningFunctionMap;
bool SnapPlanner::mIsRegisteredPlanningFunctions = false;

//==============================================================================
SnapPlanner::SnapPlanner()
{
  if (!mIsRegisteredPlanningFunctions)
  {
    registerPlanningFunction<PlanToConfiguration>(
        &SnapPlanner::planToConfiguration);

    mIsRegisteredPlanningFunctions = true;
  }
}

//==============================================================================
trajectory::InterpolatedPtr SnapPlanner::planToConfiguration(
    const Problem* problem, Problem::Result* result)
{
  if (!problem)
    return nullptr;

  const auto* castedProblem = dynamic_cast<const PlanToConfiguration*>(problem);
  if (!castedProblem)
    throw std::invalid_argument("problem is not PlanToConfiguration type");

  auto* castedResult = dynamic_cast<PlanToConfiguration::Result*>(result);
  if (result && !castedResult)
  {
    throw std::invalid_argument(
        "result is not PlanToConfiguration::Result type");
  }

  auto stateSpace = castedProblem->getStateSpace();
  auto interpolator = castedProblem->getInterpolator();
  auto returnTraj
      = std::make_shared<trajectory::Interpolated>(stateSpace, interpolator);
  auto testState = stateSpace->createState();
  auto startState = castedProblem->getStartState();
  auto goalState = castedProblem->getGoalState();
  auto constraint = castedProblem->getConstraint();

  aikido::common::VanDerCorput vdc{1, true, true, 0.02}; // TODO junk resolution
  for (const auto alpha : vdc)
  {
    interpolator->interpolate(startState, goalState, alpha, testState);
    if (!constraint->isSatisfied(testState))
    {
      if (castedResult)
        result->setMessage("Collision detected");

      return nullptr;
    }
  }

  returnTraj->addWaypoint(0, startState);
  returnTraj->addWaypoint(1, goalState);

  return returnTraj;
}

//==============================================================================
SnapPlanner::PlanningFunctionMap& SnapPlanner::getPlanningFunctionMap()
{
  return mPlanningFunctionMap;
}

//==============================================================================
trajectory::InterpolatedPtr planSnap(
    const statespace::ConstStateSpacePtr& stateSpace,
    const aikido::statespace::StateSpace::State* startState,
    const aikido::statespace::StateSpace::State* goalState,
    const std::shared_ptr<aikido::statespace::Interpolator>& interpolator,
    const std::shared_ptr<aikido::constraint::Testable>& constraint,
    aikido::planner::PlanningResult& planningResult)
{
  if (stateSpace != constraint->getStateSpace())
  {
    throw std::invalid_argument(
        "StateSpace of constraint not equal to StateSpace of planning space");
  }
  aikido::common::VanDerCorput vdc{1, true, true, 0.02}; // TODO junk resolution
  auto returnTraj
      = std::make_shared<trajectory::Interpolated>(stateSpace, interpolator);
  auto testState = stateSpace->createState();

  for (const auto alpha : vdc)
  {
    interpolator->interpolate(startState, goalState, alpha, testState);
    if (!constraint->isSatisfied(testState))
    {
      planningResult.message = "Collision detected";
      return nullptr;
    }
  }

  returnTraj->addWaypoint(0, startState);
  returnTraj->addWaypoint(1, goalState);
  return returnTraj;
}

} // namespace planner
} // namespace aikido

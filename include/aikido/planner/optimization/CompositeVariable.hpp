#ifndef AIKIDO_PLANNER_OPTIMIZATION_COMPOSITEVARIABLE_HPP_
#define AIKIDO_PLANNER_OPTIMIZATION_COMPOSITEVARIABLE_HPP_

#include <map>
#include <memory>

#include <dart/optimizer/optimizer.hpp>

#include "aikido/common/algorithm.hpp"
#include "aikido/constraint/Testable.hpp"
#include "aikido/planner/PlanningResult.hpp"
#include "aikido/planner/optimization/Variable.hpp"
#include "aikido/statespace/dart/MetaSkeletonStateSpace.hpp"
#include "aikido/trajectory/Trajectory.hpp"

namespace aikido {
namespace planner {
namespace optimization {

class CompositeVariable : public Variable
{
public:
  using Index = std::size_t;
  static Index InvalidIndex;

  /// Clone
  std::unique_ptr<Variable> clone() const override;

  /// Returns the dimension of optimization variables.
  std::size_t getDimension() const override;

  /// Sets the optimization variables.
  void setValue(const Eigen::VectorXd& value) override;
  // TODO(JS): Change to setValues()

  /// Returns the optimization variables.
  Eigen::VectorXd getValue() const override;
  // TODO(JS): Change to getValues()

  Eigen::Map<const Eigen::VectorXd> getValueSegment(
      const Eigen::VectorXd& value, const Variable* variable) const;

  std::size_t addSubVariable(VariablePtr variable);

  ConstVariablePtr getSubVariable(std::size_t index) const;

  std::size_t getSubVariableIndex(const Variable* variable) const;

protected:
  void updateDimension();

  std::vector<VariablePtr> mVariables;

  std::size_t mDimension;
};

using CompositeVariablePtr = std::shared_ptr<CompositeVariable>;
using ConstCompositeVariablePtr = std::shared_ptr<const CompositeVariable>;

} // namespace optimization
} // namespace planner
} // namespace aikido

#endif // AIKIDO_PLANNER_OPTIMIZATION_COMPOSITEVARIABLE_HPP_

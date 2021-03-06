set(sources
  SnapPlanner.cpp
  World.cpp
  WorldStateSaver.cpp
)

add_library("${PROJECT_NAME}_planner" SHARED ${sources})
target_link_libraries("${PROJECT_NAME}_planner"
  PUBLIC
    "${PROJECT_NAME}_common"
    "${PROJECT_NAME}_constraint"
    "${PROJECT_NAME}_distance"
    "${PROJECT_NAME}_trajectory"
    "${PROJECT_NAME}_statespace"
    ${DART_LIBRARIES}
    ${Boost_LIBRARIES}
)
target_include_directories("${PROJECT_NAME}_planner" SYSTEM
  PUBLIC
    ${Boost_INCLUDE_DIRS}
    ${DART_INCLUDE_DIRS}
)
target_compile_options("${PROJECT_NAME}_planner"
  PUBLIC ${AIKIDO_CXX_STANDARD_FLAGS}
)

add_component(${PROJECT_NAME} planner)
add_component_targets(${PROJECT_NAME} planner "${PROJECT_NAME}_planner")
add_component_dependencies(${PROJECT_NAME} planner
  common
  constraint
  distance
  statespace
  trajectory
)

format_add_sources(${sources})

add_subdirectory("ompl")      # [constraint], [distance], [statespace], [trajectory], dart, ompl
add_subdirectory("parabolic") # [external], [common], [trajectory], [statespace], dart
add_subdirectory("vectorfield") # [common], [trajectory], [statespace], dart

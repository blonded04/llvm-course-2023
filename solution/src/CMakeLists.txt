set(SIM_SOURCE sim/sim.c)
set(SIM_HEADER "${GameOfLife_SOURCE_DIR}/include/sim/sim.h")

add_library(sim STATIC ${SIM_SOURCE} ${SIM_HEADER})

target_link_libraries(sim PUBLIC csfml-audio csfml-graphics csfml-system csfml-window)
target_include_directories(sim PUBLIC ${GameOfLife_SOURCE_DIR}/include/sim)

set(LIFE_SOURCE life/life.c)
set(LIFE_HEADER "${GameOfLife_SOURCE_DIR}/include/life/life.h")

add_library(life STATIC ${LIFE_SOURCE} ${LIFE_HEADER})

target_include_directories(life PUBLIC ${GameOfLife_SOURCE_DIR}/include/life ${GameOfLife_SOURCE_DIR}/include)

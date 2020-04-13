cmake_minimum_required(VERSION 3.15)
project(ServeurBraveRats)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp
        src/GameManager.cpp
        src/Player.cpp
        src/Game.cpp src/Move.cpp src/Move.hpp src/barriers/BarrierGame.hpp src/variantes/VarianteAbstract.hpp src/variantes/VarianteDefault.hpp src/barriers/BarrierCard.hpp src/variantes/VarianteTraitor.hpp src/variantes/VarianteFastRat.hpp src/variantes/VarianteRandom.hpp src/variantes/VariantePrincess.hpp)

add_executable(ServeurBraveRats ${SOURCE_FILES})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpthread")
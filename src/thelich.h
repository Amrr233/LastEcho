#pragma once
#include <SFML/Graphics.hpp>
#include <string>

enum LichDir   { L_SOUTH=0, L_NORTH=1, L_WEST=2, L_EAST=3 };
enum LichState { L_IDLE=0,  L_WALK=1,  L_ATTACK=2};
enum LichPhase { PHASE_1=0, PHASE_2=1 };

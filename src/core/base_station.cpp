#include "core/base_station.hpp"

BaseStation::BaseStation() {}

BaseStation::BaseStation(Position position)
    : position(position) {}

Position BaseStation::get_position() const {
    return position;
}

void BaseStation::set_position(const Position &position){
    this->position = position;
}
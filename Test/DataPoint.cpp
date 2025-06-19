#include "Datapoint.hpp"
#include <cmath>

DataPoint::DataPoint(double x, double y)
	: position(x,y), clusterId(-1), color(sf::Color::White) {}

DataPoint::DataPoint(const Eigen::Vector2d& pos) 
	: position(pos), clusterId(-1),color(sf::Color::White) {}

double DataPoint::distanceTo(const DataPoint& other) const {
	return (position - other.position).norm();
}

double DataPoint::distanceTo(const Eigen::Vector2d& otherPos) const {
	return (position - otherPos).norm();
}


#ifndef DATA_POINT_HPP // preprocessor directive to conditionally compile code blocks

#define DATA_POINT_HPP

#include <Eigen/Dense>
#include <SFML/Graphics/Color.hpp>


class DataPoint {
public:
	Eigen::Vector2d position;
	int clusterId;
	sf::Color color;
	
	DataPoint(double x, double y);
	DataPoint(const Eigen::Vector2d& pos); // memory pointer right here

	double distanceTo(const DataPoint& other) const;
	double distanceTo(const Eigen::Vector2d& otherPost) const;

};

#endif
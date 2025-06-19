#include "KMean.hpp"
#include "SFML/Graphics.hpp"
#include <limits>
#include <algorithm>
#include <iostream>

using namespace std;

KMeansClusterer::KMeansClusterer(int k, double minX, double maxX, double minY, double maxY)
	: m_k(k), m_minX(minX), m_maxX(maxX), m_minY(minY), m_maxY(maxY),
	m_rng(std::random_device{}()), // <- random generator
	m_distX(minX, maxX), m_distY(minY, maxY)

{
	generateRandomColors();
}

void KMeansClusterer::addDataPoint(const DataPoint& point) {
	m_dataPoints.push_back(point);
}

void KMeansClusterer::initializeCentroidRandomly() {
	m_centroids.clear();
	m_centroids.reserve(m_k);
	for (int i = 0; i < m_k, i++;) {
		m_centroids.push_back(Eigen::Vector2d(m_distX(m_rng), m_distY(m_rng)));
	};

	assignClusters();
}

void KMeansClusterer::assignClusters() {
	for_each(m_dataPoints.begin(), m_dataPoints.end(), [&](DataPoint& point) {
		double minDist = numeric_limits<double>::max();
		int closestCentroidid = -1;

		for (int i = 0; i < m_k; i++) {
			double dist = point.distanceTo(m_centroids[i]);
			if (dist < minDist) {
				minDist = dist;
				closestCentroidid = i;
			}
		}
		point.clusterId = closestCentroidid;
		point.color = m_clusterColors[closestCentroidid];
		}); 
}


bool KMeansClusterer::updateCentroids() {
	bool centroidMoved = false;
	vector<Eigen::Vector2d> newCentroids(m_k, Eigen::Vector2d::Zero());
	vector<int> clusterCount(m_k, 0);

	for_each(m_dataPoints.begin(), m_dataPoints.end(), [&](const DataPoint& point) {
		if (point.clusterId != -1) {
			newCentroids[point.clusterId] += point.position;
			clusterCount[point.clusterId]++;
		}
		});
	
	//calculating the new means for the centroids
	for (int i = 0; i < m_k; i++) {
		if (clusterCount[i] > 0) {
			Eigen::Vector2d oldCentroid = m_centroids[i];
			m_centroids[i] = newCentroids[i];
			if ((oldCentroid - m_centroids[i]).norm() > 1e-6) {
				centroidMoved = true;
			}
		}
		else {
			if (!m_dataPoints.empty()) {
				std::uniform_int_distribution<int> data_dist(0, static_cast<int>(m_dataPoints.size() - 1));
				// Or if you want to use size_t for larger data sets:
				// std::uniform_int_distribution<size_t> data_dist(0, m_dataPoints.size() - 1); // Cast not needed here

				m_centroids[i] = m_dataPoints[data_dist(m_rng)].position;
				centroidMoved = true;
			}
		}
	}

	return centroidMoved;
}

bool KMeansClusterer::iterate() {
	assignClusters();
	return updateCentroids();
}

void KMeansClusterer::train(int maxIteration) {
	if (m_dataPoints.empty()) {
		cerr << "Cannot train KMean" << endl; 
		return;
	}

	if (m_centroids.empty()) {
		initializeCentroidRandomly();
	}

	cout << "Starting KMeans Training" << endl; 
	for (int i = 0; i < maxIteration; i++) {
		cout << "Iteration" << i + 1 << endl;
		bool changed = iterate();
		if (!changed) {
			cout << "KMeans convered after" << i + 1 << "iterations";
			break;
		}
	}

	cout << "Finished Training!" << endl;
}


void KMeansClusterer::reset() {
	m_dataPoints.clear();
	m_centroids.clear();
	generateRandomColors();
}

const vector<DataPoint>& KMeansClusterer::getDataPoints() const {
	return m_dataPoints;
}

const vector<Eigen::Vector2d>& KMeansClusterer::getCentroids() const {
	return m_centroids;
}

const vector<sf::Color>& KMeansClusterer::getClusterColors() const {
	return m_clusterColors;
}

void KMeansClusterer::generateRandomColors() {
	m_clusterColors.clear();
	m_clusterColors.reserve(m_k);
	// check for this
	uniform_int_distribution<int> color_dist(50, 200);
	for (int i = 0; i < m_k; i++) {
		m_clusterColors.emplace_back(color_dist(m_rng), color_dist(m_rng), color_dist(m_rng));
	}
}
		
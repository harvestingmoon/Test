#ifndef KMEANS_CLUSTER_HPP
#define KMEANS_CLUSTER_HPP

#include "Datapoint.hpp"
#include <vector>
#include <random>
#include <functional>

using namespace std;

class KMeansClusterer {
public: 
	KMeansClusterer(int k, double minX, double maxX, double minY, double maxY);
	
	void addDataPoint(const DataPoint& point);

	void initializeCentroidRandomly();

	void assignClusters();

	bool updateCentroids();

	bool iterate();

	void train(int mixIterations);

	void reset();

	const vector<DataPoint>& getDataPoints() const;
	const vector<Eigen::Vector2d>& getCentroids() const;
	const vector<sf::Color>& getClusterColors() const;
	int getK() const { return m_k; }

private:
	int  m_k;
	vector<DataPoint> m_dataPoints;
	vector<Eigen::Vector2d> m_centroids;
	vector<sf::Color> m_clusterColors;

	double m_minX, m_maxX, m_minY, m_maxY;
	// boundaries for random init

	mt19937 m_rng;// seed generator
	uniform_real_distribution<double> m_distX;
	uniform_real_distribution<double> m_distY;

	void generateRandomColors();

};

#endif

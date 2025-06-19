
#include "Application.hpp"
#include <iostream>
#include <random> // For random data generation
#include <string>
#include <algorithm> // For std::min

// Constructor for the Application
Application::Application(unsigned int width, unsigned int height, const std::string& title)
    : m_window(sf::VideoMode({ width, height }), title),
    m_clusterer(5, -1.0, 1.0, -1.0, 1.0), // K=5, data range -1 to 1 for both axes
    m_dataScale(static_cast<float>(std::min(width, height)) * 0.4f), // Scale to 40% of smaller dim
    m_offset(static_cast<float>(width) / 2.f, static_cast<float>(height) / 2.f), // Center of window
    m_isTraining(false),
    m_iterationCount(0),
    m_font() 
    
{
    m_window.setFramerateLimit(60);

    // Load font - CRUCIAL: UPDATE THIS PATH TO YOUR FONT FILE
    if (!m_font.openFromFile("C:\\Windows\\Fonts\\arial.ttf")) {
        std::cerr << "Error: Could not load font file for Application! Please specify a valid path.\n";
        throw std::runtime_error("Failed to load font!"); // Critical error, terminate
    }

    // Font is loaded, now safely construct the sf::Text objects inside the optionals
    // This will call sf::Text(const sf::Font&) constructor
    m_statusText.emplace(m_font);
    m_instructionText.emplace(m_font);

    setupText(); // Now call setupText to configure the properties of the text objects
    initializeSFMLObjects();
}

// Main application loop
void Application::run() {
    sf::Clock clock;
    while (m_window.isOpen()) {
        sf::Time deltaTime = clock.restart();
        processEvents();
        update(deltaTime);
        render();
    }
}

// Event processing
void Application::processEvents() {
    while (const std::optional<sf::Event> event = m_window.pollEvent()) {
        if (event.has_value()) {
            if (event->is<sf::Event::Closed>()) {
                m_window.close();
            }
            else if (auto keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
                    m_window.close();
                }
                if (keyPressed->scancode == sf::Keyboard::Scancode::Space) {
                    // Start/Continue training
                    m_isTraining = !m_isTraining;
                    if (m_isTraining) {
                        // Access optional with ->
                        if (m_statusText) m_statusText->setString("Training: Running...");
                        std::cout << "Training started/resumed.\n";
                    }
                    else {
                        if (m_statusText) m_statusText->setString("Training: Paused.");
                        std::cout << "Training paused.\n";
                    }
                }
                if (keyPressed->scancode == sf::Keyboard::Scancode::R) {
                    // Reset everything
                    m_clusterer.reset();
                    generateRandomData(200); 
                    m_clusterer.initializeCentroidRandomly();
                    m_isTraining = false;
                    m_iterationCount = 0;
                    if (m_statusText) m_statusText->setString("Reset. Press SPACE to train.");
                    std::cout << "Reset and new data generated.\n";
                }
                if (keyPressed->scancode == sf::Keyboard::Scancode::I) {
                    // Perform one iteration manually
                    if (!m_isTraining) { // Only allow manual step if not auto-training
                        bool changed = m_clusterer.iterate();
                        m_iterationCount++;
                        if (m_statusText) m_statusText->setString("Iteration: " + std::to_string(m_iterationCount) + (changed ? "" : " (Converged!)"));
                        std::cout << "Manual iteration " << m_iterationCount << (changed ? " - Changed." : " - Converged.") << std::endl;
                    }
                }
                if (keyPressed->scancode == sf::Keyboard::Scancode::A) {
                    // Add a random point
                    std::random_device rd;
                    std::mt19937 rng(rd());
                    std::uniform_real_distribution<double> dist_x(-1.0, 1.0);
                    std::uniform_real_distribution<double> dist_y(-1.0, 1.0);
                    m_clusterer.addDataPoint(DataPoint(dist_x(rng), dist_y(rng)));
                    // After adding, you might want to re-initialize centroids or re-run clustering
                    // For simplicity, we just add the point. Full re-clustering requires 'R'
                    if (m_statusText) m_statusText->setString("Point added. Press R to reset & re-cluster.");
                    std::cout << "New point added. Clustering will need to re-run (press R).\n";
                }
            }
        }
    }
}

// Update game state (training)
void Application::update(sf::Time deltaTime) {
    if (m_isTraining) {
        // Run one iteration per frame (or less often if desired)
        bool changed = m_clusterer.iterate();
        m_iterationCount++;
        if (m_statusText) m_statusText->setString("Iteration: " + std::to_string(m_iterationCount));
        if (!changed) {
            m_isTraining = false;
            if (m_statusText) m_statusText->setString("Training: CONVERGED after " + std::to_string(m_iterationCount) + " iterations!");
            std::cout << "Training converged after " << m_iterationCount << " iterations.\n";
        }
    }
    updateSFMLPoints();  
    updateSFLCentroids(); // Always update centroid positions
}

// Render everything
void Application::render() {
    m_window.clear(sf::Color(20, 20, 20)); // Dark background

    // Draw data points
    // Functional: Using std::for_each with a lambda to draw each point
    std::for_each(m_sfPoints.begin(), m_sfPoints.end(), [&](const sf::CircleShape& shape) {
        m_window.draw(shape);
        });

    // Draw centroids
    std::for_each(m_sfCentroids.begin(), m_sfCentroids.end(), [&](const sf::CircleShape& shape) {
        m_window.draw(shape);
        });

    if (m_statusText) {
        m_window.draw(*m_statusText); // Dereference the optional to get the sf::Text object
    }
    if (m_instructionText) {
        m_window.draw(*m_instructionText); // Dereference the optional
    }

    m_window.display();
}

// Helper to initialize SFML drawable objects
void Application::initializeSFMLObjects() {
    generateRandomData(200); // Start with some initial data
    m_clusterer.initializeCentroidRandomly(); // Initialize K-Means centroids (corrected function name)

    const float pointRadius = 4.f;
    const float centroidRadius = 8.f;


    m_sfPoints.reserve(m_clusterer.getDataPoints().size());
    for (const auto& dp : m_clusterer.getDataPoints()) {
        sf::CircleShape shape(pointRadius);

        shape.setOrigin(sf::Vector2f(pointRadius, pointRadius)); // Correct: takes two floats
        m_sfPoints.push_back(shape);
    }
    updateSFMLPoints(); // Initial update of positions and colors

    // Create SFML circle shapes for centroids
    m_sfCentroids.reserve(m_clusterer.getK());
    for (int i = 0; i < m_clusterer.getK(); ++i) {
        sf::CircleShape centroidShape(centroidRadius);
        centroidShape.setOrigin(sf::Vector2f(centroidRadius, centroidRadius)); // Correct: takes two floats
        centroidShape.setOutlineThickness(2.f);
        centroidShape.setOutlineColor(sf::Color::White); // Centroid outline
        m_sfCentroids.push_back(centroidShape);
    }
    updateSFLCentroids(); // Initial update of positions and colors
}

// Helper to update SFML point positions and colors from KMeansClusterer data
void Application::updateSFMLPoints() {
    const auto& dataPoints = m_clusterer.getDataPoints();
    // Resize m_sfPoints if data points were added/removed
    if (m_sfPoints.size() != dataPoints.size()) {
        m_sfPoints.resize(dataPoints.size(), sf::CircleShape(4.f));
        // Re-set origin if new shapes were added
        std::for_each(m_sfPoints.begin(), m_sfPoints.end(), [](sf::CircleShape& shape) {
            // Corrected: sf::CircleShape::setOrigin takes two floats or sf::Vector2f
            shape.setOrigin(sf::Vector2f(shape.getRadius(), shape.getRadius()));
            });
    }

    // Functional: Use std::transform to update SFML shapes based on DataPoints
    // This maps each DataPoint to its corresponding SFML shape's properties.
    for (size_t i = 0; i < dataPoints.size(); ++i) {
        // Map Eigen coordinates to SFML window coordinates
        float screenX = static_cast<float>(dataPoints[i].position.x() * m_dataScale + m_offset.x);
        float screenY = static_cast<float>(dataPoints[i].position.y() * m_dataScale + m_offset.y);

        // Correctly create sf::Vector2f from two floats using initializer list
        m_sfPoints[i].setPosition({ screenX, screenY });

        m_sfPoints[i].setFillColor(dataPoints[i].color);
    }
}

// Helper to update SFML centroid positions and colors
void Application::updateSFLCentroids() {
    const auto& centroids = m_clusterer.getCentroids();
    const auto& clusterColors = m_clusterer.getClusterColors();

    if (centroids.size() != m_sfCentroids.size()) {
        m_sfCentroids.resize(centroids.size(), sf::CircleShape(8.f));
        // Re-set origin and outline if new shapes were added
        std::for_each(m_sfCentroids.begin(), m_sfCentroids.end(), [](sf::CircleShape& shape) {
            // Corrected: sf::CircleShape::setOrigin takes two floats or sf::Vector2f
            shape.setOrigin(sf::Vector2f(shape.getRadius(), shape.getRadius()));
            shape.setOutlineThickness(2.f);
            shape.setOutlineColor(sf::Color::White);
            });
    }

    for (size_t i = 0; i < centroids.size(); ++i) {
        // Correctly create sf::Vector2f from two floats using initializer list for centroid positions
        float centroidScreenX = static_cast<float>(centroids[i].x() * m_dataScale + m_offset.x);
        float centroidScreenY = static_cast<float>(centroids[i].y() * m_dataScale + m_offset.y);
        m_sfCentroids[i].setPosition({ centroidScreenX, centroidScreenY });

        m_sfCentroids[i].setFillColor(clusterColors[i]);
    }
}

void Application::generateRandomData(int count) {
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_real_distribution<double> dist_x(-1.0, 1.0);
    std::uniform_real_distribution<double> dist_y(-1.0, 1.0);

    for (int i = 0; i < count; ++i) {
        m_clusterer.addDataPoint(DataPoint(dist_x(rng), dist_y(rng)));
    }
}

void Application::setupText() {

    m_statusText->setCharacterSize(20);
    m_statusText->setFillColor(sf::Color::White);
    m_statusText->setPosition({ 10.f, 10.f });
    m_statusText->setString("Press SPACE to start/pause training. Press R to reset. Press I for one iteration. Press A to add a point.");

    m_instructionText->setCharacterSize(16);
    m_instructionText->setFillColor(sf::Color::Cyan);
    m_instructionText->setPosition({ 10.f, m_window.getSize().y - 40.f });
    m_instructionText->setString("K-Means Clustering Example. Each color is a cluster.");
}
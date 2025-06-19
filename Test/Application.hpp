// Application.hpp
#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <SFML/Graphics.hpp>
#include "KMean.hpp" // Includes DataPoint.hpp and Eigen::Dense
#include <optional> // Required for std::optional

class Application {
public:
    Application(unsigned int width, unsigned int height, const std::string& title);
    void run();

private:
    sf::RenderWindow m_window;
    KMeansClusterer m_clusterer;
    sf::Font m_font; // m_font can still be a regular member as it has a default constructor

    float m_dataScale;
    sf::Vector2f m_offset;

    bool m_isTraining;
    int m_iterationCount;

    std::vector<sf::CircleShape> m_sfPoints;
    std::vector<sf::CircleShape> m_sfCentroids;

    // Use std::optional for sf::Text members
    std::optional<sf::Text> m_statusText;
    std::optional<sf::Text> m_instructionText;


    void processEvents();
    void update(sf::Time deltaTime);
    void render();

    void initializeSFMLObjects();
    void updateSFMLPoints();
    void updateSFLCentroids();
    void generateRandomData(int count);
    void setupText(); // This method will now EMplace the sf::Text objects
};

#endif // APPLICATION_HPP
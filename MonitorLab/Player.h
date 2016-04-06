#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>

class Player : public sf::Sprite {
public:
	Player(sf::Texture &texture, sf::Vector2f const& position, float speed);

	void moveLeft();
	void moveRight();
	void moveUp();
	void moveDown();
private:
	const float speed_;
	sf::Texture& texture_;
};
#endif
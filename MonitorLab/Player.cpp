#include "Player.h"

Player::Player(sf::Texture& texture, sf::Vector2f const& position, float speed) :
sf::Sprite(texture),
speed_(speed),
texture_(texture_)
{
	setPosition(position);

	auto bounds = getLocalBounds();
	setOrigin(bounds.width * 0.5f, bounds.height * 0.5f);
}

void Player::moveLeft() {
	move(-speed_, 0.f);
	setRotation(180.f);
}

void Player::moveRight() {
	move(speed_, 0.f);
	setRotation(0.f);
}

void Player::moveUp() {
	move(0.f, -speed_);
	setRotation(270.f);
}

void Player::moveDown() {
	move(0.f, speed_);
	setRotation(90.f);
}

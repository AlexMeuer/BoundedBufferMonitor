#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <boost/thread.hpp>
#include <boost/atomic.hpp>
#include <chrono>
#include "BoundedBuffer.h"
#include "Player.h"

#define UPDATE_INTERVAL 8
#define DRAW_INTERVAL 16

class Game {
public:
	Game();
	~Game();

	int run();
private:
	void doEventLoop();
	void handleEvent(sf::Event &e);

	void doUpdateLoop();
	void update();

	void doRenderLoop();
	void draw();

	void startMessageFetching();

	boost::atomic<bool> running_;

	sf::RenderWindow *window_;
	//boost::mutex windowMutex_;
	void createWindow();

	sf::Texture playerTex_;
	Player *player_;

	boost::thread updateThread_, renderThread_, eventThread_, visibleMsgMakerThread_;

	BoundedBuffer<std::string> messageBuffer_;

	std::list<std::pair<sf::Text, float>> visibleMessages_;
	boost::mutex visibleMessageListMutex;
	const sf::Vector2f messageSpacing_;
};
#endif
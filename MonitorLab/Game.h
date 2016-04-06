#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <boost/thread.hpp>
#include <boost/atomic.hpp>
#include <chrono>
#include "BoundedBuffer.h"
#include "Player.h"

//Millisecons between each update loop iteration
#define UPDATE_INTERVAL 8

//Milliseconds between each draw loop iteration
#define DRAW_INTERVAL 16

class Game {
public:
	Game();
	~Game();

	int run();
private:
	void handleEvent(sf::Event &e);

	void doUpdateLoop();
	void update();

	void doRenderLoop();
	void draw();

	//Begins message fetching loop. Does not return until running_ is false.
	void startMessageFetching();

	//Set to false to close the game.
	boost::atomic<bool> running_;

	sf::RenderWindow *window_;
	void createWindow();

	sf::Texture playerTex_;
	Player *player_;

	boost::thread updateThread_, renderThread_, visibleMsgMakerThread_;

	//Bounded buffer of strings. Threads can deposit and fetch safely.
	BoundedBuffer<std::string> messageBuffer_;

	//! List of SFML Text objects and their time to live.
	std::list<std::pair<sf::Text, float>> visibleMessages_;

	//! Mutex for accessing visible msg list.
	boost::mutex visibleMessageListMutex;

	//Change in position between one visible msg and the next.
	const sf::Vector2f messageSpacing_;
};
#endif
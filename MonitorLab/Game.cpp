#include "Game.h"

Game::Game() :
running_(false),
messageBuffer_(10),
messageSpacing_(0.f, 30.f)
{
	//window_.setActive(false);
	assert(playerTex_.loadFromFile("player.png"));
	player_ = new Player(playerTex_, {300.f, 300.f}, 20.f);
}

Game::~Game() {
	delete player_;
}

int Game::run() {
	running_ = true;
	updateThread_ = boost::thread(boost::bind(&Game::doUpdateLoop, this));
	renderThread_ = boost::thread(boost::bind(&Game::doRenderLoop, this));

	eventThread_.join();
	renderThread_.join();
	updateThread_.join();

	return 0;
}

void Game::handleEvent(sf::Event& e) {
	printf("\t\t\t\tHandling event.\n");
	switch (e.type) {

		case sf::Event::Closed:
			running_ = false;
			break;

		case sf::Event::KeyPressed:
			if (e.key.code == sf::Keyboard::Right) {
				messageBuffer_.deposit("Moving right...");
				player_->moveRight();
			}
			else if (e.key.code == sf::Keyboard::Left) {
				messageBuffer_.deposit("Moving left...");
				player_->moveLeft();
			}
			else if (e.key.code == sf::Keyboard::Up) {
				messageBuffer_.deposit("Moving up...");
				player_->moveUp();
			}
			else if (e.key.code == sf::Keyboard::Down) {
				messageBuffer_.deposit("Moving down...");
				player_->moveDown();
			}
			break;
		case sf::Event::KeyReleased:
			messageBuffer_.deposit("Key Released");
			break;
		case sf::Event::MouseEntered:
			messageBuffer_.deposit("Mouse Entered");
			break;
		case sf::Event::MouseLeft:
			messageBuffer_.deposit("Mouse Left");
			break;
		default:
			break;


	}
}

void Game::doUpdateLoop() {
	auto clock = std::chrono::steady_clock();
	const auto interval = std::chrono::milliseconds(UPDATE_INTERVAL);
	auto lastTickTime = clock.now() - interval;

	while (running_) {
		if (lastTickTime + interval <= clock.now()) {
			lastTickTime = clock.now();

			update();
		}
	}
}

void Game::update() {
	static const unsigned int boredomInterval = 1000u;
	static unsigned int updatesSinceBoredom = 0u;
	static bool sayAltPhrase = false;

	printf("Updating.\n");

	if (++updatesSinceBoredom >= boredomInterval) {
		if (sayAltPhrase) {		
			messageBuffer_.deposit("Much Bound. Such Buffer. Wow.");
			sayAltPhrase = false;
		}
		else {
			messageBuffer_.deposit("Pacman grows tired of your shenanigans...");
			sayAltPhrase = true;
		}
		updatesSinceBoredom = 0u;
	}
}

void Game::doRenderLoop() {
	visibleMsgMakerThread_ = boost::thread(boost::bind(&Game::startMessageFetching, this));

	createWindow();

	//window_->setActive(true);

	auto clock = std::chrono::steady_clock();
	const auto interval = std::chrono::milliseconds(DRAW_INTERVAL);
	auto lastDrawTime = clock.now() - interval;

	sf::Event e;

	while (running_) {
		if (window_->pollEvent(e)) {
			handleEvent(e);
		}

		if (lastDrawTime + interval <= clock.now()) {
			lastDrawTime = clock.now();

			draw();

		}
	}

	visibleMsgMakerThread_.join();
}

void Game::draw() {
	//windowMutex_.lock();

	printf("\t\tDrawing.\n");

	window_->clear();
	window_->draw(*player_);
	
	visibleMessageListMutex.lock();
	for (auto itr = visibleMessages_.begin(); itr != visibleMessages_.end(); ++itr) {
		itr->second -= DRAW_INTERVAL;
		if (itr->second > 0.f) {
			if (itr->second < 3000.f && itr->first.getCharacterSize() > 0.f) {
				itr->first.setCharacterSize(itr->second / 100.f);
			}
			window_->draw(itr->first);
		}
	}
	visibleMessageListMutex.unlock();

	window_->display();

	//windowMutex_.unlock();
}

void Game::startMessageFetching() {
	sf::Font font;
	assert(font.loadFromFile("COOPBL.TTF"));

	const float timeToLive = 8000.f;

	while (running_) {
		//Fetch a string from the buffer
		auto msg = messageBuffer_.fetch();

		auto txt = sf::Text(msg, font);

		visibleMessageListMutex.lock();
		visibleMessages_.push_front(std::make_pair(txt, timeToLive));

		auto itr = visibleMessages_.begin();
		++itr;	//dont bother with first message, we just put it there
		while (itr != visibleMessages_.end()) {

			//move it down the screen (make space for the new msg)
			itr->first.move(messageSpacing_);

			if (itr->second <= 0.f || itr->first.getCharacterSize() <= 1.f) {
				itr = visibleMessages_.erase(itr);
			} else {
				++itr;
			}
			
		}
		visibleMessageListMutex.unlock();
	}
}

void Game::createWindow() {
	assert(window_ == nullptr);

	window_ = new sf::RenderWindow({ 800u, 800u }, "Monitor Lab");
}

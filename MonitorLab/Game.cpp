#include "Game.h"

Game::Game() :
running_(false),
messageBuffer_(10),
messageSpacing_(0.f, 30.f)
{
	//window_.setActive(false);

	//Create the player.
	assert(playerTex_.loadFromFile("player.png"));
	player_ = new Player(playerTex_, {400.f, 400.f}, 20.f);
}

Game::~Game() {
	delete player_;
}

int Game::run() {
	running_ = true;

	//Start update thread
	updateThread_ = boost::thread(boost::bind(&Game::doUpdateLoop, this));

	//Start render thread
	renderThread_ = boost::thread(boost::bind(&Game::doRenderLoop, this));

	//Wait for threads to be finished
	renderThread_.join();
	updateThread_.join();

	return 0;	//EXIT_SUCCESS
}

void Game::handleEvent(sf::Event& e) {
	printf("\t\t\t\tHandling event.\n");
	switch (e.type) {

		case sf::Event::Closed:
			running_ = false;
			break;

		case sf::Event::KeyPressed:
			//Move the player if a directional key is pressed and deposit a message into the boundedbuffer
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
	//setup fixed timestep
	auto clock = std::chrono::steady_clock();
	const auto interval = std::chrono::milliseconds(UPDATE_INTERVAL);
	auto lastTickTime = clock.now() - interval;

	while (running_) {
		//update at a set interval
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

	//Print a phrase if enough updates have passed (alternate between phrases)
	if (++updatesSinceBoredom >= boredomInterval) {
		if (sayAltPhrase) {		
			messageBuffer_.deposit("Much Bound. Such Buffer. Wow.");
			sayAltPhrase = false;
		}
		else {
			messageBuffer_.deposit("Pacman grows tired of your shenanigans...");
			sayAltPhrase = true;
		}
		//Reset counter
		updatesSinceBoredom = 0u;
	}
}

void Game::doRenderLoop() {
	//Begine message fetching thread
	visibleMsgMakerThread_ = boost::thread(boost::bind(&Game::startMessageFetching, this));

	//Create SFML window on this thread (it is important that this thread creates the window because other pollEvent won't work)
	createWindow();

	//window_->setActive(true);

	//Setup fixed timestep
	auto clock = std::chrono::steady_clock();
	const auto interval = std::chrono::milliseconds(DRAW_INTERVAL);
	auto lastDrawTime = clock.now() - interval;

	sf::Event e;

	while (running_) {
		//Poll and handle any events
		if (window_->pollEvent(e)) {
			handleEvent(e);
		}

		//Draw at a fixed interval
		if (lastDrawTime + interval <= clock.now()) {
			lastDrawTime = clock.now();

			draw();

		}
	}

	//Wait for message fetching thread to finish
	visibleMsgMakerThread_.join();
}

void Game::draw() {
	//windowMutex_.lock();

	printf("\t\tDrawing.\n");

	window_->clear();

	//Draw the player
	window_->draw(*player_);
	
	//Draw each visible mesage to the screen
	visibleMessageListMutex.lock();
	for (auto itr = visibleMessages_.begin(); itr != visibleMessages_.end(); ++itr) {

		//Reduce the text object's time to live
		itr->second -= DRAW_INTERVAL;
		
		if (itr->second > 0.f) {

			//Shrink the text towards end of lifetime
			if (itr->second < 3000.f && itr->first.getCharacterSize() > 0.f) {
				itr->first.setCharacterSize(itr->second / 100.f);
			}

			//Draw the text object
			window_->draw(itr->first);
		}
	}
	visibleMessageListMutex.unlock();

	//Render window to the screen
	window_->display();

	//windowMutex_.unlock();
}

void Game::startMessageFetching() {
	sf::Font font;
	assert(font.loadFromFile("COOPBL.TTF"));

	//Millisecons that each visible msg will live for
	const float timeToLive = 8000.f;

	while (running_) {
		//Fetch a string from the buffer
		auto msg = messageBuffer_.fetch();

		//Turn the string into a visible message
		auto txt = sf::Text(msg, font);

		visibleMessageListMutex.lock();
		//Add the visible message to the list and give it its time to live.
		visibleMessages_.push_front(std::make_pair(txt, timeToLive));

		auto itr = visibleMessages_.begin();
		++itr;	//dont bother with first message, we just put it there
		while (itr != visibleMessages_.end()) {

			//move it down the screen (make space for the new msg)
			itr->first.move(messageSpacing_);

			//Remove any old messages
			if (itr->second <= 0.f) {
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

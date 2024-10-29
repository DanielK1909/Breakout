#include "GameManager.h"
#include "Ball.h"
#include "PowerupManager.h"
#include <iostream>

#define BRICKS_X 5
#define BRICKS_Y 10

GameManager::GameManager(sf::RenderWindow* window)
    : _window(window), _paddle(nullptr), _ball(nullptr), _brickManager(nullptr), _powerupManager(nullptr),
    _messagingSystem(nullptr), _ui(nullptr), _pause(false), _time(0.f), _lives(3), _pauseHold(0.f), _levelComplete(false),
    _powerupInEffect({ none,0.f }), _timeLastPowerupSpawned(0.f)
{
    _font.loadFromFile("font/montS.ttf");
    _masterText.setFont(_font);
    _masterText.setPosition(50, 400);
    _masterText.setCharacterSize(48);
    _masterText.setFillColor(sf::Color::Yellow);

    _name_input.setFont(_font);
    _name_input.setPosition(50, 500);
    _name_input.setCharacterSize(36);
    _name_input.setFillColor(sf::Color::White);
}

void GameManager::initialize()
{
    _paddle = new Paddle(_window);
    _brickManager = new BrickManager(_window, this);
    _messagingSystem = new MessagingSystem(_window);
    _ball = new Ball(_window, 400.0f, this); 
    _powerupManager = new PowerupManager(_window, _paddle, _ball);
    _ui = new UI(_window, _lives, this);

    _view_centre = _window->getView().getCenter();

    _shake_timer = 100.f;

    // Create bricks
    _brickManager->createBricks(BRICKS_X, BRICKS_Y, 80.0f, 30.0f, 5.0f);

    _state = GAME;
}

void GameManager::update(float dt, const sf::Event& event)
{

    switch (_state) {
    case GAME:
        _masterText.setString("");
        updateGame(dt);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) _state = PAUSE;
        if (_brickManager->getRemainingBricks() == 0 || _lives == 0) {
            _state = NAME_INPUT;
            _player_name = "";
            _name_input.setString(_player_name);
        }

        break;
    case PAUSE:
        _masterText.setString("Paused.");

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) _state = GAME;

        break;
    case LEADERBOARD:
        _masterText.setString("Game over. Press R to replay.");

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
            _state = GAME;
            _lives = 3;
            _ui->resetLives();
            _brickManager->createBricks(BRICKS_X, BRICKS_Y, 80.0f, 30.0f, 5.0f);
        }

        break;
    case NAME_INPUT:
        _masterText.setString("Input your name and press ENTER to continue.");

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
            _leaderboard_map[BRICKS_X * BRICKS_Y - _brickManager->getRemainingBricks()].push_back(_player_name);
            _state = LEADERBOARD;
        }

        break;
    }

}

void GameManager::loseLife()
{
    _lives--;
    _ui->lifeLost(_lives);

    _shake_timer = 0.f;
}

void GameManager::render()
{
    switch (_state) {
    case GAME:
        renderGame();
        break;
    case PAUSE:
        renderGame();
        break;
    case LEADERBOARD:
        displayLeaderboard();
        break;
    case NAME_INPUT:
        renderNameInput();
        break;
    }
}

void GameManager::levelComplete()
{
    _levelComplete = true;
}

sf::RenderWindow* GameManager::getWindow() const { return _window; }
UI* GameManager::getUI() const { return _ui; }
Paddle* GameManager::getPaddle() const { return _paddle; }
BrickManager* GameManager::getBrickManager() const { return _brickManager; }
PowerupManager* GameManager::getPowerupManager() const { return _powerupManager; }


void GameManager::updateGame(float dt) {

    _powerupInEffect = _powerupManager->getPowerupInEffect();
    _ui->updatePowerupText(_powerupInEffect);
    _powerupInEffect.second -= dt;

    // timer.
    _time += dt;


    if (_time > _timeLastPowerupSpawned + POWERUP_FREQUENCY && rand() % POWERUP_CHANCE == 0)
    {
        _powerupManager->spawnPowerup();
        _timeLastPowerupSpawned = _time;
    }

    // move paddle
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) _paddle->moveRight(dt);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) _paddle->moveLeft(dt);

    // update everything 
    _paddle->update(dt);
    _ball->update(dt);
    _powerupManager->update(dt);

    if (_shake_timer < 0.5f) {
        sf::View current_view = _window->getView();

        current_view.setCenter(_view_centre + sf::Vector2f(sin(_shake_timer * 30) * 10, sin(_shake_timer * 30) * 20));

        _window->setView(current_view);

        _shake_timer += dt;
    }
}

void GameManager::renderGame() {
    _ui->render();
    _paddle->render();
    _ball->render();
    _brickManager->render();
    _powerupManager->render();
    _window->draw(_masterText);
}

void GameManager::promptNameInput(const sf::Event& event) {
   
    if (_state != NAME_INPUT) return;

    if (event.type == sf::Event::TextEntered) {
        if (event.text.unicode > 127) return;

        _player_name += event.text.unicode;
        _name_input.setString(_player_name);
    }
    
}

void GameManager::renderNameInput() {
    _masterText.setString("Enter your player name:");

    _window->draw(_masterText);
    _window->draw(_name_input);
}
void GameManager::displayLeaderboard() {
    _window->draw(_masterText);

    int i = 0;
    for (auto it = _leaderboard_map.rbegin(); it != _leaderboard_map.rend(); ++it) {

        for (auto& s : it->second) {
            sf::Text text;

            text.setString(s + ": " + std::to_string(it->first));

            text.setFont(_font);
            text.setPosition(50, 500 + i * 20);
            text.setCharacterSize(18);
            text.setFillColor(sf::Color::White);

            _window->draw(text);

            i++;
        }
    }
}
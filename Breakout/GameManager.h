#pragma once
#include <SFML/Graphics.hpp>
#include "CONSTANTS.h"
#include "Paddle.h"
#include "Ball.h"
#include "BrickManager.h"
#include "PowerupManager.h"
#include "MessagingSystem.h"
#include "UI.h"

enum GameState {
    GAME,
    LEADERBOARD,
    PAUSE,
    NAME_INPUT
};

class GameManager {
public:
    GameManager(sf::RenderWindow* window);
    void initialize();
    void update(float dt, const sf::Event& event);
    void loseLife();
    void render();
    void levelComplete();
    void powerupEffect(POWERUPS pu, float t);

    Paddle* getPaddle() const;
    BrickManager* getBrickManager() const;
    PowerupManager* getPowerupManager() const;
    sf::RenderWindow* getWindow() const;
    UI* getUI() const;


    void promptNameInput(const sf::Event& event);

private:
    bool _pause;
    float _pauseHold;
    float _time;
    float _timeLastPowerupSpawned;
    int _lives;
    bool _levelComplete;
    std::pair<POWERUPS, float> _powerupInEffect;

    sf::Font _font;
    sf::Text _masterText;

    sf::RenderWindow* _window;
    sf::Vector2f _view_centre;
    float _shake_timer;
    Paddle* _paddle;
    Ball* _ball;
    BrickManager* _brickManager;
    PowerupManager* _powerupManager;
    MessagingSystem* _messagingSystem;
    UI* _ui;

    static constexpr float PAUSE_TIME_BUFFER = 0.5f;
    static constexpr float POWERUP_FREQUENCY = 7.5f;    // time between minimum powerup spawn
    int POWERUP_CHANCE = 700.f;    // time between minimum powerup spawn

    int _score;
    bool _game_over;
    std::map<int, std::vector<sf::String>> _leaderboard_map;
    sf::Text _name_input;
    sf::String _player_name;

    GameState _state;

    void updateGame(float dt);
    void renderGame();

    void renderNameInput();

    void displayLeaderboard();
};

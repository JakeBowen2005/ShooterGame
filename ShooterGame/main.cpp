#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <vector>

// g++ -o main main.cpp -I/opt/homebrew/include -L/opt/homebrew/lib -lsfml-graphics -lsfml-window -lsfml-system

// cd Desktop/SFML/ShooterGame/ShooterGame

//Just random comments

using namespace std;
using namespace sf;

#define screen_width 640
#define screen_length 480

class Enemy {
private:
    float speedy_;            // Speed for movement along the y-axis
    float speedX_;           // Speed for movement along the x-axis
    RectangleShape shape_;   // Shape representing the enemy

public:
    Enemy(float x, float y, float speedX, float speedY) {
        shape_.setSize(Vector2f(25.f, 25.f));
        shape_.setFillColor(Color::Magenta);
        shape_.setPosition(x, y);
        speedy_ = speedY;
        speedX_ = speedX;
    }

    // Destructor
    ~Enemy() {}

    // Method to move the enemy
    void move() {
        shape_.move(speedX_, speedy_);
    }
    
    void changeDirection() {
        speedX_ *= -1;
        shape_.move(speedX_, speedy_);
    }

    // Method to get the enemy's position (for boundary checks)
    Vector2f getPosition() const {
        return shape_.getPosition();
    }

    // Method to draw the enemy
    void draw(RenderWindow& window) {
        window.draw(shape_);
    }
    FloatRect getGlobalBounds() {
        return shape_.getGlobalBounds();
    }
};

int main() {
    
    RenderWindow window(VideoMode(screen_width, screen_length), "Shooter");
    window.setFramerateLimit(60);
    srand(static_cast<unsigned>(time(0)));
    
    //Projectiles
    CircleShape projectile;
    projectile.setFillColor(Color::Red);
    projectile.setRadius(5.f);
    vector<CircleShape> projectiles;
    projectiles.push_back(CircleShape(projectile));
    
    //Enemies
    /*RectangleShape enemy;
    enemy.setFillColor(Color::Magenta);
    enemy.setSize(Vector2f(50.f, 50.f));
    */
    
    vector<Enemy> enemies;
    int enemySpawnTimer = 0;
    
    
    //Players
    CircleShape player;
    player.setFillColor(Color::White);
    player.setRadius(25.f);
    player.setPosition(screen_width / 2 - player.getRadius(),
                       screen_length - player.getRadius() * 2 - 10.f);
    Vector2f playerCenter;
    int shootTimer = 0;
        
    while (window.isOpen()) {
        
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }
        }
        
        //Update
        playerCenter = Vector2f(player.getPosition().x + player.getRadius(), player.getPosition().y + player.getRadius());
        player.setPosition(Mouse::getPosition(window).x, player.getPosition().y);
        
        if (player.getPosition().x < 0) {
            player.setPosition(0.f, player.getPosition().y);
        }
        if (player.getPosition().x + player.getRadius() > screen_width) {
            player.setPosition(screen_width - player.getRadius()*2, player.getPosition().y);
        }
        
        //Projectiles
        if (shootTimer < 10) {
            shootTimer++;
        }
        if (Mouse::isButtonPressed(Mouse::Left) && shootTimer >= 10) {
            //shoot
            projectile.setPosition(playerCenter);
            projectiles.push_back(CircleShape(projectile));
            
            shootTimer = 0;
            
        }
        
        for (size_t i = 0; i < projectiles.size(); i++) {
            projectiles[i].move(0.f, -10.f);
            
            if (projectiles[i].getPosition().y < 0) {
                projectiles.erase(projectiles.begin() + i);
            }
        }
        
        //Enemies
        if (enemySpawnTimer < 50) {
            enemySpawnTimer++;
        }
        if (enemySpawnTimer >= 50) {
            float xpos = rand() % static_cast<int>(screen_width);
            float ypos = 0.f;
            float speedX = static_cast<float>(rand() % 5 - 2); // Random x speed between -2 and 2
            float speedY = 3.f; // Constant downward speed
            Enemy newEnemy(xpos, ypos, speedX, speedY);
            enemies.push_back(newEnemy);
            enemySpawnTimer = 0;
        }
        for (size_t i = 0; i < enemies.size(); i++) {
            enemies[i].move();
            if (enemies[i].getPosition().x <= 0) {
                enemies[i].changeDirection();
            }
            if (enemies[i].getPosition().x + 25.f >= screen_width) {
                enemies[i].changeDirection();
            }
            
            if (enemies[i].getPosition().y > screen_length) {
                enemies.erase(enemies.begin() + i);
            }
        }
        
        if (!enemies.empty() && !projectiles.empty()) {
            for (size_t i = 0; i < enemies.size(); i++) {
                for (size_t k = 0; k < projectiles.size(); k++) {
                    if (projectiles[k].getGlobalBounds().intersects(enemies[i].getGlobalBounds())) {
                        enemies.erase(enemies.begin() + i);
                        projectiles.erase(projectiles.begin() + k);
                        break;
                    }
                }
            }
        }
        
        //Draw
        
        window.clear();
        
        window.draw(player);
        
        for (size_t i = 0; i < enemies.size(); i++) {
            enemies[i].draw(window);
        }
        
        for (size_t i = 0; i < projectiles.size(); i++) {
            window.draw(projectiles[i]);
        }
        
        window.display();
    }
    
    return 0;
}

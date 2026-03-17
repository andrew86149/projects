#include <sstream>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "ZombieArena.h"
#include "Player.h"
#include "TextureHolder.h"
#include "Bullet.h"
#include "Pickup.h"
#include<iostream>
using namespace sf;

int main()
{
    // Here is the instance of TextureHolder
    TextureHolder holder;


    // The game will always be in one of four states
    enum class State {
        PAUSED, LEVELING_UP,
        GAME_OVER, PLAYING
    };

    // Start with the GAME_OVER state
    State state = State::GAME_OVER;
    // Get the screen resolution and
    // create an SFML window
    Vector2f resolution;
    resolution.x = VideoMode::getDesktopMode().size.x;
    resolution.y = VideoMode::getDesktopMode().size.y;
    RenderWindow window(
        VideoMode({resolution.x, resolution.y}),
        "Zombie Arena", Style::Default);
    // Create a an SFML View for the main action
    View mainView(FloatRect({0, 0}, {resolution.x, resolution.y}));
    // Here is our clock for timing everything
    Clock clock;
    // How long has the PLAYING state been active
    Time gameTimeTotal;
    // Where is the mouse in
    // relation to world coordinates
    Vector2f mouseWorldPosition;
    // Where is the mouse in
    // relation to screen coordinates
    Vector2i mouseScreenPosition;
    // Create an instance of the Player class
    Player player;
    // The boundaries of the arena
    IntRect arena;
    // Create the background
    VertexArray background;
    // Load the texture for our background vertex array
    Texture textureBackground = TextureHolder::GetTexture("graphics/background_sheet.png");

    // Prepare for a horde of zombies
    int numZombies;
    int numZombiesAlive;
    Zombie* zombies = nullptr;

    // 100 bullets should do
    Bullet bullets[100];
    int currentBullet = 0;
    int bulletsSpare = 24;
    int bulletsInClip = 6;
    int clipSize = 6;
    float fireRate = 1;
    // When was the fire button last pressed?
    Time lastPressed;

    // Hide the mouse pointer and replace it with crosshair
    window.setMouseCursorVisible(true);
    Texture textureCrosshair = TextureHolder::GetTexture("graphics/crosshair.png");
    Sprite spriteCrosshair(textureCrosshair);
    
    //spriteCrosshair.setTexture(textureCrosshair);
    spriteCrosshair.setOrigin({25, 25});

    // Create a couple of pickups
    Pickup healthPickup(1);
    Pickup ammoPickup(2);

    // About the game
    int score = 0;
    int hiScore = 0;

    // For the home/game over screen
    Texture textureGameOver = TextureHolder::GetTexture("graphics/background.png");
    Sprite spriteGameOver(textureGameOver);
    
    //spriteGameOver.setTexture(textureGameOver);
    spriteGameOver.setPosition({0, 0});
    // Create a view for the HUD
    View hudView(FloatRect({0, 0}, {1920,1080}));
    // Create a sprite for the ammo icon
    Texture textureAmmoIcon = TextureHolder::GetTexture("graphics/ammo_icon.png");
    Sprite spriteAmmoIcon(textureAmmoIcon);
    
    //spriteAmmoIcon.setTexture(textureAmmoIcon);
    spriteAmmoIcon.setPosition({20, 980});
    // Load the font
    Font font("fonts/zombiecontrol.ttf");
    //font.loadFromFile("fonts/zombiecontrol.ttf");
    // Paused
    Text pausedText(font);
    //pausedText.setFont(font);
    pausedText.setCharacterSize(155);
    pausedText.setFillColor(Color::White);
    pausedText.setPosition({400, 400});
    pausedText.setString("Press Enter \nto continue");
    // Game Over
    Text gameOverText(font);
    //gameOverText.setFont(font);
    gameOverText.setCharacterSize(125);
    gameOverText.setFillColor(Color::White);
    gameOverText.setPosition({250, 850});
    gameOverText.setString("Press Enter to play");
    // LEVELING up
    Text levelUpText(font);
    //levelUpText.setFont(font);
    levelUpText.setCharacterSize(80);
    levelUpText.setFillColor(Color::White);
    levelUpText.setPosition({150, 250});
    std::stringstream levelUpStream;
    levelUpStream <<
        "1- Increased rate of fire" <<
        "\n2- Increased clip size(next reload)" <<
        "\n3- Increased max health" <<
        "\n4- Increased run speed" <<
        "\n5- More and better health pickups" <<
        "\n6- More and better ammo pickups";
    levelUpText.setString(levelUpStream.str());
    // Ammo
    Text ammoText(font);
    //ammoText.setFont(font);
    ammoText.setCharacterSize(55);
    ammoText.setFillColor(Color::White);
    ammoText.setPosition({200, 980});
    // Score
    Text scoreText(font);
    //scoreText.setFont(font);
    scoreText.setCharacterSize(55);
    scoreText.setFillColor(Color::White);
    scoreText.setPosition({20, 0});

    // Load the high score from a text file
    std::ifstream inputFile("gamedata/scores.txt");
    if (inputFile.is_open())
    {
        // >> Reads the data
        inputFile >> hiScore;
        inputFile.close();
    }


    // Hi Score
    Text hiScoreText(font);
    //hiScoreText.setFont(font);
    hiScoreText.setCharacterSize(55);
    hiScoreText.setFillColor(Color::White);
    hiScoreText.setPosition({1400, 0});
    std::stringstream s;
    s << "Hi Score:" << hiScore;
    hiScoreText.setString(s.str());
    // Zombies remaining
    Text zombiesRemainingText(font);
    //zombiesRemainingText.setFont(font);
    zombiesRemainingText.setCharacterSize(55);
    zombiesRemainingText.setFillColor(Color::White);
    zombiesRemainingText.setPosition({1500, 980});
    zombiesRemainingText.setString("Zombies: 100");
    // Wave number
    int wave = 0;
    Text waveNumberText(font);
    //waveNumberText.setFont(font);
    waveNumberText.setCharacterSize(55);
    waveNumberText.setFillColor(Color::White);
    waveNumberText.setPosition({1250, 980});
    waveNumberText.setString("Wave: 0");
    // Health bar
    RectangleShape healthBar;
    healthBar.setFillColor(Color::Red);
    healthBar.setPosition({450, 980});

    // When did we last update the HUD?
    int framesSinceLastHUDUpdate = 0;
    // How often (in frames) should we update the HUD
    int fpsMeasurementFrameInterval = 1000;

    // Prepare the hit sound
    SoundBuffer hitBuffer;
    hitBuffer.loadFromFile("sound/hit.wav");
    Sound hit(hitBuffer);
    //hit.setBuffer(hitBuffer);
    // Prepare the splat sound
    SoundBuffer splatBuffer;
    splatBuffer.loadFromFile("sound/splat.wav");
    Sound splat(splatBuffer);
    //splat.setBuffer(splatBuffer);
    // Prepare the shoot sound
    SoundBuffer shootBuffer;
    shootBuffer.loadFromFile("sound/shoot.wav");
    Sound shoot(shootBuffer);
    //shoot.setBuffer(shootBuffer);
    // Prepare the reload sound
    SoundBuffer reloadBuffer;
    reloadBuffer.loadFromFile("sound/reload.wav");
    Sound reload(reloadBuffer);
    //reload.setBuffer(reloadBuffer);
    // Prepare the failed sound
    SoundBuffer reloadFailedBuffer;
    reloadFailedBuffer.loadFromFile("sound/reload_failed.wav");
    Sound reloadFailed(reloadFailedBuffer);
    //reloadFailed.setBuffer(reloadFailedBuffer);
    // Prepare the powerup sound
    SoundBuffer powerupBuffer;
    powerupBuffer.loadFromFile("sound/powerup.wav");
    Sound powerup(powerupBuffer);
    //powerup.setBuffer(powerupBuffer);
    // Prepare the pickup sound
    SoundBuffer pickupBuffer;
    pickupBuffer.loadFromFile("sound/pickup.wav");
    Sound pickup(pickupBuffer);
    //pickup.setBuffer(pickupBuffer);


    // The main game loop
    while (window.isOpen())
    {
        /*
    ************
    Handle input
    ************
    */
    // Handle events by polling
        //Event event;
        while (const std::optional event = window.pollEvent())
        {
            //if (event.type == Event::KeyPressed)
            if (event->is<sf::Event::KeyPressed>())
            {
                // Pause a game while playing
                if (Keyboard::isKeyPressed(Keyboard::Key::Enter) && state == State::PLAYING)
                {
                    state = State::PAUSED;
                }
                // Restart while paused
                else if (Keyboard::isKeyPressed(Keyboard::Key::Enter) && state == State::PAUSED)
                {
                    state = State::PLAYING;
                    // Reset the clock so there isn't a frame jump
                    clock.restart();
                }
                // Start a new game while in GAME_OVER state
                else if (Keyboard::isKeyPressed(Keyboard::Key::Enter) && state == State::GAME_OVER)
                {
                    state = State::LEVELING_UP;
                    wave = 0;
                    score = 0;
                    // Prepare the gun and ammo for next game
                    currentBullet = 0;
                    bulletsSpare = 24;
                    bulletsInClip = 6;
                    clipSize = 6;
                    fireRate = 1;
                    // Reset the player's stats
                    player.resetPlayerStats();

                }
                if (state == State::PLAYING)
                {
                    // Reloading
                    if (Keyboard::isKeyPressed(Keyboard::Key::R))
                    {
                        if (bulletsSpare >= clipSize)
                        {
                            // Plenty of bullets. Reload.
                            bulletsInClip = clipSize;
                            bulletsSpare -= clipSize;
                            reload.play();
                        }
                        else if (bulletsSpare > 0)
                        {
                            // Only few bullets left
                            bulletsInClip = bulletsSpare;
                            bulletsSpare = 0;
                            reload.play();
                        }
                        else
                        {
                            // More here soon?!
                            reloadFailed.play();
                        }
                    }

                }
            }
        }// End event polling

        // Handle the player quitting
        if (Keyboard::isKeyPressed(Keyboard::Key::Escape))
        {
            window.close();
        }

        // Handle WASD while playing
        if (state == State::PLAYING)
        {
            // Handle the pressing and releasing of WASD keys
            if (Keyboard::isKeyPressed(Keyboard::Key::W))
            {
                player.moveUp();
            }
            else
            {
                player.stopUp();
            }
            if (Keyboard::isKeyPressed(Keyboard::Key::S))
            {
                player.moveDown();
            }
            else
            {
                player.stopDown();
            }
            if (Keyboard::isKeyPressed(Keyboard::Key::A))
            {
                player.moveLeft();
            }
            else
            {
                player.stopLeft();
            }
            if (Keyboard::isKeyPressed(Keyboard::Key::D))
            {
                player.moveRight();
            }
            else
            {
                player.stopRight();
            }

            // Fire a bullet
            if (Mouse::isButtonPressed(sf::Mouse::Button::Left))
            {
                if (gameTimeTotal.asMilliseconds()
                    - lastPressed.asMilliseconds()
            > 1000 / fireRate && bulletsInClip > 0)
                {
                    // Pass the centre of the player
                    // and the centre of the cross-hair
                    // to the shoot function
                    bullets[currentBullet].shoot(
                        player.getCenter().x, player.getCenter().y,
                        mouseWorldPosition.x, mouseWorldPosition.y);
                    currentBullet++;
                    if (currentBullet > 99)
                    {
                        currentBullet = 0;
                    }
                    lastPressed = gameTimeTotal;
                    shoot.play();
                    bulletsInClip--;
                }
            }// End fire a bullet

        }// End WASD while playing

        // Handle the LEVELING up state
        if (state == State::LEVELING_UP)
        {
            // Handle the player LEVELING up
            // Handle the player LEVELING up
            if (Keyboard::isKeyPressed(Keyboard::Key::Num1))
            {
                // Increase fire rate
                fireRate++;
                state = State::PLAYING;
            }
            if (Keyboard::isKeyPressed(Keyboard::Key::Num2))
            {
                // Increase clip size
                clipSize += clipSize;
                state = State::PLAYING;
            }
            if (Keyboard::isKeyPressed(Keyboard::Key::Num3))
            {
                // Increase health
                player.upgradeHealth();
                state = State::PLAYING;
            }
            if (Keyboard::isKeyPressed(Keyboard::Key::Num4))
            {
                // Increase speed
                player.upgradeSpeed();
                state = State::PLAYING;
            }
            if (Keyboard::isKeyPressed(Keyboard::Key::Num5))
            {
                // Upgrade pickup
                healthPickup.upgrade();
                state = State::PLAYING;
            }
            if (Keyboard::isKeyPressed(Keyboard::Key::Num6))
            {
                // Upgrade pickup
                ammoPickup.upgrade();
                state = State::PLAYING;
            }


            if (state == State::PLAYING)
            {
                // Increase the wave number
                wave++;

                // Prepare the level
                // We will modify the next two lines later
                arena.size.x = 500 * wave;
                arena.size.y = 500 * wave;

                arena.position.x = 0;
                arena.position.y = 0;
                // Pass the vertex array by reference
                // to the createBackground function
                int tileSize = createBackground(background, arena);
                // We will modify this line of code later
                // int tileSize = 50;

                // Spawn the player in middle of the arena
                player.spawn(arena, resolution, tileSize);
                // Create a horde of zombies
                // Configure the pick-ups
                healthPickup.setArena(arena);
                ammoPickup.setArena(arena);

                numZombies = 5 * wave;
                // Delete the previously allocated memory (if it exists)
                delete[] zombies;
                zombies = createHorde(numZombies, arena);
                numZombiesAlive = numZombies;

                // Play the powerup sound
                powerup.play();


                // Reset clock so there isn't a frame jump
                clock.restart();
            }
        }// End LEVELING up

        /*
    ****************
    UPDATE THE FRAME
    ****************
    */
        if (state == State::PLAYING)
        {
            // Update the delta time
            Time dt = clock.restart();

            // Update the total game time
            gameTimeTotal += dt;

            // Make a fraction of 1 from the delta time
            float dtAsSeconds = dt.asSeconds();
            // Where is the mouse pointer
            mouseScreenPosition = Mouse::getPosition();

            // Convert mouse position to world 
            // based coordinates of mainView
            mouseWorldPosition = window.mapPixelToCoords(
                Mouse::getPosition(), mainView);

            // Set the crosshair to the mouse world location
            spriteCrosshair.setPosition(mouseWorldPosition);

            // Update the player
            player.update(dtAsSeconds, Mouse::getPosition());
            // Make a note of the players new position
            Vector2f playerPosition(player.getCenter());

            // Make the view centre 
            // the around player                
            mainView.setCenter(player.getCenter());
            // Loop through each Zombie and update them
            for (int i = 0; i < numZombies; i++)
            {
                if (zombies[i].isAlive())
                {
                    zombies[i].update(dt.asSeconds(), playerPosition);
                }
            }
            // Update any bullets that are in-flight
            for (int i = 0; i < 100; i++)
            {
                if (bullets[i].isInFlight())
                {
                    bullets[i].update(dtAsSeconds);
                }
            }

            // Update the pickups
            healthPickup.update(dtAsSeconds);
            ammoPickup.update(dtAsSeconds);

            // Collision detection
            // Have any zombies been shot?
            for (int i = 0; i < 100; i++)
            {
                for (int j = 0; j < numZombies; j++)
                {
                    if (bullets[i].isInFlight() &&
                        zombies[j].isAlive())
                    {
                        if (bullets[i].getPosition().findIntersection(zombies[j].getPosition()))
                        {
                            // Stop the bullet
                            bullets[i].stop();
                            // Register the hit and see if it was a kill
                            if (zombies[j].hit())
                            {
                                // Not just a hit but a kill too
                                score += 10;
                                if (score >= hiScore)
                                {
                                    hiScore = score;
                                }
                                numZombiesAlive--;
                                // When all the zombies are dead (again)
                                if (numZombiesAlive == 0) {
                                    state = State::LEVELING_UP;
                                }

                                // Make a splat sound
                                splat.play();

                            }

                        }
                    }
                }
            }// End zombie being shot

            // Have any zombies touched the player            
            for (int i = 0; i < numZombies; i++)
            {
                if (player.getPosition().findIntersection(zombies[i].getPosition()) && zombies[i].isAlive())
                {
                    if (player.hit(gameTimeTotal))
                    {
                        // More here later
                        
                        hit.play();
                    }
                    if (player.getHealth() <= 0)
                    {
                        state = State::GAME_OVER;
                        std::ofstream outputFile("gamedata/scores.txt");
                        // << writes the data
                        outputFile << hiScore;
                        outputFile.close();

                    }
                }
            }// End player touched

            // Has the player touched health pickup
            if (player.getPosition().findIntersection(healthPickup.getPosition()) && healthPickup.isSpawned())
            {
                player.increaseHealthLevel(healthPickup.gotIt());
                // Play a sound
                pickup.play();



            }
            // Has the player touched ammo pickup
            if (player.getPosition().findIntersection(ammoPickup.getPosition()) && ammoPickup.isSpawned())
            {
                bulletsSpare += ammoPickup.gotIt();
                // Play a sound
                reload.play();


            }

            // size up the health bar
            healthBar.setSize(Vector2f(player.getHealth() * 3, 50));
            // Increment the number of frames since the previous update
            framesSinceLastHUDUpdate++;
            // re-calculate every fpsMeasurementFrameInterval frames
            if (framesSinceLastHUDUpdate > fpsMeasurementFrameInterval)
            {
                // Update game HUD text
                std::stringstream ssAmmo;
                std::stringstream ssScore;
                std::stringstream ssHiScore;
                std::stringstream ssWave;
                std::stringstream ssZombiesAlive;
                // Update the ammo text
                ssAmmo << bulletsInClip << "/" << bulletsSpare;
                ammoText.setString(ssAmmo.str());
                // Update the score text
                ssScore << "Score:" << score;
                scoreText.setString(ssScore.str());
                // Update the high score text
                ssHiScore << "Hi Score:" << hiScore;
                hiScoreText.setString(ssHiScore.str());
                // Update the wave
                ssWave << "Wave:" << wave;
                waveNumberText.setString(ssWave.str());
                // Update the high score text
                ssZombiesAlive << "Zombies:" << numZombiesAlive;
                zombiesRemainingText.setString(ssZombiesAlive.str());
                framesSinceLastHUDUpdate = 0;
            }// End HUD update


        }// End updating the scene

         

    /*
    **************
    Draw the scene
    **************
    */
        if (state == State::PLAYING)
        {
            window.clear();
            // set the mainView to be displayed in the window
            // And draw everything related to it
            window.setView(mainView);

            // Draw the background
            window.draw(background, &textureBackground);

            // Draw the zombies
            for (int i = 0; i < numZombies; i++)
            {
                window.draw(zombies[i].getSprite());
            }

            for (int i = 0; i < 100; i++)
            {
                if (bullets[i].isInFlight())
                {
                    window.draw(bullets[i].getShape());
                }
            }

            // Draw the pick-ups, if currently spawned
            if (ammoPickup.isSpawned())
            {
                window.draw(ammoPickup.getSprite());
            }

            if (healthPickup.isSpawned())
            {
                window.draw(healthPickup.getSprite());
            }

            //Draw the crosshair
            window.draw(spriteCrosshair);

            // Draw the player
            window.draw(player.getSprite());

            // Switch to the HUD view
            window.setView(hudView);
            // Draw all the HUD elements
            window.draw(spriteAmmoIcon);
            window.draw(ammoText);
            window.draw(scoreText);
            window.draw(hiScoreText);
            window.draw(healthBar);
            window.draw(waveNumberText);
            window.draw(zombiesRemainingText);


            
        }

        if (state == State::LEVELING_UP)
        {
            window.draw(spriteGameOver);
            window.draw(levelUpText);

        }

        if (state == State::PAUSED)
        {
            window.draw(pausedText);
        }

        if (state == State::GAME_OVER)
        {
            window.draw(spriteGameOver);
            window.draw(gameOverText);
            window.draw(scoreText);
            window.draw(hiScoreText);

        }

        window.display();

    }// End game loop


    // Delete the previously allocated memory (if it exists)
    delete[] zombies;

    return 0;

}

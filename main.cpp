#include <SFML/Graphics.hpp>
#include <iostream>
#include <experimental/random>
#include <SFML/Audio.hpp>

int main()
{
    std::srand(std::time(nullptr));
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Minecrap", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);
    window.setPosition(sf::Vector2i(0,0));
    window.setMouseCursorVisible(false);

    //#include <memory>
    //auto object = std::make_shared<sf::RectangleShape>();//Ponteiros inteligentes, não precisa destruir e atribuir nul no destrutor
    //sf::RectangleShape * object = new sf::RectangleShape();//Ponteiros, porem precisa declarar o delete e atribuir nullptr ao objeto no destrutor
    //opcoes acima é quando instancia como Ponteiro, ideal para games grandes, acesso por -> e não por .

    bool pressed = false, gameover = false, paused = false;
    float velocidade = 5.f;


    //Pontos e Vidas do Jogo
    int points = 0;
    int vidas = 5;

    //Efeitos de Sons
    sf::SoundBuffer pop, gameover_buffer;
    pop.loadFromFile("./sounds/pop.wav");
    gameover_buffer.loadFromFile("./sounds/game-over-sound.wav");

    sf::Sound pop_sound(pop), gameover_sound(gameover_buffer);

    //Musicas
    sf::Music music;
    music.openFromFile("./sounds/soundtrack.wav");


    //Fontes
    sf::Font font, jet;
    font.loadFromFile("./fonts/Minecraft.ttf");
    jet.loadFromFile("./fonts/jetbrains.ttf");

    //Textos
    sf::Text score, life, gameover_text, f1, paused_text;
    score.setFont(font);
    score.setString("Pontos: " + std::to_string(points));
    score.setFillColor(sf::Color::White);
    score.setPosition(5.f, 5.f);

    life.setFont(font);
    life.setString("Vidas: " + std::to_string(vidas));
    life.setFillColor(sf::Color::White);
    life.setPosition(1130.f, 5.f);

    paused_text.setFont(jet);
    paused_text.setString("PAUSED");
    paused_text.setFillColor(sf::Color::White);
    paused_text.setPosition(350, 300);
    paused_text.setCharacterSize(180);

    gameover_text.setFont(font);
    gameover_text.setString("GAME OVER");
    gameover_text.setFillColor(sf::Color::White);
    gameover_text.setPosition(350, 300);
    gameover_text.setCharacterSize(100);

    f1.setFont(jet);
    f1.setFillColor(sf::Color::Yellow);
    f1.setPosition(600, 100);
    f1.setString("Pressione F1 para um novo jogo.");


    //Texturas
    sf::Texture bg;
    bg.loadFromFile("./imgs/fundo.jpg");

    sf::Texture texture;
    texture.loadFromFile("./imgs/minecrap.png");

    sf::Texture hammer;//machado a ser substituido no ponteiro do mouse
    hammer.loadFromFile("./imgs/hammer.png");

    sf::Texture stop;
    stop.loadFromFile("./imgs/paused.jpg");

    //Sprites
    sf::Sprite object(texture), fundo(bg), ham(hammer), stop_sprite(stop);


    float x = static_cast<float>(std::experimental::randint(10, (int) (window.getSize().x - texture.getSize().x - 10)));

    //Posicao do mouse e coordenadas do mouse 
    sf::Vector2i pos_mouse_win;//posicao do mouse em relacao a janela (window)
    sf::Vector2f pos_mouse_coord;//armazenara as coordenadas mapeadas

    //Objetos
    std::vector<sf::Sprite> objs;

    size_t max_objs = 5;
    float obj_vel_mx = 10.f;
    float obj_vel = obj_vel_mx;

    //para não permitir que o usuario depois de pausar coloque o mouse onde ele quiser, e assim roubar no jogo
    sf::Vector2i current_position;


    while (window.isOpen()) {
        sf::Event event;

        if (music.getStatus() == sf::Music::Stopped || music.getStatus() == sf::Music::Paused) {
            music.play();
        }

        while(window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            //se for redimensionado a janela ajusta dinamicamente
            if (event.type == sf::Event::Resized) {
                sf::FloatRect visible_area(0, 0, event.size.width, event.size.height);
                window.setView(sf::View(visible_area));
            }

            //controle para que force o jogador a clicar no mouse 
            if (event.type == event.MouseButtonPressed) {
                current_position = sf::Mouse::getPosition(window);
                pressed = false;
            }

            //se precionado a tecla ENTER
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return)) {
                paused = true;
            }

            pos_mouse_win = sf::Mouse::getPosition(window);
            pos_mouse_coord = window.mapPixelToCoords(pos_mouse_win);

            ham.setPosition((sf::Vector2f) pos_mouse_win);//setando o martelo para a posicao do mouse
        }

        //Gameover
        if (gameover) {
            music.stop();//parar a musica de fundo
            gameover_sound.play();
            window.clear(sf::Color::Black);
            window.draw(gameover_text);
            window.draw(score);
            window.draw(f1);
            window.setMouseCursorVisible(true);
            window.display();

            if(sf::Keyboard::isKeyPressed(sf::Keyboard::F1)){
                window.setMouseCursorVisible(false);
                objs.clear();
                vidas = 5;
                points = 0;
                score.setString("Pontos: " + std::to_string(points));
                life.setString("Vidas: " + std::to_string(vidas));
                velocidade = 5.f;
                gameover = false;
            }


        } else {

            //Jogo Pausado?
            if (paused) {
                music.pause();//pausar a musica de fundo
                window.clear();
                window.draw(stop_sprite);
                window.draw(paused_text);
                window.display();

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
                    paused = false;
                }

                ham.setPosition(static_cast<sf::Vector2f>(current_position));

            } else {

                //Adicionar objects do nosso vetor com atrasos
                if (objs.size() < max_objs) {
                    if (obj_vel >= obj_vel_mx) {
                        float x = static_cast<float>(std::experimental::randint(10, (int) (window.getSize().x - texture.getSize().x - 10)));
                        object.setPosition(x, 0.f);
                        objs.push_back(object);
                        obj_vel = 0.f;
                    } else {
                        obj_vel += 1.f;
                    }
                }

                //mover e deletar os objetos do Vetor
                for (size_t i{}; i < objs.size(); ++i) {
                    bool del = false;
                    objs[i].move(0.f, velocidade);  

                    if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !pressed) {//quando precionado o click do lado esquerdo do mouse
                        if (objs[i].getGlobalBounds().contains(pos_mouse_coord)) {//verifica se a posicao da coordenada do mouse esta contida dentro do "object" Rectangle
                            pop_sound.play();
                            del = true;
                            points += 10.f;
                            score.setString("Pontos: " + std::to_string(points));
                            pressed = true;

                            //a cada 100 pontos o jogo aumenta a velocidade
                            if (points % 100 == 0) {
                                velocidade += 0.1f;
                                //std::cout << "Aumento a velocidade e agora ele é: " << velocidade << '\n';
                            }

                            //a cada 200 pontos ganha um bonus
                            if (points % 200 == 0) {
                                ++vidas;
                                //std::cout << "Ganhou mais uma vida, vidas Atual: " << vidas << '\n';
                            }
                        }
                    } 

                    //se posição do objeto no eixo Y for maior que o tamanho da tela no eixo Y então seta para deletar
                    if (objs[i].getPosition().y > window.getSize().y) {
                        --vidas;
                        life.setString("Vidas: " + std::to_string(vidas));
                        del = true;

                        if (vidas <= 0) {
                            std::cout << "Points: " << points << '\n';
                            std::cout << "GAME OVER, Vidas: " << vidas << '\n';
                            gameover = true;
                        }
                    }
                
                    //deletando o objeto
                    if(del) {
                        objs.erase(objs.begin() + i);
                    }
                }
                

                //Desenhando na tela
                window.clear();
                window.draw(fundo); 
                window.draw(score);  
                window.draw(life);  

                //desenhando os objetos
                for(auto &e : objs) {
                    window.draw(e);    
                }

                window.draw(ham);
                window.display();

            }//Fim do Else do Pause

        }//fim do Else do gameover

        
       
    }//WHILE LOOP PRINCIPAL

    //delete object;
    //object = nullptr;

    return EXIT_SUCCESS;
}

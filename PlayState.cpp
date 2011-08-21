/**
Copyright (C) 2011 babin philippe

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.*/

#include "PlayState.hpp"
#include "GameEngine.hpp"

/**
    Construction des éléments du jeu
**/
PlayState::PlayState(GameEngine* theGameEngine): m_playerOne(0),m_map(0)
,m_score(0),m_dMovX(0),m_dMovY(0),m_draMovX(true),m_draMovY(true),m_end(false),m_dead(false),m_level(1),m_vitesseDragon(0.07)
,m_scoreIcon(GameConfig::g_imgManag["coin"].img,GameConfig::GameConfig::g_imgManag["coin"].nbrCollum,GameConfig::GameConfig::g_imgManag["coin"].nbrLine)
,m_gameEngine(theGameEngine){

    m_playerOne= new Player(&m_map);

    m_map =new MapTile(&(*m_gameEngine).m_app,m_playerOne);

    m_mapEntity=m_map->GetMapEntity();
    m_playerOne->SetMapObject(m_mapEntity);

    m_map->GenerateMap(3,300,20,6,10,2,3,1);

    m_scoreIcon.setDelay(0.1);
    m_scoreIcon.play();
    m_scoreIcon.loop(true);
    m_scoreIcon.Scale(0.5,0.5);

    m_escape.SetTexture(GameConfig::g_imgManag["escape"].img);
    m_escape.SetScale(0.5,0.5);
    m_escape.SetOrigin(GameConfig::g_imgManag["escape"].img.GetWidth()/2,GameConfig::g_imgManag["escape"].img.GetHeight()/2);

    m_dragon.SetTexture(GameConfig::g_imgManag["dragon"].img);
    m_dragon.SetScale(2,2);
    m_dragon.SetOrigin(GameConfig::g_imgManag["dragon"].img.GetWidth()/2,0);
    m_dragon.SetPosition(m_playerOne->GetPosition());
    m_dragon.Move(-200,0);


    m_road.SetTexture(GameConfig::g_imgManag["road"].img);
    m_road.SetOrigin(GameConfig::g_imgManag["road"].img.GetWidth()/2,0);

    m_arrowDragon.SetTexture(GameConfig::g_imgManag["arrowdragon"].img);
    m_arrowDragon.SetOrigin(GameConfig::g_imgManag["arrowdragon"].img.GetWidth()/2,GameConfig::g_imgManag["arrowdragon"].img.GetHeight());

    m_arrow.SetTexture(GameConfig::g_imgManag["arrow"].img);
    m_arrow.SetOrigin(GameConfig::g_imgManag["arrow"].img.GetWidth()/2,GameConfig::g_imgManag["arrow"].img.GetHeight());

   m_camera=sf::View(m_playerOne->GetViewRect());
   m_font.LoadFromFile("font/pixel.ttf");
   m_levelInfo.SetFont(m_font);
   m_scoreInfo.SetFont(m_font);

    m_fade= sf::Shape::Rectangle(0,0,(int)GameConfig::g_config["screenwidth"]/4,(int)GameConfig::g_config["screenheight"]/4,sf::Color::White);

    m_deadSound.SetBuffer(GameConfig::g_soundManag["dead"]);
    m_checkpoint.SetBuffer(GameConfig::g_soundManag["checkpoint"]);
    m_coin.SetBuffer(GameConfig::g_soundManag["pickup_coin"]);
    m_escapeSound.SetBuffer(GameConfig::g_soundManag["escape"]);

    m_music.OpenFromFile("sounds/music.wav");
    m_music.SetLoop(true);
    m_music.Play();
}
/**
    Initialisation des éléments du jeu
    Cette méthode est appelé lors démarrage ou du redémarrage du state
**/
void PlayState::init(){
    resume();
}
/**
    Exécution des éléments
**/
void PlayState::loop(){
    //! On vérifie s'il a gagné.
    if(m_playerOne->GetPosition().x>(m_map->m_width-1)*GameConfig::g_config["tilewidth"] && !m_end){
        m_fade.SetColor(sf::Color(255,255,255,1));
        m_end=true;
        m_fadeIn=true;
        m_transition.Reset();
        m_escapeSound.Play();
    }
    if(m_end){
        int time=m_transition.GetElapsedTime();
        if(time>=1000 && m_fadeIn){
            m_fadeIn=false;
            m_level++;
            for(unsigned int i=0;i<m_mapEntity->size();i++){
                delete m_mapEntity->at(i);
                m_mapEntity->erase(m_mapEntity->begin()+i);
                i--;
            }
            if(m_level==2){
                m_map->GenerateMap(3,300,20,5,8,3,4,2);
                m_vitesseDragon=0.08;
            }
            if(m_level==3){
                m_map->GenerateMap(2,400,20,3,6,3,5,3);
                m_vitesseDragon=0.09;
            }
            if(m_level>=4){
                m_map->GenerateMap(3,400,20,2,4,3,6,4);
                m_vitesseDragon=0.12;
            }
            m_camera=sf::View(m_playerOne->GetViewRect());
            m_dragon.SetPosition(m_playerOne->GetPosition());
            m_dragon.Move(-200,0);
            m_gameEngine->m_app.SetView(m_camera);
        }
        if(m_fadeIn){
            m_fade.SetColor(sf::Color(255,255,255,time/1000.f*255.f));
        }
        else{
            m_fade.SetColor(sf::Color(255,255,255,255-(time-1000.f)/1000.f*255.f));
        }
    }
    if(m_end && m_transition.GetElapsedTime()>2000){
        m_end=false;
    }
//!S'il est mort
    if(m_dead){
        int time=m_transitionDead.GetElapsedTime();
        if(time>=1000 && m_fadeIn){
            for(int y=0;y<m_map->m_height;y++){
                for(int x=0;x<m_map->m_width;x++){
                    m_map->m_tileSet.at(x).at(y).tile.SetColor(sf::Color::White);
                    m_map->m_tileSet.at(x).at(y).touch=false;
                }
            }
            m_playerOne->SetPosition(m_map->m_spawnLocationOne);
            m_camera.SetCenter(m_map->m_spawnLocationOne);

            m_dragon.SetPosition(m_playerOne->GetPosition());
            m_dragon.Move(-200,0);
            m_gameEngine->m_app.SetView(m_camera);
            m_fadeIn=false;
        }
        if(m_fadeIn){
            m_fade.SetColor(sf::Color(255,255,255,time/1000.f*255.f));
        }
        else{
            m_fade.SetColor(sf::Color(255,255,255,255-(time-1000.f)/1000.f*255.f));
        }
    }
    if(m_dead && m_transitionDead.GetElapsedTime()>2000){
        m_dead=false;
    }
    if(!m_end&&!m_dead){
        /**
            Gestion des entrées claviers
        */
        //! Pauser le jeu
       if(sf::Keyboard::IsKeyPressed(sf::Keyboard::Return))pause();
        //! Control du joueur
        if (sf::Keyboard::IsKeyPressed(sf::Keyboard::X))m_playerOne->Jump();
        m_playerOne->Turn(sf::Keyboard::IsKeyPressed(sf::Keyboard::Left),sf::Keyboard::IsKeyPressed(sf::Keyboard::Right));
        if(sf::Keyboard::IsKeyPressed(sf::Keyboard::Z))m_playerOne->Switch();


    //    const sf::Input &Input =m_gameEngine->m_app.GetInput();
    //
    //    //! Control du joueur 1
    //    if(Input.IsKeyDown(sf::Key::L))m_playerOne->Degat(-40);
    //    if(Input.IsKeyDown(sf::Key::X))m_playerOne->Jump();
    //    if(Input.IsKeyDown(sf::Key::Z))m_playerOne->Shoot();
    //    m_playerOne->TurnUp(Input.IsKeyDown(sf::Key::Up));
    //    m_playerOne->Turn(Input.IsKeyDown(sf::Key::Left),Input.IsKeyDown(sf::Key::Right));


        /**
            Gestion des personnages et objets
        */

     //! Déplacement du personnage 1
        movePlayer(*m_playerOne);

     //! Déplacement de la caméra
        m_camera.SetCenter(m_playerOne->GetPosition());
        m_gameEngine->m_app.SetView(m_camera);
        //m_camera.SetCenter(0.04*m_gameEngine->m_app.GetFrameTime()+m_camera.GetCenter().x, m_playerOne->GetPosition().y);

     //! Déplacement des objets
        moveObject();
         //! Déplacement du dragon
         if(m_draMovX && m_dMovX>0)m_draMovX=false;
         if(!m_draMovX && m_dMovX<-20.f)m_draMovX=true;

         if(m_draMovX)m_dMovX+=0.03*m_gameEngine->m_app.GetFrameTime();
         else m_dMovX-=0.03*m_gameEngine->m_app.GetFrameTime();

         if(m_draMovY && m_dMovY>40)m_draMovY=false;
         if(!m_draMovY && m_dMovY<20.f)m_draMovY=true;

         if(m_draMovY)m_dMovY+=0.01*m_gameEngine->m_app.GetFrameTime();
         else m_dMovY-=0.01*m_gameEngine->m_app.GetFrameTime();
            m_dragon.SetPosition(m_dragon.GetPosition().x/**+m_dMovX*/+m_vitesseDragon*m_gameEngine->m_app.GetFrameTime(),
                            m_gameEngine->m_app.GetView().GetCenter().y-m_gameEngine->m_app.GetView().GetSize().y/2+m_dMovY);

    }
    //! On met à jour les menus
    m_levelInfo.SetPosition(m_gameEngine->m_app.GetView().GetCenter().x-m_gameEngine->m_app.GetView().GetSize().x/2+5,
                        m_gameEngine->m_app.GetView().GetCenter().y-m_gameEngine->m_app.GetView().GetSize().y/2);
    stringstream ss;
    ss<<m_level;
    string text="Level - "+ ss.str();
    m_levelInfo.SetString(text);
    m_levelInfo.SetScale(0.25,0.25);
    m_levelInfo.SetCharacterSize(48);

    m_scoreInfo.SetPosition(m_gameEngine->m_app.GetView().GetCenter().x+m_gameEngine->m_app.GetView().GetSize().x/2-30,
                        m_gameEngine->m_app.GetView().GetCenter().y-m_gameEngine->m_app.GetView().GetSize().y/2);
    stringstream ss2;
    ss2<<m_score;
    string text2=ss2.str()+" x ";
    m_scoreInfo.SetString(text2);
    m_scoreInfo.SetScale(0.25,0.25);
    m_scoreInfo.SetCharacterSize(24);

    m_scoreIcon.SetPosition(m_gameEngine->m_app.GetView().GetCenter().x+m_gameEngine->m_app.GetView().GetSize().x/2-13,
                        m_gameEngine->m_app.GetView().GetCenter().y-m_gameEngine->m_app.GetView().GetSize().y/2);


    m_road.SetPosition(m_gameEngine->m_app.GetView().GetCenter().x,
                        m_gameEngine->m_app.GetView().GetCenter().y+m_gameEngine->m_app.GetView().GetSize().y/2-20);

    m_arrowDragon.SetPosition(m_gameEngine->m_app.GetView().GetCenter().x+2-GameConfig::g_imgManag["road"].img.GetWidth()/2+(m_dragon.GetPosition().x/GameConfig::g_config["tilewidth"])/m_map->m_width*100,
                        m_gameEngine->m_app.GetView().GetCenter().y+m_gameEngine->m_app.GetView().GetSize().y/2-20);

    m_arrow.SetPosition(m_gameEngine->m_app.GetView().GetCenter().x+2-GameConfig::g_imgManag["road"].img.GetWidth()/2+(m_playerOne->GetPosition().x/GameConfig::g_config["tilewidth"])/m_map->m_width*100,
                        m_gameEngine->m_app.GetView().GetCenter().y+m_gameEngine->m_app.GetView().GetSize().y/2-20);

    m_escape.SetPosition(m_gameEngine->m_app.GetView().GetCenter());


    m_fade.SetPosition(m_gameEngine->m_app.GetView().GetCenter().x-m_gameEngine->m_app.GetView().GetSize().x/2,
                        m_gameEngine->m_app.GetView().GetCenter().y-m_gameEngine->m_app.GetView().GetSize().y/2);
}
/**
    Pause le jeu
    Appelé lors d'un changement de state
**/
void PlayState::pause(){
    m_playerOne->Pause();
    for(unsigned int i=0;i<m_mapEntity->size();i++){
        m_mapEntity->at(i)->pause();
    }
    //! On change le state principale
    m_gameEngine->changeState(2);
    m_transition.Pause();
    m_transitionDead.Pause();
}
/**
    Démarrage après une pause
**/
void PlayState::resume(){
    m_playerOne->Resume();
    for(unsigned int i=0;i<m_mapEntity->size();i++){
        if(!m_mapEntity->at(i)->isStop())m_mapEntity->at(i)->play();
    }
    m_transition.Play();
    m_transitionDead.Play();
}
/**
    Remet à zéro
**/
void PlayState::stop(){
}
/**
    Utilisation des entrés
**/
void PlayState::GetEvents(sf::Event){
}
/**
    Affichage des éléments
**/
void PlayState::draw(){
    m_map->Draw();
    m_gameEngine->m_app.Draw(m_dragon);
    m_gameEngine->m_app.Draw(m_levelInfo);
    m_gameEngine->m_app.Draw(m_scoreInfo);
    m_gameEngine->m_app.Draw(m_scoreIcon);
    m_gameEngine->m_app.Draw(m_road);
    m_gameEngine->m_app.Draw(m_arrowDragon);
    m_gameEngine->m_app.Draw(m_arrow);

    if(m_dead||m_end)m_gameEngine->m_app.Draw(m_fade);

    if(m_end)m_gameEngine->m_app.Draw(m_escape);


}

/**
    Déplacement d'un Player dans la map
**/
void PlayState::movePlayer(Player &player){
    float movHor=0;
    float movVer=0;
    int limitVer=0;
    int limitHor=0;
    float movHorTest=player.GetVelx()*m_gameEngine->m_app.GetFrameTime()/1000.f;
    float movVerTest=player.GetVely()*m_gameEngine->m_app.GetFrameTime()/1000.f;
    bool bas=false, haut=false, gauche=false, droite=false, kill=false;
    //! On vérifie les collisions horizontals
    if(!player.CollisionHorizontal(player.GetMovedPlayerRect(movHorTest,0),gauche,droite,limitHor)){//! Pas de collision
        movHor=movHorTest;
    }
    else{//! Sinon on reposition le joueur
        player.SetVelx(0);
        if(gauche)movHor=((((limitHor+1)*GameConfig::g_config["tilewidth"]))-player.GetPosition().x)/1000.f;
        if(droite)movHor=((((limitHor)*GameConfig::g_config["tilewidth"]))-GameConfig::g_config["playercollwidth"]-player.GetPosition().x)/1000.f;
    }

    //! On vérifie les collisions vertical
    if(!player.CollisionVertical(player.GetMovedPlayerRect(0,movVerTest),haut,bas,limitVer)){//! Pas de collision
        player.Gravity(m_gameEngine->m_app);
        movVer=movVerTest;
    }
    else{//! Sinon on reposition le joueur
        if(haut){//! Si l'on touche le haut
            player.SetVely(0);
        }
        if(bas){//! Si l'on touche le sol
            if(!player.GetBottomCollision())movVer=(player.GetPosition().y-(limitVer*GameConfig::g_config["tileheight"])+GameConfig::g_config["playercollheight"])/1000.f;
            player.UnlockJump();
            player.SetBottomCollision(true);
        }
    }

    //! On vérifie si le mouvement envisagé cause une collision
    if(!player.CollisionGeneral(player.GetMovedPlayerRect(movHor,movVer),kill)&&movHor<GameConfig::g_config["tileheight"]&&movVer<GameConfig::g_config["tilewidth"])player.Move(movHor,movVer);
    else player.SetVely(0);

    //! Ouch!
    if(kill||m_playerOne->GetPosition().x<m_dragon.GetPosition().x+GameConfig::g_imgManag["dragon"].img.GetWidth()/2){
        m_transitionDead.Reset();
        m_fade.SetColor(sf::Color(255,255,255,1));
        m_deadSound.Play();
        m_dead=true;
        m_fadeIn=true;
    }
}

/**
    Déplacement des objets
**/
void PlayState::moveObject(){
    for(unsigned int i=0;i<m_mapEntity->size();i++){
        if(m_mapEntity->at(i)->isCollision()){
            //! On affiche détermine le rectangle de l'object
            sf::FloatRect Rect=m_mapEntity->at(i)->GetMovedRect(m_mapEntity->at(i)->GetVelx()*m_gameEngine->m_app.GetFrameTime()/1000.f,m_mapEntity->at(i)->GetVely()*m_gameEngine->m_app.GetFrameTime()/1000.f);
            //! On vérifie si l'object touche le joueur si oui on supprimer l'objet et crée un animation d'un explosion
            if((m_playerOne->GetPlayerRect().Intersects(Rect))){
                if(m_mapEntity->at(i)->collisionEffect(*m_playerOne)){
                    m_map->m_spawnLocationOne=m_mapEntity->at(i)->GetPosition();
                    m_checkpoint.Play();
                }
                else{
                    m_coin.Play();
                }
                //! On crée libère la mémoire de le l'instance de l'objet
                delete m_mapEntity->at(i);
                //! On supprime le pointeur du tableau dynamique
                m_mapEntity->erase(m_mapEntity->begin()+i);
                m_score++;
            }
        }
        else{
            m_mapEntity->at(i)->Move(m_mapEntity->at(i)->GetVelx()*m_gameEngine->m_app.GetFrameTime(),m_mapEntity->at(i)->GetVely()*m_gameEngine->m_app.GetFrameTime());
        }
    }
}

/**
    Déconstruction des éléments du jeu
**/
PlayState::~PlayState(){
    delete m_playerOne;
    delete m_map;
}

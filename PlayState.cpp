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
,m_scoreK(0),m_scoreP(0)
,m_gameEngine(theGameEngine){


    m_playerOne= new Player(&m_map);

    m_map =new MapTile(&(*m_gameEngine).m_app,m_playerOne);

    m_mapEntity=m_map->GetMapEntity();
    m_playerOne->SetMapObject(m_mapEntity);

    m_map->GenerateMap(3,300,20,6,10,3,6);
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
    /**
        Gestion des entrées claviers
    */


    //! Control du joueur 1
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
    m_gameEngine->m_app.SetView(sf::View(m_playerOne->GetViewRect()));

 //! Déplacement des objets
    moveObject();
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
}
/**
    Démarrage après une pause
**/
void PlayState::resume(){
    m_playerOne->Resume();
    for(unsigned int i=0;i<m_mapEntity->size();i++){
        if(!m_mapEntity->at(i)->isStop())m_mapEntity->at(i)->play();
    }
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
    if(kill)player.SetPosition(m_map->m_spawnLocationOne);
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
                //! On crée libère la mémoire de le l'instance de l'objet
                delete m_mapEntity->at(i);
                //! On supprime le pointeur du tableau dynamique
                m_mapEntity->erase(m_mapEntity->begin()+i);
                if(m_playerOne->IsPrincess())m_scoreP+=10;
                else m_scoreK+=10;
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

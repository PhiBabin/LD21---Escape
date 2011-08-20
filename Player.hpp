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

#ifndef PLAYER_HPP_INCLUDED
#define PLAYER_HPP_INCLUDED
class MapTile;
class Player:public ImgAnim{
    public:
    //! Construteur
        Player(MapTile **map);
    //! Affiche
        void Drawing(sf::RenderWindow* app);
    //! Retourne le rectangle de Camera
        sf::FloatRect GetViewRect();
    //! Retourne le rectangle de collision
        sf::FloatRect GetPlayerRect();
        sf::FloatRect GetMovedPlayerRect(const float moveX,const float moveY);
    //! Collision
        bool CollisionGeneral(const sf::FloatRect playerRect,bool &kill);
        bool CollisionHorizontal(const sf::FloatRect playerRect, bool &gauche, bool &droite,int &solidLimit);
        bool CollisionVertical(const sf::FloatRect playerRect, bool &haut, bool &bas,int &solidLimit);
        void SetBottomCollision(bool is);
        bool GetBottomCollision() const;
    //! Saut
        void Jump();
        void UnlockJump();
    //! Tire
        void Shoot();
    //! Déplacement
        void Turn(bool left, bool right);
        void TurnUp(bool up);
    //! Link la liste des entités
        void SetMapObject(vector<GameEntity*> *listObject);
    //! Vie et mort
        int GetHp();
        void SetHp(int nhp);
        void Degat(int degats);
        bool IsDead();
    //! Velocité
        void Gravity(sf::RenderWindow &app);
        float GetVelx();
        float GetVely();
        void SetVelx(float nx);
        void SetVely(float ny);
        void Pause();
        void Resume();
    //! Déconstruteur
        ~Player();
    private:
        MapTile **m_map;
        vector<GameEntity*> *m_listObject;

        int m_hp;
        float m_velx;
        float m_vely;
        bool m_jumpLock;
        bool m_colBot;
        bool m_direction;
        bool m_lookUp;
        bool m_moving;

        PausableClock m_lastShot;
        PausableClock m_hurt;
};
#endif // PLAYER_HPP_INCLUDED

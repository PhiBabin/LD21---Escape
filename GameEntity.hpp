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
#ifndef GameEntity_H
#define GameEntity_H
class Player;

class GameEntity:public ImgAnim{
    public:
        GameEntity(sf::Texture &img,int nbrFrame,int nbrLigne,float height, float width,float offsetColX,float offsetColY,bool col);
        sf::FloatRect GetRect()const;
        sf::FloatRect GetMovedRect(const float moveX,const float moveY)const;
        float GetVelx()const;
        float GetVely()const;
        virtual bool isDraw();
        virtual bool isDelete()const;
        virtual bool isCollision()const;
        virtual bool collisionEffect(Player &player)=0;
        virtual ~GameEntity();
    protected:
        float m_velx;
        float m_vely;
        bool m_col;
        float m_offsetColX;
        float m_offsetColY;
        float m_colWidth;
        float m_colHeight;
};

#endif // GameEntity_H

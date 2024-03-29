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

#ifndef GAMEANIM_H
#define GAMEANIM_H


class GameAnim: public GameEntity{
    public:
        GameAnim(sf::Texture &img,int nbrFrame,int nbrLigne,float time,bool col,float velx,float vely,float height, float width,float offsetColX,float offsetColY);
        virtual bool isDraw();
        virtual bool isDelete()const;
        virtual bool isCollision()const;
        virtual bool collisionEffect(Player &player);
        virtual ~GameAnim();
    private:
        unsigned int m_nbrFrame;
};

#endif // GAMEANIM_H

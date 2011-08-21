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

#include "Player.hpp"
Player::Player(MapTile **map):
ImgAnim::ImgAnim(GameConfig::g_imgManag["player"].img,GameConfig::g_imgManag["player"].nbrCollum,GameConfig::g_imgManag["player"].nbrLine)
,m_map(map)
,m_velx(0),m_vely(0),
m_jumpLock(false),m_colBot(false),m_direction(false),m_moving(false),m_princess(false)
{
   pause();
    m_jump.SetBuffer(GameConfig::g_soundManag["jump"]);
}

sf::FloatRect Player::GetPlayerRect(){
    return sf::FloatRect(GetPosition().x,GetPosition().y,GameConfig::g_config["playercollwidth"],GameConfig::g_config["playercollheight"]);
}
sf::FloatRect Player::GetMovedPlayerRect(const float moveX,const float moveY){
  return sf::FloatRect(GetPosition().x+moveX,GetPosition().y+moveY,GameConfig::g_config["playercollwidth"],GameConfig::g_config["playercollheight"]);
}
sf::FloatRect Player::GetViewRect(){
   return sf::FloatRect(GetPosition().x-(int)GameConfig::g_config["screenwidth"]/8,GetPosition().y-(int)GameConfig::g_config["screenheight"]/8,(int)GameConfig::g_config["screenwidth"]/4,(int)GameConfig::g_config["screenheight"]/4);
}

void Player::Gravity(sf::RenderWindow &app){
        m_vely+=GameConfig::g_config["gravity"]/1000.f*app.GetFrameTime();
}
void Player::Jump(){
    if(!m_jumpLock){
        m_jumpLock=true;
        m_vely+=GameConfig::g_config["jump"];
        SetBottomCollision(false);
        m_jump.Play();
    }
}
void Player::Turn(bool left, bool right){

    if(left&&!right){
        m_moving=true;
        m_direction=GAUCHE;
        m_velx=-100;
    }
    else if(!left&&right){
        m_moving=true;
        m_direction=DROITE;
        m_velx=100;
    }
    else{
        m_moving=false;
        m_velx*=0.8;
    }
    FlipX(m_direction);
}
 bool Player::CollisionGeneral(const sf::FloatRect playerRect,bool &kill){
    int maxHeight, minHeight, maxWidth, minWidth;
    bool collision=false;
    minHeight=playerRect.Top/GameConfig::g_config["tileheight"];
    minWidth=playerRect.Left/GameConfig::g_config["tilewidth"];
    maxHeight=(playerRect.Top+playerRect.Height-1)/GameConfig::g_config["tileheight"];
    maxWidth=(playerRect.Left+playerRect.Width-1)/GameConfig::g_config["tilewidth"];

    if(minHeight<0)minHeight=0;
    if(maxHeight>(*m_map)->m_height)maxHeight=(*m_map)->m_height;
    if(minWidth<0)minWidth=0;
    if(maxWidth>(*m_map)->m_width)maxWidth=(*m_map)->m_width;
    for(int y=minHeight;y<=maxHeight;y++){
        for(int x=minWidth;x<=maxWidth;x++){
            if(!(x>=(*m_map)->m_width or y>=(*m_map)->m_height)){
                if((*m_map)->Tile(x,y).kill)kill=true;
                if(((*m_map)->Tile(x,y).fall && IsPrincess() && !(*m_map)->Tile(x,y).princess)||
                ((*m_map)->Tile(x,y).fall && !IsPrincess() && (*m_map)->Tile(x,y).princess)||
                !(*m_map)->Tile(x,y).fall){
                    if((*m_map)->Tile(x,y).solid && !((*m_map)->Tile(x,y).touch && (*m_map)->Tile(x,y).tile.GetColor().a==0)){
                        if((*m_map)->Tile(x,y).fall)(*m_map)->m_tileSet.at(x).at(y).touch=true;
                        sf::FloatRect  theTile(x*GameConfig::g_config["tilewidth"],y*GameConfig::g_config["tileheight"],GameConfig::g_config["tilewidth"],GameConfig::g_config["tileheight"]);
                        if(playerRect.Intersects(theTile)||theTile.Intersects(playerRect)){
                            collision= true;
                        }
                    }
                }
            }
        }
    }
    return collision;
 }
 bool Player::CollisionVertical(const sf::FloatRect playerRect, bool &haut, bool &bas,int &solidLimit){
    int maxHeight, minHeight, maxWidth, minWidth;
    bool CollisionVertical=false;
    minHeight=playerRect.Top/GameConfig::g_config["tileheight"];
    minWidth=playerRect.Left/GameConfig::g_config["tilewidth"];
    maxHeight=(playerRect.Top+playerRect.Height-1)/GameConfig::g_config["tileheight"];
    maxWidth=(playerRect.Left+playerRect.Width-1)/GameConfig::g_config["tilewidth"];

    if(minHeight<0)minHeight=0;
    if(maxHeight>(*m_map)->m_height)maxHeight=(*m_map)->m_height;
    if(minWidth<0)minWidth=0;
    if(maxWidth>(*m_map)->m_width)maxWidth=(*m_map)->m_width;
    for(int y=minHeight;y<=maxHeight;y++){
        for(int x=minWidth;x<=maxWidth;x++){
            if(!(x>=(*m_map)->m_width or y>=(*m_map)->m_height)){
                if(((*m_map)->Tile(x,y).fall && IsPrincess() && !(*m_map)->Tile(x,y).princess)||
                ((*m_map)->Tile(x,y).fall && !IsPrincess() && (*m_map)->Tile(x,y).princess)||
                !(*m_map)->Tile(x,y).fall){
                    if((*m_map)->Tile(x,y).solid && !((*m_map)->Tile(x,y).touch && (*m_map)->Tile(x,y).tile.GetColor().a==0)){
                        sf::FloatRect  theTile(x*GameConfig::g_config["tilewidth"],y*GameConfig::g_config["tileheight"],GameConfig::g_config["tilewidth"],GameConfig::g_config["tileheight"]);
                        if(playerRect.Intersects(theTile)||theTile.Intersects(playerRect)){
                            CollisionVertical=true;
                            if((*m_map)->Tile(x,y).fall)(*m_map)->m_tileSet.at(x).at(y).touch=true;
                            if(y*GameConfig::g_config["tileheight"]<=playerRect.Top+playerRect.Height&&y*GameConfig::g_config["tileheight"]>=playerRect.Top){
                                bas=true;
                                solidLimit=y;
                            }
                            if((y+1)*GameConfig::g_config["tileheight"]>=playerRect.Top&&(y+1)*GameConfig::g_config["tileheight"]<=playerRect.Top+playerRect.Height){
                                haut=true;
                            }
                        }
                    }
                }
            }
        }
    }
    return CollisionVertical;
 }
 bool Player::CollisionHorizontal(const sf::FloatRect playerRect, bool &gauche, bool &droite,int &solidLimit){
    int maxHeight, minHeight, maxWidth, minWidth;
    bool CollisionHorizontal=false;
    minHeight=playerRect.Top/GameConfig::g_config["tileheight"];
    minWidth=playerRect.Left/GameConfig::g_config["tilewidth"];
    maxHeight=(playerRect.Top+playerRect.Height-1)/GameConfig::g_config["tileheight"];
    maxWidth=(playerRect.Left+playerRect.Width-1)/GameConfig::g_config["tilewidth"];

    if(minHeight<0)minHeight=0;
    if(maxHeight>(*m_map)->m_height)maxHeight=(*m_map)->m_height;
    if(minWidth<0)minWidth=0;
    if(maxWidth>(*m_map)->m_width)maxWidth=(*m_map)->m_width;
    for(int y=minHeight;y<=maxHeight;y++){
        for(int x=minWidth;x<=maxWidth;x++){
            if(!(x>=(*m_map)->m_width or y>=(*m_map)->m_height)){
                if(((*m_map)->Tile(x,y).fall && IsPrincess() && !(*m_map)->Tile(x,y).princess)||
                ((*m_map)->Tile(x,y).fall && !IsPrincess() && (*m_map)->Tile(x,y).princess)||
                !(*m_map)->Tile(x,y).fall){
                    if((*m_map)->Tile(x,y).solid && !((*m_map)->Tile(x,y).touch && (*m_map)->Tile(x,y).tile.GetColor().a==0)){
                        sf::FloatRect  theTile(x*GameConfig::g_config["tilewidth"],y*GameConfig::g_config["tileheight"],GameConfig::g_config["tilewidth"],GameConfig::g_config["tileheight"]);
                        if(playerRect.Intersects(theTile)||theTile.Intersects(playerRect)){
                            CollisionHorizontal= true;
                            if((*m_map)->Tile(x,y).fall)(*m_map)->m_tileSet.at(x).at(y).touch=true;
                            if(x*GameConfig::g_config["tilewidth"]>=playerRect.Left&&x*GameConfig::g_config["tilewidth"]<=playerRect.Left+playerRect.Width){
                                droite=true;
                                solidLimit=x;
                            }
                            if((x+1)*GameConfig::g_config["tilewidth"]<=playerRect.Left+playerRect.Width&&(x+1)*GameConfig::g_config["tilewidth"]>=playerRect.Left){
                                gauche=true;
                                solidLimit=x;
                            }
                        }
                    }
                }

            }
        }
    }
    return CollisionHorizontal;
 }
void Player::SetMapObject(vector<GameEntity*> *listObject){
    m_listObject=listObject;
}
bool Player::IsDead(){
    return true;
}
float Player::GetVelx(){
    return m_velx;
}
float Player::GetVely(){
    return m_vely;
}
void Player::SetVelx(float nx){
    m_velx=nx;
}
void Player::SetVely(float ny){
    m_vely=ny;
}
void Player::SetBottomCollision(bool is){
    m_colBot=is;
}
bool Player::GetBottomCollision() const{
    return m_colBot;
}
void Player::UnlockJump(){
    m_jumpLock=false;
    m_vely=0;
}
bool Player::IsPrincess(){
    return m_princess;
}
void Player::Switch(){
    if(m_switch.GetElapsedTime()>500){
        if(m_princess)m_princess=false;
        else m_princess=true;
        m_switch.Reset();
    }
}
void Player::Drawing(sf::RenderWindow* app){
    if(m_princess)setAnimRow(1);
    else setAnimRow(0);
}
void Player::Pause(){
    m_switch.Pause();
}
void Player::Resume(){
    m_switch.Play();
}
Player::~Player(){
}

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
#ifndef MAP_HPP_INCLUDED
#define MAP_HPP_INCLUDED
struct Type{
    //! Propriétés
	bool visible, solid, kill;
	//! La couleur correspondante
	sf::Color   colorPix;
	//! Le rectangle visible du tileset
	sf::IntRect zoneRect;
	sf::Sprite tile;};
class MapTile{
	public:
        MapTile();
        MapTile(sf::RenderWindow *App,Player *playerOne);
		void LoadMap();
		void Draw();
        vector<Type> & operator [] (int X);
        unsigned char FindType(sf::Color Pix);
        bool CollisionTile(float x,float y);
        Type Tile(float x, float y);
        vector<GameEntity*> * GetMapEntity();
        bool CollisionGeneral(const sf::FloatRect playerRect);
        ~MapTile();

        //! Public variable
		int m_width, m_height;
		sf::Vector2f m_spawnLocationOne;
		sf::Vector2f m_spawnLocationTwo;
	private:
        sf::RenderWindow *m_app;
		sf::Texture m_ImgTypeTile;
		sf::Sprite m_drawSprite;
		vector <vector<Type> > m_tileSet;
		vector <Type> m_typeList;
		vector <GameEntity*> m_mapEntity;
		Player *m_playerOne;
		sf::RenderTexture m_map;
		sf::RenderTexture m_background;
};


#endif // MAP_HPP_INCLUDED

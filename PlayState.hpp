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

#ifndef PLAYSTATE_HPP
#define PLAYSTATE_HPP
class PlayState: public GameState {
    public:
        PlayState(GameEngine* theGameEngine);
        virtual void init();
        virtual void loop();
        virtual void stop();
        virtual void pause();
        virtual void resume();
        virtual void GetEvents(sf::Event eventNew);
        virtual void draw();
        void moveObject();
        void movePlayer(Player &player);
        virtual ~PlayState();
    private:
        Player *m_playerOne;
        MapTile *m_map;
        sf::View m_camera;
        bool m_start,m_end, m_dead, m_draMovX, m_draMovY;
        int m_score, m_level;
        float m_vitesseDragon;
        sf::Text m_levelInfo;
        sf::Text m_scoreInfo;
        ImgAnim m_scoreIcon;
        sf::Sprite m_dragon;
        sf::Sprite m_escape;
        sf::Sprite m_road;
        sf::Sprite m_arrow;
        sf::Sprite m_arrowDragon;
        float m_dMovX, m_dMovY;
        PausableClock m_transition,m_transitionDead;
        sf::Font m_font;
        sf::Shape m_fade;
        bool m_fadeIn;
        sf::Sound m_deadSound, m_checkpoint, m_coin, m_escapeSound;
        sf::Music m_music;


        vector<GameEntity*> *m_mapEntity;
        GameEngine *m_gameEngine;
};

#endif // PLAYSTATE_HPP

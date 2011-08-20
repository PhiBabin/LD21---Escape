#ifndef GAMECONFIG_H
#define GAMECONFIG_H


struct imgAnim{
    sf::Texture img;
    int nbrCollum;
    int nbrLine;
};
class GameConfig
{
    public:
        GameConfig();
        static void LoadConfig();
        ~GameConfig();

        static map<string,int> g_config;
        static map<string,imgAnim> g_imgManag;
        static map<string,sf::SoundBuffer> g_soundManag;
};

#endif // GAMECONFIG_H

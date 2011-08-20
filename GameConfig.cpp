#include "GameConfig.hpp"

/**
    On déclare les attributs statics
**/
map<string,int> GameConfig::g_config;

map<string,sf::SoundBuffer> GameConfig::g_soundManag;

map<string,imgAnim> GameConfig::g_imgManag;

/**
    Constructeur
**/
GameConfig::GameConfig()
{
}
/**
    Chargement de la configuration
**/
void GameConfig::LoadConfig(){
    TiXmlDocument doc("config/config.xml");
    doc.LoadFile();

    TiXmlHandle hDoc(&doc);
    TiXmlHandle hRoot(0);
    TiXmlElement* pElem;

    pElem=hDoc.FirstChild("img").FirstChild().Element();
    for(; pElem; pElem=pElem->NextSiblingElement()){
        imgAnim newAnim;
        sf::Texture newImg;
        if(!newImg.LoadFromFile(pElem->Attribute("path"))){
            cerr<<"Image not found: "<<pElem->Attribute("path")<<endl;
            exit(0);
        }
        newAnim.img=newImg;
        newAnim.nbrCollum=atoi(pElem->Attribute("nbrCollums"));
        newAnim.nbrLine=atoi(pElem->Attribute("nbrLines"));
        g_imgManag[pElem->Attribute("name")]=newAnim;
    }
    pElem=hDoc.FirstChild("sound").FirstChild().Element();
    for(; pElem; pElem=pElem->NextSiblingElement()){
        sf::SoundBuffer newSound;
        newSound.LoadFromFile(pElem->Attribute("path"));
        g_soundManag[pElem->Attribute("name")]=newSound;
    }
    pElem=hDoc.FirstChild("config").FirstChild().Element();
    for(; pElem; pElem=pElem->NextSiblingElement()){
        GameConfig::g_config[pElem->Attribute("name")]=atoi(pElem->Attribute("value"));
    }
}

GameConfig::~GameConfig()
{
}

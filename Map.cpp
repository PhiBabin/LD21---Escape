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

#include "Map.hpp"
MapTile::MapTile():m_app(NULL),m_playerOne(NULL){}
MapTile::MapTile(sf::RenderWindow *App, Player *playerOne):
m_width(0),m_height(0),m_app(App),m_playerOne(playerOne){

    LoadMap();
}

 bool MapTile::CollisionTile(float x, float y){
    return m_tileSet.at(x).at(y).solid;
 }

Type MapTile::Tile(float x, float y){
    return (m_tileSet.at(x)).at(y);
 }

 vector<GameEntity*> * MapTile::GetMapEntity(){
    return &m_mapEntity;
 }


 bool MapTile::CollisionGeneral(const sf::FloatRect playerRect){
    int maxHeight, minHeight, maxWidth, minWidth;
    minHeight=playerRect.Top/GameConfig::g_config["tileheight"];
    minWidth=playerRect.Left/GameConfig::g_config["tilewidth"];
    maxHeight=(playerRect.Top+playerRect.Height-1)/GameConfig::g_config["tileheight"];
    maxWidth=(playerRect.Left+playerRect.Width-1)/GameConfig::g_config["tilewidth"];

    if(minHeight<0)minHeight=0;
    if(maxHeight>m_height)maxHeight=m_height;
    if(minWidth<0)minWidth=0;
    if(maxWidth>m_width)maxWidth=m_width;
    for(int y=minHeight;y<=maxHeight;y++){
        for(int x=minWidth;x<=maxWidth;x++){
            if(!(x>=m_width or y>=m_height)){
                if(m_tileSet[x][y].solid){
                    sf::FloatRect  theTile(x*GameConfig::g_config["tilewidth"],y*GameConfig::g_config["tileheight"],GameConfig::g_config["tilewidth"],GameConfig::g_config["tileheight"]);
                    if(playerRect.Intersects(theTile)||theTile.Intersects(playerRect)) return true;
                }
            }
        }
    }
    return false;
 }

void MapTile::Draw(){
    cout<<"FPS="<</*1.f/(m_app->GetFrameTime())*1000<<*/"Joueur 1 x="<<m_playerOne->GetPosition().x/GameConfig::g_config["tilewidth"]
    <<" y="<<m_playerOne->GetPosition().y<<" vely="<<m_playerOne->GetVely()<<" velx="<<m_playerOne->GetVelx()<<endl;
    //! On affiche les tiles du foreground
    float falling;
    for(int y=0;y<m_height;y++){
        for(int x=0;x<m_width;x++){
            if((m_tileSet.at(x)).at(y).fall&& (m_tileSet.at(x)).at(y).touch){
                falling=(m_tileSet.at(x)).at(y).tile.GetColor().a-0.2*m_app->GetFrameTime();
                if(falling<0)falling=0;
                (m_tileSet.at(x)).at(y).tile.SetColor(sf::Color(255,255,255,falling));
            }
            if((m_tileSet.at(x)).at(y).visible){
                m_app->Draw(m_tileSet.at(x).at(y).tile);
            }
        }
    }
    //! On affiche le personnage et ces éléments
    m_app->Draw(*m_playerOne);
    m_playerOne->Drawing(m_app);
    //! On affiche les objets de la carte
    for(unsigned int i=0;i<m_mapEntity.size();i++){
        if((m_mapEntity.at(i))->isDelete()){
            delete m_mapEntity.at(i);
            m_mapEntity.erase( m_mapEntity.begin() + i );
        }
        else{
            m_app->Draw(*(m_mapEntity.at(i)));
        }
    }
}
vector<Type> & MapTile::operator [] (int X){
    return m_tileSet.at(X);
}
 unsigned char MapTile::FindType(sf::Color Pix){
        for(unsigned char it=0;it<m_typeList.size();it++){
            if(m_typeList[it].colorPix==Pix) return it;
        }
        cerr<<"[Fatal Error]Impossible de trouver un type correspondant."<<endl;
        exit(0);
        return 0;
 }
void MapTile::GenerateMap(int rate,int wmap,int hmap, int pmin, int pmax, int dmin, int dmax){
	m_tileSet.erase(m_tileSet.begin(),m_tileSet.end());
    m_width=wmap;
    m_height=hmap;
    //! On vide la map
    vector<Type> emptyVec;
    emptyVec.resize(hmap);
    m_tileSet.resize(wmap,emptyVec);
    for(int y=0;y<hmap;y++){
        for(int x=0;x<wmap;x++){
            (m_tileSet.at(x)).at(y)=m_typeList[VIDE];
            (m_tileSet.at(x)).at(y).tile.SetPosition(x*GameConfig::g_config["tilewidth"],y*GameConfig::g_config["tileheight"]);
            (m_tileSet.at(x)).at(y).tile.SetSubRect(m_typeList[VIDE].zoneRect);
        }
    }
    //! On met le premier palier
    int firstP=rand() % (hmap/2) + 3;
    int longerP=rand() % pmax + pmin;
    for(int x=0;x<longerP;x++){
        (m_tileSet.at(x)).at(firstP)=m_typeList[BLOCK];
        (m_tileSet.at(x)).at(firstP).tile.SetPosition(x*GameConfig::g_config["tilewidth"],firstP*GameConfig::g_config["tileheight"]);
        (m_tileSet.at(x)).at(firstP).tile.SetSubRect(m_typeList[BLOCK].zoneRect);
    }
    //! On met le spawn
    sf::Vector2f spawnLocationOne(3*GameConfig::g_config["tilewidth"] ,(firstP-1)*GameConfig::g_config["tileheight"]-GameConfig::g_config["playercollheight"]);
    m_spawnLocationOne=spawnLocationOne;
    m_playerOne->SetPosition(m_spawnLocationOne);

    int cursor= longerP+3;
    int hight= firstP+rand() % 3 - 3;
    int trans=KNIGHT;
    while(cursor+longerP<wmap){
        if(rand() % rate +1 == 1)trans=PRINCESS;
        else trans=KNIGHT;
        for(int x=cursor;x<cursor+longerP;x++){
            (m_tileSet.at(x)).at(hight)=m_typeList[trans];
            (m_tileSet.at(x)).at(hight).tile.SetPosition(x*GameConfig::g_config["tilewidth"],hight*GameConfig::g_config["tileheight"]);
            (m_tileSet.at(x)).at(hight).tile.SetSubRect(m_typeList[trans].zoneRect);
            m_mapEntity.push_back(new GameAnim(GameConfig::g_imgManag["coin"].img,GameConfig::GameConfig::g_imgManag["coin"].nbrCollum,GameConfig::GameConfig::g_imgManag["coin"].nbrLine));
            m_mapEntity.back()->SetPosition(x*GameConfig::g_config["tilewidth"],(hight-1)*GameConfig::g_config["tileheight"]);
            m_mapEntity.back()->setDelay(0.1);
            m_mapEntity.back()->SetScale(0.5,0.5);
        }
        cursor+=longerP+rand() % dmax + dmin;
        if(hight+5>wmap)hight-=rand() % 4 +1;
        if(hight-5<0)hight+=rand() % 4 +1;
        else hight+=rand() % 4 -5;
        longerP=rand() % pmax + pmin;
    }

    //! On met le sol mortel
    for(int x=0;x<wmap;x++){
        (m_tileSet.at(x)).at(hmap-1)=m_typeList[KILL];
        (m_tileSet.at(x)).at(hmap-1).tile.SetPosition(x*GameConfig::g_config["tilewidth"],(hmap-1)*GameConfig::g_config["tileheight"]);
        (m_tileSet.at(x)).at(hmap-1).tile.SetSubRect(m_typeList[KILL].zoneRect);
    }


}
void MapTile::LoadMap(){
    map<string,string> levelConfig;
	int typeSpawn1,typeSpawn2;
	sf::Vector2f backbackCoor;

    //! On charge la configuration du niveau
    stringstream ss;
    ss<<GameConfig::g_config["level"];
    string pathConfig="map/level"+ ss.str()+".xml";

    TiXmlDocument doc(pathConfig.c_str());
    doc.LoadFile();

    TiXmlHandle hRoot(&doc);
    TiXmlHandle hDoc(0);
    TiXmlElement* pElem;

    hDoc=TiXmlHandle(hRoot.FirstChildElement().Element());

    pElem=hDoc.FirstChild("metadata").FirstChild().Element();
    for(; pElem; pElem=pElem->NextSiblingElement()){
        levelConfig[pElem->Attribute("name")]=string(pElem->Attribute("value"));
///       if(pElem->Attribute("name")==string("backbackpath"))backbackCoor=sf::Vector2f(atoi(pElem->Attribute("x")),atoi(pElem->Attribute("y")));
    }

    //! Initiation des images temporaire
    sf::Texture tilesetImg,backImg ,backbackImg ,image_schemaImg;
    //! On supprime les vectors
	m_typeList.erase(m_typeList.begin(),m_typeList.end());
	m_tileSet.erase(m_tileSet.begin(),m_tileSet.end());
    image_schemaImg.LoadFromFile(levelConfig["corrpath"]);
    //! Initiation des images des tiles
    m_ImgTypeTile.LoadFromFile(levelConfig["tilepath"]);
    m_width=tilesetImg.GetWidth();
    m_height=tilesetImg.GetHeight();

    //! On charge les items
    pElem=hDoc.FirstChild("items").FirstChild().Element();
    for(; pElem; pElem=pElem->NextSiblingElement()){
        int itemX =atoi(pElem->Attribute("x1"));
        int itemY =atoi(pElem->Attribute("y1"));
        int itemX2 =atoi(pElem->Attribute("x2"));
        int itemY2 =atoi(pElem->Attribute("y2"));
        int itemX3 =atoi(pElem->Attribute("x3"));
        int itemY3 =atoi(pElem->Attribute("y3"));
    }

    //! On charge les différentes tiles
    pElem=hDoc.FirstChild("tileset").FirstChild().Element();
    for(; pElem; pElem=pElem->NextSiblingElement()){
        int x =atoi(pElem->Attribute("x"));
        int y =atoi(pElem->Attribute("y"));

        Type newTile;
        newTile.colorPix = image_schemaImg.CopyToImage().GetPixel(x, y);
        newTile.zoneRect=sf::IntRect(x*GameConfig::g_config["tilewidth"], y*GameConfig::g_config["tileheight"], GameConfig::g_config["tilewidth"], GameConfig::g_config["tileheight"]);
        newTile.tile.SetTexture(m_ImgTypeTile);

        if(atoi(pElem->Attribute("spawn"))==1)typeSpawn1=m_typeList.size();

        if(atoi(pElem->Attribute("visible"))==1)newTile.visible=true;
        else newTile.visible=false;

        if(atoi(pElem->Attribute("kill"))==1)newTile.kill=true;
        else newTile.kill=false;

        if(atoi(pElem->Attribute("solid"))==1)newTile.solid=true;
        else newTile.solid=false;

        if(atoi(pElem->Attribute("princess"))==1)newTile.princess=true;
        else newTile.princess=false;

        if(atoi(pElem->Attribute("fall"))==1)newTile.fall=true;
        else newTile.fall=false;

        newTile.touch=false;

        m_typeList.insert(m_typeList.end(),newTile);
    }

	//! Charge le niveau
	int theTile;
    for(int it=0;it<m_width;it++){
        vector<Type> tileList2;
        m_tileSet.insert(m_tileSet.end(),tileList2);
        for(int it2=0;it2< m_height;it2++){
            theTile=FindType(tilesetImg.CopyToImage().GetPixel(it, it2));
//            cout<<"Color r="<<(int)tilesetImg.CopyToImage().GetPixel(it, it2).r<<"Color g="<<(int)tilesetImg.CopyToImage().GetPixel(it, it2).g<<"Color b="<<(int)tilesetImg.CopyToImage().GetPixel(it, it2).b<<endl;
            if(theTile==typeSpawn1){
                sf::Vector2f spawnLocationOne(it*GameConfig::g_config["tilewidth"] ,(it2+1)*GameConfig::g_config["tileheight"]-GameConfig::g_config["playercollheight"]);
                m_spawnLocationOne=spawnLocationOne;
                m_playerOne->SetPosition(m_spawnLocationOne);
            }
            Type theNewTile= m_typeList[theTile];
            theNewTile.tile.SetPosition(it*GameConfig::g_config["tilewidth"],it2*GameConfig::g_config["tileheight"]);
            theNewTile.tile.SetSubRect(m_typeList[theTile].zoneRect);
            m_tileSet[it].insert( m_tileSet[it].end(),theNewTile);
        }
    }
    //! Chargement du background
    for(int it=0;it<m_width;it++){
        for(int it2=0;it2<m_height;it2++){
            theTile=FindType(backImg.CopyToImage().GetPixel(it, it2));
            Type theNewTile= m_typeList[theTile];
            theNewTile.tile.SetPosition(it*GameConfig::g_config["tilewidth"],it2*GameConfig::g_config["tileheight"]);
            //theNewTile.tile.SetTexture(m_ImgTypeTile);
            theNewTile.tile.SetSubRect(m_typeList[theTile].zoneRect);
        }
    }
}
MapTile::~MapTile(){
    for(unsigned int i=0;i<m_mapEntity.size();i++){
        delete m_mapEntity.at(i);
    }
}

/**
 * @file Nextion.h
 * @author Dartagnan Leonard (dleonard@industrieshd.com)
 * @brief  Cette Llibrary va permettre de controller un écran Nextion pour le program de calibrationMachine
 * @version 0.1
 * @date 2021-12-13
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef _NEXTION_
#define _NEXTION_

#include <Arduino.h>
///#include "NextionVariable.h"

/***************************************************** NextionVariable ****************************************************/
/**
 * @brief Cree les variable du nextion dans la classe nextion
 * 
 */
class NextionVariable
{
 public:
    uint8_t PageNumber;
    String Page;
    String Nom;
    int16_t Valeur;
 public:
    NextionVariable(uint8_t pageNumber,String page, String nom,int16_t valeur); //Contructeur de variable Nextion 
    
};

    NextionVariable::NextionVariable(uint8_t pageNumber,String page, String nom,int16_t valeur) //Contructeur de variable Nextion 
    {
        this->PageNumber = pageNumber;
        this->Page = page;
        this->Nom = nom;
        this->Valeur = valeur;
    }


/**************************************************** NextionScreen *******************************************************/
class NextionScreen
{
    public:
   NextionScreen(HardwareSerial *ptrSerial, uint8_t nombrePage);


    HardwareSerial *PtrSerial;
    uint8_t NombrePage; 
    uint8_t PageActive;

    uint8_t nombreVariable = 0;
    NextionVariable* tabNextionVariable[50]; // Tableau de 50 pointeur de nextionVaraible 
    void popVariable();
    void pushVariable(NextionVariable* nextionVariable);

    void creeVariable(String page, uint8_t pageNumber, String nom, int value);

    

    void init(HardwareSerial* ptrSerial);
    void update(String page,String name,String type,int value);
    void uptateString(String);
    void lectureDesValeur();
    void pageActive(uint8_t pageActive);
    String receptionMessageSerial(HardwareSerial* ptrSerial);

    private:
    void endMessage();

};

NextionScreen::NextionScreen(HardwareSerial *ptrSerial, uint8_t nombrePage)
{
    this->PtrSerial = ptrSerial;
    this->NombrePage = nombrePage;
}

 
/**
 * @brief 
 * 
 * @param page  : page ou remplacer
 * @param name  : nom de la variable 
 * @param champ : champ a modifier
 * @param value : La nouvelle valeur du champs doit prendre
 */
void NextionScreen::update(String page,String name,String champ,int value)
{
    PtrSerial->print(page + "." + name + "." + champ + "." +value);
    endMessage();
}

void NextionScreen::uptateString(String sendToDisplay)
{
    PtrSerial->print(sendToDisplay);
    endMessage();
}

/**
 * @brief Permet d'ecrire les messages de fin quand on ecrit a l'ecran nextion
 * 
 */
void NextionScreen::endMessage()
{
    PtrSerial->write(0xFF);
    PtrSerial->write(0xFF);
    PtrSerial->write(0xFF);
}



void NextionScreen::creeVariable(String page, uint8_t pageNumber, String nom, int value)
{
    tabNextionVariable[nombreVariable]->Page = page;
    tabNextionVariable[nombreVariable]->PageNumber = pageNumber;
    tabNextionVariable[nombreVariable]->Nom = nom;
    tabNextionVariable[nombreVariable]->Valeur = value;
    nombreVariable++;
}



void NextionScreen::pageActive(uint8_t pageActive)
{
    PageActive = pageActive;
}

String NextionScreen::receptionMessageSerial(HardwareSerial* ptrSerial)
{
  if(ptrSerial->available())
  {
    String data_from_Display="";
    delay(30);
    while(ptrSerial->available())
    {
      data_from_Display+= char(ptrSerial->read());
    }
    return data_from_Display;
  }
}





/********************************Pas obligatoire*******************************/
void NextionScreen::popVariable()
{

}
void NextionScreen::pushVariable(NextionVariable* nextionVariable)
{
    tabNextionVariable[nombreVariable] = nextionVariable;
    nombreVariable++;
}

#endif /*_NEXTION_*/


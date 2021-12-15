/**
 * @file Nextion.cpp
 * @author Dartagnan Leonard (dleonard@industrieshd.com)
 * @brief  Cette Llibrary va permettre de controller un écran Nextion pour le program de calibrationMachine
 * @version 0.1
 * @date 2021-12-13
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "Nextion.h"

#define PAGE_B0 "pageB0"
#define PAGE_B1 "pageB1"
#define PAGE_B2 "pageB2"
#define pB0 1
#define pB1 2
#define pB2 3


/*Creation des variabes Nextion dans le Nextion Screen a ecrire dedans*/

NextionVariable p1_frontPos(pB0,PAGE_B0,"frontPos",0);
NextionVariable p1_rearPos(pB0,PAGE_B0,"rearPos",0);
NextionVariable p1_frontOz(pB0,PAGE_B0,"frontOz",0);
NextionVariable p1_rearOz(pB0,PAGE_B0,"rearOz",0);
NextionVariable p1_nRPM(pB0,PAGE_B0,"nRPM",0);
NextionVariable p1_xDiametre(pB0,PAGE_B0,"xDiametre",40);

NextionVariable p2_frontPos(pB1,PAGE_B1,"frontPos",0);
NextionVariable p2_rearPos(pB1,PAGE_B1,"rearPos",0);
NextionVariable p2_frontOz(pB1,PAGE_B1,"frontOz",0);
NextionVariable p2_rearOz(pB1,PAGE_B1,"rearOz",0);
NextionVariable p2_b1Pos(pB1,PAGE_B1,"b1Pos",0);
NextionVariable p2_b1Oz(pB1,PAGE_B1,"b1Oz",0);
NextionVariable p2_nRPM(pB1,PAGE_B1,"nRPM",0);
NextionVariable p2_xDiametre(pB1,PAGE_B1,"xDiametre",40);


NextionVariable p3_frontPos(pB2,PAGE_B2,"frontPos",0);
NextionVariable p3_rearPos(pB2,PAGE_B2,"rearPos",0);
NextionVariable p3_frontOz(pB2,PAGE_B2,"frontOz",0);
NextionVariable p3_rearOz(pB2,PAGE_B2,"rearOz",0);
NextionVariable p3_b1Pos(pB2,PAGE_B2,"b1Pos",0);
NextionVariable p3_b1Oz(pB2,PAGE_B2,"b1Oz",0);
NextionVariable p3_b2Pos(pB2,PAGE_B2,"b2Pos",0);
NextionVariable p3_b2Oz(pB2,PAGE_B2,"b2Oz",0);
NextionVariable p3_nRPM(pB2,PAGE_B2,"nRPM",0);
NextionVariable p3_xDiametre(pB2,PAGE_B2,"xDiametre",40);


/*Creation des variables a lires dedans */


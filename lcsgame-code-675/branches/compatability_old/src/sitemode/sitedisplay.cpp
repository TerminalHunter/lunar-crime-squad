/*

Copyright (c) 2002,2003,2004 by Tarn Adams                                            //
                                                                                      //
This file is part of Liberal Crime Squad.                                             //
                                                                                    //
    Liberal Crime Squad is free software; you can redistribute it and/or modify     //
    it under the terms of the GNU General Public License as published by            //
    the Free Software Foundation; either version 2 of the License, or               //
    (at your option) any later version.                                             //
                                                                                    //
    Liberal Crime Squad is distributed in the hope that it will be useful,          //
    but WITHOUT ANY WARRANTY; without even the implied warranty of                  //
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the                  //
    GNU General Public License for more details.                                    //
                                                                                    //
    You should have received a copy of the GNU General Public License               //
    along with Liberal Crime Squad; if not, write to the Free Software              //
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA   02111-1307   USA     //
*/

/*
	This file was created by Chris Johnson (grundee@users.sourceforge.net)
	by copying code from game.cpp.
	To see descriptions of files and functions, see the list at
	the bottom of includes.h in the top src folder.
*/

#include <includes.h>
#include <externs.h>



/* prints the 'map graphics' on the bottom right */
void printsitemap(int32 x,int32 y,int32 z)
{
   int32 partysize=0;
   int32 partyalive=0;
   for(int32 p=0;p<6;p++)
   {
      if(activesquad->squad[p]!=NULL)partysize++;
      else continue;

      if(activesquad->squad[p]->alive==1)partyalive++;
   }
   int32 encsize=0;
   int32 freeable=0;
   int32 enemy=0;
   int32 majorenemy=0;
   int32 talkers=0;
   for(int32 e=0;e<ENCMAX;e++)
   {
      if(encounter[e].exists)
      {
         encsize++;
         if(encounter[e].align==-1)enemy++;
         if(encounter[e].type==CREATURE_WORKER_SERVANT||
            encounter[e].type==CREATURE_WORKER_FACTORY_CHILD||
            encounter[e].type==CREATURE_WORKER_SWEATSHOP)freeable++;
         else if(encounter[e].align==0||encounter[e].align==1)talkers++;
         if(encounter[e].type==CREATURE_CORPORATE_CEO||
            encounter[e].type==CREATURE_RADIOPERSONALITY||
            encounter[e].type==CREATURE_NEWSANCHOR||
            encounter[e].type==CREATURE_SCIENTIST_EMINENT||
            encounter[e].type==CREATURE_JUDGE_CONSERVATIVE)majorenemy++;
      }
   }

   set_color(COLOR_WHITE,COLOR_BLACK,0);
   for(int32 sx=57;sx<80;sx++)
   {
      for(int32 sy=9;sy<25;sy++)
      {
         move(sy,sx);
         if(sy==24)addch('-');
         else if(sx==57||sx==79)addch('|');
         else addch(' ');
      }
   }

   int32 px,py;

   //YOUR BLOCK
   px=65;py=14;
   printblock(x,y,z,px,py);

   //UP BLOCK
   px=65;py=9;
   if(y>0)
   {
      if(map[x][y-1][z].flag & SITEBLOCK_BLOCK)
      {
         if(map[x][y][z].flag & SITEBLOCK_BLOODY2)set_color(COLOR_RED,COLOR_BLACK,0);
         else set_color(COLOR_WHITE,COLOR_BLACK,0);
         move(py+4,px);addch(CH_FULL_BLOCK);
         move(py+4,px+1);addch(CH_FULL_BLOCK);
         move(py+4,px+2);addch(CH_FULL_BLOCK);
         move(py+4,px+3);addch(CH_FULL_BLOCK);
         move(py+4,px+4);addch(CH_FULL_BLOCK);
         move(py+4,px+5);addch(CH_FULL_BLOCK);
         move(py+4,px+6);addch(CH_FULL_BLOCK);
      }
      else
      {
         printblock(x,y-1,z,px,py);
      }
   }

   //DOWN BLOCK
   px=65;py=19;
   if(y<MAPY-1)
   {
      if(map[x][y+1][z].flag & SITEBLOCK_BLOCK)
      {
         if(map[x][y][z].flag & SITEBLOCK_BLOODY2)set_color(COLOR_RED,COLOR_BLACK,0);
         else set_color(COLOR_WHITE,COLOR_BLACK,0);
         move(py,px);addch(CH_FULL_BLOCK);
         move(py,px+1);addch(CH_FULL_BLOCK);
         move(py,px+2);addch(CH_FULL_BLOCK);
         move(py,px+3);addch(CH_FULL_BLOCK);
         move(py,px+4);addch(CH_FULL_BLOCK);
         move(py,px+5);addch(CH_FULL_BLOCK);
         move(py,px+6);addch(CH_FULL_BLOCK);
      }
      else
      {
         printblock(x,y+1,z,px,py);
      }
   }

   //WEST BLOCK
   px=58;py=14;
   if(x>0)
   {
      if(map[x-1][y][z].flag & SITEBLOCK_BLOCK)
      {
         if(map[x][y][z].flag & SITEBLOCK_BLOODY2)set_color(COLOR_RED,COLOR_BLACK,0);
         else set_color(COLOR_WHITE,COLOR_BLACK,0);
         move(py,px+6);addch(CH_FULL_BLOCK);
         move(py+1,px+6);addch(CH_FULL_BLOCK);
         move(py+2,px+6);addch(CH_FULL_BLOCK);
         move(py+3,px+6);addch(CH_FULL_BLOCK);
         move(py+4,px+6);addch(CH_FULL_BLOCK);
      }
      else
      {
         printblock(x-1,y,z,px,py);
      }
   }

   //EAST BLOCK
   px=72;py=14;
   if(x<MAPX-1)
   {
      if(map[x+1][y][z].flag & SITEBLOCK_BLOCK)
      {
         if(map[x][y][z].flag & SITEBLOCK_BLOODY2)set_color(COLOR_RED,COLOR_BLACK,0);
         else set_color(COLOR_WHITE,COLOR_BLACK,0);
         move(py,px);addch(CH_FULL_BLOCK);
         move(py+1,px);addch(CH_FULL_BLOCK);
         move(py+2,px);addch(CH_FULL_BLOCK);
         move(py+3,px);addch(CH_FULL_BLOCK);
         move(py+4,px);addch(CH_FULL_BLOCK);
      }
      else
      {
         printblock(x+1,y,z,px,py);
      }
   }

   //NE BLOCK
   px=72;py=9;
   if(x<MAPX-1&&y>0)
   {
      if(map[x+1][y-1][z].flag & SITEBLOCK_BLOCK)
      {
         if(!(map[x][y-1][z].flag & SITEBLOCK_BLOCK))
         {
            if(map[x][y-1][z].flag & SITEBLOCK_BLOODY2)set_color(COLOR_RED,COLOR_BLACK,0);
            else set_color(COLOR_WHITE,COLOR_BLACK,0);
            move(py,px);addch(CH_FULL_BLOCK);
            move(py+1,px);addch(CH_FULL_BLOCK);
            move(py+2,px);addch(CH_FULL_BLOCK);
            move(py+3,px);addch(CH_FULL_BLOCK);
            move(py+4,px);addch(CH_FULL_BLOCK);
         }
         if(!(map[x+1][y][z].flag & SITEBLOCK_BLOCK))
         {
            if(map[x+1][y][z].flag & SITEBLOCK_BLOODY2)set_color(COLOR_RED,COLOR_BLACK,0);
            else set_color(COLOR_WHITE,COLOR_BLACK,0);
            move(py+4,px);addch(CH_FULL_BLOCK);
            move(py+4,px+1);addch(CH_FULL_BLOCK);
            move(py+4,px+2);addch(CH_FULL_BLOCK);
            move(py+4,px+3);addch(CH_FULL_BLOCK);
            move(py+4,px+4);addch(CH_FULL_BLOCK);
            move(py+4,px+5);addch(CH_FULL_BLOCK);
            move(py+4,px+6);addch(CH_FULL_BLOCK);
         }
      }
      else
      {
         printblock(x+1,y-1,z,px,py);
      }
   }

   //SE BLOCK
   px=72;py=19;
   if(x<MAPX-1&&y<MAPY-1)
   {
      if(map[x+1][y+1][z].flag & SITEBLOCK_BLOCK)
      {
         if(!(map[x][y+1][z].flag & SITEBLOCK_BLOCK))
         {
            if(map[x][y+1][z].flag & SITEBLOCK_BLOODY2)set_color(COLOR_RED,COLOR_BLACK,0);
            else set_color(COLOR_WHITE,COLOR_BLACK,0);
            move(py,px);addch(CH_FULL_BLOCK);
            move(py+1,px);addch(CH_FULL_BLOCK);
            move(py+2,px);addch(CH_FULL_BLOCK);
            move(py+3,px);addch(CH_FULL_BLOCK);
            move(py+4,px);addch(CH_FULL_BLOCK);
         }
         if(!(map[x+1][y][z].flag & SITEBLOCK_BLOCK))
         {
            if(map[x+1][y][z].flag & SITEBLOCK_BLOODY2)set_color(COLOR_RED,COLOR_BLACK,0);
            else set_color(COLOR_WHITE,COLOR_BLACK,0);
            move(py,px);addch(CH_FULL_BLOCK);
            move(py,px+1);addch(CH_FULL_BLOCK);
            move(py,px+2);addch(CH_FULL_BLOCK);
            move(py,px+3);addch(CH_FULL_BLOCK);
            move(py,px+4);addch(CH_FULL_BLOCK);
            move(py,px+5);addch(CH_FULL_BLOCK);
            move(py,px+6);addch(CH_FULL_BLOCK);
         }
      }
      else
      {
         printblock(x+1,y+1,z,px,py);
      }
   }

   //NW BLOCK
   px=58;py=9;
   if(x>0&&y>0)
   {
      if(map[x-1][y-1][z].flag & SITEBLOCK_BLOCK)
      {
         if(!(map[x][y-1][z].flag & SITEBLOCK_BLOCK))
         {
            if(map[x][y-1][z].flag & SITEBLOCK_BLOODY2)set_color(COLOR_RED,COLOR_BLACK,0);
            else set_color(COLOR_WHITE,COLOR_BLACK,0);
            move(py,px+6);addch(CH_FULL_BLOCK);
            move(py+1,px+6);addch(CH_FULL_BLOCK);
            move(py+2,px+6);addch(CH_FULL_BLOCK);
            move(py+3,px+6);addch(CH_FULL_BLOCK);
            move(py+4,px+6);addch(CH_FULL_BLOCK);
         }
         if(!(map[x-1][y][z].flag & SITEBLOCK_BLOCK))
         {
            if(map[x-1][y][z].flag & SITEBLOCK_BLOODY2)set_color(COLOR_RED,COLOR_BLACK,0);
            else set_color(COLOR_WHITE,COLOR_BLACK,0);
            move(py+4,px);addch(CH_FULL_BLOCK);
            move(py+4,px+1);addch(CH_FULL_BLOCK);
            move(py+4,px+2);addch(CH_FULL_BLOCK);
            move(py+4,px+3);addch(CH_FULL_BLOCK);
            move(py+4,px+4);addch(CH_FULL_BLOCK);
            move(py+4,px+5);addch(CH_FULL_BLOCK);
            move(py+4,px+6);addch(CH_FULL_BLOCK);
         }
      }
      else
      {
         printblock(x-1,y-1,z,px,py);
      }
   }

   //SW BLOCK
   px=58;py=19;
   if(x>0&&y<MAPY-1)
   {
      if(map[x-1][y+1][z].flag & SITEBLOCK_BLOCK)
      {
         if(!(map[x][y+1][z].flag & SITEBLOCK_BLOCK))
         {
            if(map[x][y+1][z].flag & SITEBLOCK_BLOODY2)set_color(COLOR_RED,COLOR_BLACK,0);
            else set_color(COLOR_WHITE,COLOR_BLACK,0);
            move(py,px+6);addch(CH_FULL_BLOCK);
            move(py+1,px+6);addch(CH_FULL_BLOCK);
            move(py+2,px+6);addch(CH_FULL_BLOCK);
            move(py+3,px+6);addch(CH_FULL_BLOCK);
            move(py+4,px+6);addch(CH_FULL_BLOCK);
         }
         if(!(map[x-1][y][z].flag & SITEBLOCK_BLOCK))
         {
            if(map[x-1][y][z].flag & SITEBLOCK_BLOODY2)set_color(COLOR_RED,COLOR_BLACK,0);
            else set_color(COLOR_WHITE,COLOR_BLACK,0);
            move(py,px);addch(CH_FULL_BLOCK);
            move(py,px+1);addch(CH_FULL_BLOCK);
            move(py,px+2);addch(CH_FULL_BLOCK);
            move(py,px+3);addch(CH_FULL_BLOCK);
            move(py,px+4);addch(CH_FULL_BLOCK);
            move(py,px+5);addch(CH_FULL_BLOCK);
            move(py,px+6);addch(CH_FULL_BLOCK);
         }
      }
      else
      {
         printblock(x-1,y+1,z,px,py);
      }
   }

   //PRINT SPECIAL
   char str[200];
   switch(map[locx][locy][locz].special)
   {
      case SPECIAL_LAB_COSMETICS_CAGEDANIMALS:strcpy(str,"Caged Animals");break;
      case SPECIAL_NUCLEAR_ONOFF:strcpy(str,"The On/Off Switch");break;
      case SPECIAL_LAB_GENETIC_CAGEDANIMALS:strcpy(str,"Caged \"Animals\"");break;
      case SPECIAL_POLICESTATION_LOCKUP:strcpy(str,"Police Detention Room");break;
      case SPECIAL_COURTHOUSE_LOCKUP:strcpy(str,"Court House Jail");break;
      case SPECIAL_COURTHOUSE_JURYROOM:strcpy(str,"Jury Room");break;
      case SPECIAL_PRISON_CONTROL:strcpy(str,"Prison Control Room");break;
      case SPECIAL_INTEL_SUPERCOMPUTER:strcpy(str,"Super-computer");break;
      case SPECIAL_SWEATSHOP_EQUIPMENT:strcpy(str,"Textile Equipment");break;
      case SPECIAL_POLLUTER_EQUIPMENT:strcpy(str,"Factory Equipment");break;
      case SPECIAL_HOUSE_PHOTOS:strcpy(str,"Safe");break;
      case SPECIAL_CORPORATE_FILES:strcpy(str,"Safe");break;
      case SPECIAL_RADIO_BROADCASTSTUDIO:strcpy(str,"Radio Broadcast Room");break;
      case SPECIAL_NEWS_BROADCASTSTUDIO:strcpy(str,"News Broadcast Studio");break;
      case SPECIAL_APARTMENT_LANDLORD:strcpy(str,"Landlord's Office");break;
      case SPECIAL_APARTMENT_SIGN:strcpy(str,"Sign");break;
      case SPECIAL_STAIRS_UP:strcpy(str,"Stairs Up");break;
      case SPECIAL_STAIRS_DOWN:strcpy(str,"Stairs Down");break;
      case SPECIAL_RESTAURANT_TABLE:strcpy(str,"Table");break;
      case SPECIAL_CAFE_COMPUTER:strcpy(str,"Computer");break;
   }
   if(map[locx][locy][locz].special!=-1)
   {
      set_color(COLOR_WHITE,COLOR_BLACK,1);
      move(24,68-(strlen(str)>>1));
      addstr(str);
   }

   //PRINT PARTY
   if(partyalive>0)set_color(COLOR_GREEN,COLOR_BLACK,1);
   else set_color(COLOR_BLACK,COLOR_BLACK,1);
   move(16,66);
   addstr("SQUAD");

   //PRINT ANY OPPOSING FORCE INFO
   if(encsize>0)
   {
      set_color(COLOR_YELLOW,COLOR_BLACK,1);
      move(17,65);
      if(map[locx][locx][locz].siegeflag & SIEGEFLAG_HEAVYUNIT)
      {
         addstr("ENC+ARM");
      }
      else if(map[locx][locx][locz].siegeflag & SIEGEFLAG_UNIT)
      {
         addstr("ENC+ENM");
      }
      else if(map[locx][locx][locz].siegeflag & SIEGEFLAG_UNIT_DAMAGED)
      {
         addstr("ENC+enm");
      }
      else addstr("ENCNTER");

      printencounter();
   }

   if(groundloot.size()>0)
   {
      set_color(COLOR_MAGENTA,COLOR_BLACK,1);
      move(18,66);
      addstr("LOOT!");

      printencounter();
   }
}



void printblock(int32 x,int32 y,int32 z,int32 px,int32 py)
{
   if(map[x][y][z].flag & SITEBLOCK_RESTRICTED)
   {
      set_color(COLOR_BLACK,COLOR_BLACK,1);
      for(int32 px2=px;px2<px+7;px2++)
      {
         for(int32 py2=py;py2<py+5;py2++)
         {
            move(py2,px2);
            addstr("x");
         }
      }
   }

   if(map[x][y][z].flag & SITEBLOCK_BLOODY2)
   {
      set_color(COLOR_RED,COLOR_BLACK,0);
      move(py+3,px+2);
      addch('\\');
      move(py+1,px+1);
      addch('%');
      move(py+4,px+5);
      addch('&');
      move(py+2,px+1);
      addch('.');
      move(py+1,px+2);
      addch('.');
      move(py+4,px+4);
      addch('.');
   }
   else if(map[x][y][z].flag & SITEBLOCK_BLOODY)
   {
      set_color(COLOR_RED,COLOR_BLACK,0);
      move(py+2,px+1);
      addch('.');
      move(py+1,px+2);
      addch('.');
      move(py+4,px+4);
      addch('.');
   }

   if(map[x][y][z].flag & SITEBLOCK_EXIT)
   {
      set_color(COLOR_WHITE,COLOR_BLACK,0);
      move(py+1,px+1);
      addstr("EXIT");
   }
   else if(map[x][y][z].flag & SITEBLOCK_DOOR)
   {
      if((map[x][y][z].flag & SITEBLOCK_CLOCK) &&
         (map[x][y][z].flag & SITEBLOCK_LOCKED))
      {
         set_color(COLOR_RED,COLOR_BLACK,0);
         move(py+1,px);
         addstr("L. DOOR");
      }
      else if((map[x][y][z].flag & SITEBLOCK_KLOCK) &&
         (map[x][y][z].flag & SITEBLOCK_LOCKED))
      {
         set_color(COLOR_YELLOW,COLOR_BLACK,0);
         move(py+1,px);
         addstr("L. DOOR");
      }
      else
      {
         set_color(COLOR_YELLOW,COLOR_BLACK,0);
         move(py+1,px+1);
         addstr("DOOR");
      }
   }
   else if(map[x][y][z].flag & SITEBLOCK_LOOT)
   {
      set_color(COLOR_MAGENTA,COLOR_BLACK,1);
      move(py+1,px+1);
      addstr("GOODS");
   }

   if(map[x][y][z].siegeflag & SIEGEFLAG_TRAP)
   {
      set_color(COLOR_YELLOW,COLOR_BLACK,1);
      move(py,px+1);
      addstr("TRAP!");
   }
   else if(map[x][y][z].siegeflag & SIEGEFLAG_UNIT_DAMAGED)
   {
      set_color(COLOR_RED,COLOR_BLACK,0);
      move(py,px+1);
      addstr("enemy");
   }
   else if(map[x][y][z].special!=-1)
   {
      set_color(COLOR_YELLOW,COLOR_BLACK,1);
      move(py,px);

      switch(map[x][y][z].special)
      {
         case SPECIAL_LAB_COSMETICS_CAGEDANIMALS:addstr("ANIMALS");break;
         case SPECIAL_NUCLEAR_ONOFF:addstr("ON/OFF!");break;
         case SPECIAL_LAB_GENETIC_CAGEDANIMALS:addstr("ANIMALS");break;
         case SPECIAL_POLICESTATION_LOCKUP:addstr("LOCK-UP");break;
         case SPECIAL_COURTHOUSE_LOCKUP:addstr("LOCK-UP");break;
         case SPECIAL_COURTHOUSE_JURYROOM:move(py,px+1);addstr("JURY!");break;
         case SPECIAL_PRISON_CONTROL:addstr("CONTROL");break;
         case SPECIAL_INTEL_SUPERCOMPUTER:addstr("COMPUTR");break;
         case SPECIAL_SWEATSHOP_EQUIPMENT:addstr("MACHINE");break;
         case SPECIAL_POLLUTER_EQUIPMENT:addstr("MACHINE");break;
         case SPECIAL_HOUSE_PHOTOS:move(py,px+1);addstr("SAFE!");break;
         case SPECIAL_CORPORATE_FILES:move(py,px+1);addstr("SAFE!");break;
         case SPECIAL_RADIO_BROADCASTSTUDIO:addstr("STUDIO!");break;
         case SPECIAL_NEWS_BROADCASTSTUDIO:addstr("STUDIO!");break;
         case SPECIAL_APARTMENT_LANDLORD:addstr("LANDLRD");break;
         case SPECIAL_APARTMENT_SIGN:move(py,px+1);addstr("SIGN!");break;
         case SPECIAL_STAIRS_UP:addstr("STAIRS");addch(24);break;
         case SPECIAL_STAIRS_DOWN:addstr("STAIRS");addch(25);break;
         case SPECIAL_RESTAURANT_TABLE:move(py,px+1);addstr("TABLE");break;
         case SPECIAL_CAFE_COMPUTER:addstr("COMPUTR");break;
      }
   }
   if(map[x][y][z].siegeflag & SIEGEFLAG_HEAVYUNIT)
   {
      set_color(COLOR_RED,COLOR_BLACK,1);
      move(py+3,px+1);
      addstr("ARMOR");
   }
   else if(map[x][y][z].siegeflag & SIEGEFLAG_UNIT)
   {
      set_color(COLOR_RED,COLOR_BLACK,1);
      move(py+3,px+1);
      addstr("ENEMY");
   }
}



/* prints the names of creatures you see */
void printencounter(void)
{
   for(int32 i=19;i<=24;i++)
   {
      move(i,1);
      addstr("                                                        ");
   }

   int32 px=1,py=19;
   for(int32 e=0;e<ENCMAX;e++)
   {
      if(encounter[e].exists)
      {
         if(!encounter[e].alive)set_color(COLOR_BLACK,COLOR_BLACK,1);
         else if(encounter[e].align==0)set_color(COLOR_WHITE,COLOR_BLACK,1);
         else if(encounter[e].align==1)set_color(COLOR_GREEN,COLOR_BLACK,1);
         else set_color(COLOR_RED,COLOR_BLACK,1);
         move(py,px);
         addstr(encounter[e].name);
      }

      px+=18;
      if(px>37)
      {
         px=1;
         py++;
      }
   }
}



/* prints the names of creatures you see in car chases */
void printchaseencounter(void)
{
   if(chaseseq.enemycar.size()>0)
   {
      for(int32 i=19;i<=24;i++)
      {
         move(i,1);
         addstr("                                                     ");
      }

      int32 carsy[4]={20,20,20,20};

      char str[80];
      for(int32 v=0;v<chaseseq.enemycar.size();v++)
      {
         set_color(COLOR_WHITE,COLOR_BLACK,1);
         move(19,v*20+1);
         getcarfull(str,*chaseseq.enemycar[v],1);
         addstr(str);
      }

      for(int32 e=0;e<ENCMAX;e++)
      {
         if(encounter[e].exists)
         {
            for(int32 v=0;v<chaseseq.enemycar.size();v++)
            {
               if(chaseseq.enemycar[v]->id==encounter[e].carid)
               {
                  set_color(COLOR_RED,COLOR_BLACK,1);
                  move(carsy[v],v*20+1);
                  addstr(encounter[e].name);
                  if(encounter[e].is_driver)addstr("-D");
                  carsy[v]++;
               }
            }
         }
      }
   }
   else printencounter();
}



/* blanks a part of the screen */
void clearcommandarea(void)
{
   set_color(COLOR_WHITE,COLOR_BLACK,1);
   for(int32 y=9;y<16;y++)
   {
      move(y,1);
      addstr("                                                        ");
   }
}



void clearmessagearea(void)
{
   set_color(COLOR_WHITE,COLOR_BLACK,1);
   move(16,1);
   addstr("                                                        ");
   move(17,1);
   addstr("                                                        ");
}



void clearmaparea(void)
{
   set_color(COLOR_WHITE,COLOR_BLACK,1);
   for(int32 y=9;y<24;y++)
   {
      move(y,57);
      addstr("                       ");
   }
}
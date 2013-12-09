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



/* handles end of round stuff for everyone */
void creatureadvance(void)
{
   int32 e;
   for(int32 p=0;p<6;p++)
   {
      if(activesquad->squad[p]==NULL)continue;
      if(!activesquad->squad[p]->alive)continue;

      advancecreature(*activesquad->squad[p]);
      if(activesquad->squad[p]->prisoner!=NULL)
      {
         advancecreature(*activesquad->squad[p]->prisoner);
         if(!activesquad->squad[p]->prisoner->alive)
         {
            if(activesquad->squad[p]->prisoner->squadid==-1)
            {
               clearmessagearea();
               set_color(COLOR_WHITE,COLOR_BLACK,1);
               move(16,1);
               addstr(activesquad->squad[p]->name);
               addstr(" drops ");
               addstr(activesquad->squad[p]->prisoner->name);
               addstr("'s body.");

               makeloot(*activesquad->squad[p]->prisoner,groundloot);

               refresh();
               getch();

               sitecrime+=10;
               sitestory->crime.push_back(CRIME_KILLEDSOMEBODY);
               criminalizeparty(LAWFLAG_MURDER);

               if(activesquad->squad[p]->prisoner->type==CREATURE_CORPORATE_CEO||
                  activesquad->squad[p]->prisoner->type==CREATURE_RADIOPERSONALITY||
                  activesquad->squad[p]->prisoner->type==CREATURE_NEWSANCHOR||
                  activesquad->squad[p]->prisoner->type==CREATURE_SCIENTIST_EMINENT||
                  activesquad->squad[p]->prisoner->type==CREATURE_JUDGE_CONSERVATIVE)sitecrime+=30;

               delete activesquad->squad[p]->prisoner;
               activesquad->squad[p]->prisoner=NULL;
            }
         }
      }
   }

   if(location[cursite]->siege.siege)
   {
      for(int32 p=0;p<pool.size();p++)
      {
         if(!pool[p]->alive)continue;
         if(pool[p]->squadid!=-1)continue;
         if(pool[p]->location!=cursite)continue;

         advancecreature(*pool[p]);
      }

      autopromote(cursite);
   }

   for(e=0;e<ENCMAX;e++)
   {
      if(!encounter[e].exists)continue;
      if(!encounter[e].alive)continue;

      advancecreature(encounter[e]);
   }

   if(mode!=GAMEMODE_CHASECAR)
   {
      //TAKE THE INJURED WITH YOU
      squadgrab_immobile(0);

      //TAKE THE DEAD WITH YOU
      squadgrab_immobile(1);
   }

   for(e=ENCMAX-1;e>=0;e--)
   {
      if(!encounter[e].exists)continue;
      if(!encounter[e].alive)delenc(e,1);
   }

   if(mode==GAMEMODE_SITE)
   {
      if(sitealarm)postalarmtimer++;
      if(sitealarmtimer>0&&!sitealarm)
      {
         sitealarmtimer--;
         if(sitealarmtimer<=0)
         {
            sitealarmtimer=0;

            clearmessagearea();

            set_color(COLOR_YELLOW,COLOR_BLACK,1);
            move(16,1);
            addstr("The Squad smells Conservative panic.");

            if(mode==GAMEMODE_CHASECAR||
                  mode==GAMEMODE_CHASEFOOT)printchaseencounter();
            else printencounter();

            refresh();
            getch();
         }
      }
   }
}



/* handles end of round stuff for one creature */
void advancecreature(creaturest &cr)
{
   if(!cr.alive)return;

   char incaprint;
   if(incapacitated(cr,1,incaprint))
   {
      if(incaprint)
      {
         printparty();
         if(mode==GAMEMODE_CHASECAR||
                  mode==GAMEMODE_CHASEFOOT)printchaseencounter();
         else printencounter();

         refresh();
         getch();
      }
   }

   int32 bleed=0;

   for(int32 w=0;w<BODYPARTNUM;w++)
   {
      if(cr.wound[w] & WOUND_BLEEDING)bleed++;
   }

   if(bleed>0)
   {
      clearmessagearea();

      cr.blood-=bleed;

      map[locx][locy][locz].flag|=SITEBLOCK_BLOODY;

      if(cr.armor.type!=ARMOR_NONE)cr.armor.flag|=ARMORFLAG_BLOODY;

      char str[200];

      if(cr.blood<=0)
      {
         cr.blood=0;
         cr.alive=0;

         if(cr.squadid!=-1)
         {
            if(cr.align==1)stat_dead++;
         }
         else if(cr.align==-1&&cr.animalgloss!=ANIMALGLOSS_ANIMAL)
         {
            stat_kills++;
            if(location[cursite]->siege.siege)location[cursite]->siege.kills++;
         }
         if(cr.squadid==-1)
         {
            sitecrime+=10;
            sitestory->crime.push_back(CRIME_KILLEDSOMEBODY);
            criminalizeparty(LAWFLAG_MURDER);
         }
         adddeathmessage(cr);

         refresh();
         getch();

         if(cr.prisoner!=NULL)
         {
            freehostage(cr,1);
         }
      }
      else
      {
         set_color(COLOR_RED,COLOR_BLACK,0);
         move(16,1);
         strcpy(str,cr.name);
         strcat(str," bleeds.");
         addstr(str);

         refresh();
         getch();
      }
   }
}
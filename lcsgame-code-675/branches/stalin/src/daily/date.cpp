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

//#include <includes.h>
#include <externs.h>

#define DATERESULT_MEETTOMORROW 0
#define DATERESULT_BREAKUP      1
#define DATERESULT_JOINED       2
#define DATERESULT_ARRESTED     3


// Handles the result of a date or vacation
static int dateresult(int aroll,int troll,datest &d,int e,int p,int y)
{
   if(aroll>troll)
   {
      set_color(COLOR_BLUE,COLOR_BLACK,1);
      move(y,0);y++;
      addstr(d.date[e]->name);
      addstr(" is quite taken with ");
      addstr(pool[p]->name);
      addstr("'s unique life philosophy...");
      refresh();
      getch();

      if(signed(LCSrandom(d.date[e]->att[ATTRIBUTE_HEART]+(aroll-troll)/2))>d.date[e]->att[ATTRIBUTE_WISDOM])
      {
         if(loveslavesleft(*pool[p]) == 0)
         {
            set_color(COLOR_RED,COLOR_BLACK,1);
            
            move(y++,0);
            addstr("But when ");
            addstr(pool[p]->name);
            addstr(" mentions having other lovers, things go downhill fast.");

            refresh();
            getch();

            move(y++,0);
            addstr("This relationship is over.");

            refresh();
            getch();

            delete d.date[e];
            d.date.erase(d.date.begin() + e);

            return DATERESULT_BREAKUP;
         }
         set_color(COLOR_GREEN,COLOR_BLACK,1);
         move(y,0);y++;
         addstr("In fact, ");
         addstr(d.date[e]->name);
         addstr(" is ");
         addstr(pool[p]->name);
         addstr("'s totally unconditional love-slave!");
         //Get map of their workplace
         location[d.date[e]->worklocation]->interrogated=1;
         location[d.date[e]->worklocation]->hidden=0;
         refresh();
         getch();

         generate_name(d.date[e]->name);
         strcpy(d.date[e]->propername,d.date[e]->name);

         d.date[e]->flag|=CREATUREFLAG_LOVESLAVE;
         d.date[e]->hireid=pool[p]->id;

         erase();

         set_color(COLOR_WHITE,COLOR_BLACK,1);
         move(0,0);
         addstr("The Self-Nullifying Infatuation of ");
         addstr(d.date[e]->propername);

         move(2,0);
         set_color(COLOR_WHITE,COLOR_BLACK,0);
         addstr("What name will you use for this ");
         char str[80];
         getrecruitcreature(str,d.date[e]->type);
         addstr(str);
         addstr(" in its presence?");
         move(3,0);
         addstr("If you do not enter anything, their real name will be used.");
         
         move(4,0);
         enter_name(d.date[e]->name,CREATURE_NAMELEN,d.date[e]->propername);

         move(6,0);
         set_color(COLOR_WHITE,COLOR_BLACK,0);
         addstr("In what capacity will ");
         addstr(d.date[e]->name);
         addstr(" best serve the Liberal cause?");
         move(7,0);
         addstr("1) Stay at ");
         addstr(location[d.date[e]->worklocation]->name);
         addstr(" as a ");
         set_color(COLOR_CYAN,COLOR_BLACK,0);
         addstr("sleeper agent");
         set_color(COLOR_WHITE,COLOR_BLACK,0);
         addstr(".");
         move(8,0);
         addstr("2) Come to ");
         addstr(location[pool[p]->location]->name);
         addstr(" as a ");
         set_color(COLOR_GREEN,COLOR_BLACK,0);
         addstr("regular member");
         set_color(COLOR_WHITE,COLOR_BLACK,0);
         addstr(".");

         
         while(1)
         {
            char keystroke = getch();
            if(keystroke == '1')
            {
               d.date[e]->flag |= CREATUREFLAG_SLEEPER;
               liberalize(*d.date[e],false);
               d.date[e]->location = d.date[e]->worklocation;
               d.date[e]->base = d.date[e]->worklocation;
               location[d.date[e]->worklocation]->interrogated=1;
               location[d.date[e]->worklocation]->hidden=0;
               d.date[e]->infiltration/=2;
               break;
            }
            else if(keystroke == '2')
            {
               d.date[e]->location=pool[p]->location;
               d.date[e]->base=pool[p]->base;
               liberalize(*d.date[e],false);
               d.date[e]->infiltration/=2;
               break;
            }
         }
         
         pool.push_back(d.date[e]);
         stat_recruits++;
         d.date.erase(d.date.begin() + e);

         return DATERESULT_JOINED;
      }
      else
      {
         if(d.date[e]->att[ATTRIBUTE_HEART]<pool[p]->att[ATTRIBUTE_HEART]-4)
         {
            d.date[e]->att[ATTRIBUTE_HEART]++;
         }
         else
         {
            //Posibly date reveals map of location
            if(location[d.date[e]->worklocation]->interrogated==0 && !LCSrandom(d.date[e]->att[ATTRIBUTE_WISDOM]))
            {
               y++;
               move(y++,0);
               addstr(d.date[e]->name);
               addstr(" turns the topic of discussion to the ");
               addstr(location[d.date[e]->worklocation]->name);
               addstr(".");
               move(y++,0);
               if(!(location[d.date[e]->worklocation]->type<=SITE_RESIDENTIAL_SHELTER))
               {
                  addstr(pool[p]->name);
                  addstr(" was able to create a map of the site with this information.");
                  y++;                           
               }
               else
               {
                  addstr(pool[p]->name);
                  addstr(" knows all about that already.");
                  y++;
               }
               location[d.date[e]->worklocation]->interrogated=1;
               location[d.date[e]->worklocation]->hidden=0;
            }
         }

         set_color(COLOR_WHITE,COLOR_BLACK,0);
         move(y,0);y++;
         addstr("They'll meet again tomorrow.");
         refresh();
         getch();
         
         return DATERESULT_MEETTOMORROW;
      }
   }
   else
   {
      //WISDOM POSSIBLE INCREASE
      if(d.date[e]->align==-1&&aroll<troll/2)
      {
			set_color(COLOR_RED,COLOR_BLACK,1);
         move(y,0);y++;
         
         addstr("Talking with ");
         addstr(d.date[e]->name);
         addstr(" actually increases ");
         addstr(pool[p]->name);
         addstr("'s wisdom!!!");
         pool[p]->att[ATTRIBUTE_WISDOM]++;

         if(d.date[e]->skillval(SKILL_RELIGION)>pool[p]->skillval(SKILL_RELIGION))
         {
            pool[p]->train(SKILL_RELIGION,10*(d.date[e]->skillval(SKILL_RELIGION)-pool[p]->skillval(SKILL_RELIGION)));
         }
         if(d.date[e]->skillval(SKILL_SCIENCE)>pool[p]->skillval(SKILL_SCIENCE))
         {
            pool[p]->train(SKILL_SCIENCE,10*(d.date[e]->skillval(SKILL_SCIENCE)-pool[p]->skillval(SKILL_SCIENCE)));
         }
         if(d.date[e]->skillval(SKILL_BUSINESS)>pool[p]->skillval(SKILL_BUSINESS))
         {
            pool[p]->train(SKILL_BUSINESS,10*(d.date[e]->skillval(SKILL_BUSINESS)-pool[p]->skillval(SKILL_BUSINESS)));
         }

         refresh();
         getch();
      }

      //BREAK UP
      
      // *JDS* If your squad member is wanted by the police, a conservative who breaks up with
      // them has a 1 in 50 chance of ratting them out, unless the person being dated is law
      // enforcement, prison guard, or agent, in which case there is a 1 in 4 chance.
      if((iscriminal(*pool[p])) &&
         (!LCSrandom(50) ||(LCSrandom(2) && (d.date[e]->type==CREATURE_AGENT||
         d.date[e]->type==CREATURE_COP||d.date[e]->type==CREATURE_GANGUNIT||
         d.date[e]->type==CREATURE_SWAT||
         d.date[e]->type==CREATURE_DEATHSQUAD||d.date[e]->type==CREATURE_PRISONGUARD||
         d.date[e]->type==CREATURE_EDUCATOR))))
      {
         move(y,0);y++;
         set_color(COLOR_RED,COLOR_BLACK,1);
         addstr(d.date[e]->name);
         addstr(" was leaking information to the police the whole time!");
         refresh();
         getch();
         move(y,0);y++;
         // 3/4 chance of being arrested if less than 50 juice,
         // 1/2 chance of being arrested if more than 50 juice
         if((pool[p]->juice<50 && LCSrandom(2)) || LCSrandom(2))
         {
            // Find the police station
            long ps=-1;
            for(int l=0;l<location.size();l++)
            {
               if(location[l]->type==SITE_GOVERNMENT_POLICESTATION)
               {
                  ps=l;
               }
            }

            set_color(COLOR_MAGENTA,COLOR_BLACK,1);
            addstr(pool[p]->name);
            addstr(" has been arrested.");
            
            removesquadinfo(*pool[p]);
            pool[p]->carid=-1;
            pool[p]->location=ps;
            pool[p]->weapon.type=WEAPON_NONE;
            pool[p]->weapon.ammo=0;
            pool[p]->activity.type=ACTIVITY_NONE;

            refresh();
            getch();

            delete d.date[e];
            d.date.erase(d.date.begin() + e);

            return DATERESULT_ARRESTED;
         }
         else
         {
            set_color(COLOR_GREEN,COLOR_BLACK,1);
            addstr("But ");
            addstr(pool[p]->name);
            addstr(" escapes the police ambush!");
         }
      }
      else
      {
         set_color(COLOR_MAGENTA,COLOR_BLACK,1);
         move(y,0);y++;
         addstr(d.date[e]->name);
         addstr(" can sense that things just aren't working out.");
         move(y,0);y++;
         
         addstr("This relationship is over.");
      }
      refresh();
      getch();

      delete d.date[e];
      d.date.erase(d.date.begin() + e);

      return DATERESULT_BREAKUP;
   }
}

/* daily - date - dater p gets back from vacation */
char completevacation(datest &d,int p,char &clearformess)
{
   int e=0;

   clearformess=1;

   erase();
   set_color(COLOR_WHITE,COLOR_BLACK,1);
   move(0,0);
   addstr(pool[p]->name);
   addstr(" is back from vacation.");

   short aroll=LCSrandom(51+pool[p]->attval(ATTRIBUTE_CHARISMA)*4+LCSrandom(pool[p]->skillval(SKILL_SEDUCTION)*4))+10;
   short troll=LCSrandom(21+d.date[e]->attval(ATTRIBUTE_CHARISMA)*2+d.date[e]->attval(ATTRIBUTE_WISDOM)*4);
   pool[p]->train(SKILL_PERSUASION,LCSrandom(14)+7);

   if(d.date[e]->skillval(SKILL_BUSINESS))
   {
      troll+=d.date[e]->skillval(SKILL_BUSINESS);
      if(pool[p]->skillval(SKILL_BUSINESS))
         aroll+=pool[p]->skillval(SKILL_BUSINESS);
   }
   if(d.date[e]->skillval(SKILL_RELIGION))
   {
      troll+=d.date[e]->skillval(SKILL_RELIGION);
      if(pool[p]->skillval(SKILL_RELIGION))
         aroll+=pool[p]->skillval(SKILL_RELIGION);
   }
   if(d.date[e]->skillval(SKILL_SCIENCE))
   {
      troll+=d.date[e]->skillval(SKILL_SCIENCE);
      if(pool[p]->skillval(SKILL_SCIENCE))
         aroll+=pool[p]->skillval(SKILL_SCIENCE);
   }


   int y=2;
   switch(dateresult(aroll,troll,d,e,p,y))
   {
   default:
   case DATERESULT_MEETTOMORROW:return 0;
   case DATERESULT_BREAKUP:     return 1;
   case DATERESULT_JOINED:      return 1;
   case DATERESULT_ARRESTED:    return 1;
   }
}



/* daily - date - dater p goes on some dates */
char completedate(datest &d,int p,char &clearformess)
{
   int e;
   clearformess=1;

   erase();
   set_color(COLOR_WHITE,COLOR_BLACK,1);
   move(0,0);
   addstr(pool[p]->name);
   addstr(" has ");
   if(d.date.size()==1)
   {
      if(pool[p]->clinic)addstr("a \"hot\" date with ");
      else addstr("a hot date with ");
   }
   else addstr("dates to manage with ");
   for(e=0;e<d.date.size();e++)
   {
      addstr(d.date[e]->name);

      if(e<=d.date.size()-3)addstr(", ");
      else if(e==d.date.size()-2)addstr(" and ");
      else
      {
         if(pool[p]->clinic>0)
         {
            addstr(" at ");
            addstr(location[pool[p]->location]->name);
         }
         addstr(".");
      }
   }

   refresh();
   getch();

   if(d.date.size()>1&&!LCSrandom(5))
   {
      move(2,0);
      addstr("Unfortunately, they all know each other and had been discussing");
      move(3,0);
      addstr(pool[p]->name);
      addstr(".  An ambush was set for the lying dog...");
      refresh();
      getch();

      move(5,0);
      addstr(pool[p]->name);
      addstr(" has failed the Liberal Crime Squad.");
      addjuice(*pool[p],-5);
      refresh();
      getch();

      return 1;
   }

   for(e=d.date.size()-1;e>=0;e--)
   {
      erase();
      set_color(COLOR_WHITE,COLOR_BLACK,1);
      move(0,0);
      addstr("Seeing ");
      addstr(d.date[e]->name);
      addstr(", ");
      switch(d.date[e]->align)
      {
         case -1:addstr("Conservative");break;
         case 0:addstr("moderate");break;
         case 1:addstr("Liberal");break;
      }

      set_color(COLOR_WHITE,COLOR_BLACK,0);
      printfunds(0,1,"Money: ");

      //Others come to dates unarmed and wearing normal
      //clothing
      weaponst weapon = weapon = d.date[e]->weapon;
      d.date[e]->weapon.type = WEAPON_NONE;
      armorst armor = d.date[e]->armor;
      d.date[e]->armor.type = ARMOR_CLOTHES;

      printcreatureinfo(d.date[e]);
      makedelimiter(8,0);

      d.date[e]->weapon = weapon;
      d.date[e]->armor = armor;

      move(10,0);
      addstr("How should ");
      addstr(pool[p]->name);
      addstr(" approach the situation?");

      if(funds>=100&&!pool[p]->clinic)set_color(COLOR_WHITE,COLOR_BLACK,0);
      else set_color(COLOR_BLACK,COLOR_BLACK,1);
      move(11,0);
      addstr("A - Spend a hundred bucks tonight to get the ball rolling.");
      set_color(COLOR_WHITE,COLOR_BLACK,0);
      move(12,0);
      addstr("B - Try to get through the evening without spending a penny.");
      if(!pool[p]->clinic&&pool[p]->blood==100)set_color(COLOR_WHITE,COLOR_BLACK,0);
      else set_color(COLOR_BLACK,COLOR_BLACK,1);
      move(13,0);
      if(pool[p]->blood==100) addstr("C - Spend a week on a cheap vacation (stands up any other dates).");
      else addstr("C - Spend a week on a cheap vacation (must be uninjured).");
      set_color(COLOR_WHITE,COLOR_BLACK,0);
      move(14,0);
      addstr("D - Break it off.");
      if(d.date[e]->align==-1&&!pool[p]->clinic)
      {
         set_color(COLOR_WHITE,COLOR_BLACK,0);
         move(15,0);
         addstr("E - Just kidnap the Conservative bitch.");
      }

      do
      {
         refresh();
         int c=getch();
         translategetch(c);

         short aroll=LCSrandom(21)+pool[p]->attval(ATTRIBUTE_CHARISMA)+pool[p]->skillval(SKILL_SEDUCTION)*2;
         short troll=LCSrandom(21)+d.date[e]->attval(ATTRIBUTE_WISDOM)*2;
         if(d.date[e]->align==ALIGN_CONSERVATIVE)
            troll+=troll*(d.date[e]->juice/100);

         char test=0;
         if(c=='a'&&funds>=100&&!pool[p]->clinic)
         {
            funds-=100;
            stat_spent+=100;
            moneylost_dating+=100;
            aroll+=LCSrandom(10);
            test=1;
            pool[p]->train(SKILL_SEDUCTION,LCSrandom(8)+3);
            pool[p]->train(SKILL_SCIENCE,
               max(d.date[e]->skillval(SKILL_SCIENCE)-pool[p]->skillval(SKILL_SCIENCE),0));
            pool[p]->train(SKILL_RELIGION,
               max(d.date[e]->skillval(SKILL_RELIGION)-pool[p]->skillval(SKILL_RELIGION),0));
            pool[p]->train(SKILL_BUSINESS,
               max(d.date[e]->skillval(SKILL_BUSINESS)-pool[p]->skillval(SKILL_BUSINESS),0));
         }
         if(c=='b')
         {
            test=1;
            pool[p]->train(SKILL_SEDUCTION,LCSrandom(8)+3);
            pool[p]->train(SKILL_SCIENCE,
               max(d.date[e]->skillval(SKILL_SCIENCE)-pool[p]->skillval(SKILL_SCIENCE),0));
            pool[p]->train(SKILL_RELIGION,
               max(d.date[e]->skillval(SKILL_RELIGION)-pool[p]->skillval(SKILL_RELIGION),0));
            pool[p]->train(SKILL_BUSINESS,
               max(d.date[e]->skillval(SKILL_BUSINESS)-pool[p]->skillval(SKILL_BUSINESS),0));
         }

         if(d.date[e]->skillval(SKILL_BUSINESS))
         {
            troll+=d.date[e]->skillval(SKILL_BUSINESS);
            if(pool[p]->skillval(SKILL_BUSINESS))
               aroll+=pool[p]->skillval(SKILL_BUSINESS);
         }
         if(d.date[e]->skillval(SKILL_RELIGION))
         {
            troll+=d.date[e]->skillval(SKILL_RELIGION);
            if(pool[p]->skillval(SKILL_RELIGION))
               aroll+=pool[p]->skillval(SKILL_RELIGION);
         }
         if(d.date[e]->skillval(SKILL_SCIENCE))
         {
            troll+=d.date[e]->skillval(SKILL_SCIENCE);
            if(pool[p]->skillval(SKILL_SCIENCE))
               aroll+=pool[p]->skillval(SKILL_SCIENCE);
         }

         if(test)
         {
            int y=17;
            int result = dateresult(aroll,troll,d,e,p,y);
            if(result == DATERESULT_ARRESTED)return 1;
            break;
         }

         if(c=='c'&&!pool[p]->clinic&&pool[p]->blood==100)
         {
            for(int e2=d.date.size()-1;e2>=0;e2--)
            {
               if(e2==e)continue;
               delete d.date[e2];
               d.date.erase(d.date.begin() + e2);
            }
            d.timeleft=7;
            pool[p]->train(SKILL_SEDUCTION,LCSrandom(40)+15);
            pool[p]->train(SKILL_SCIENCE,
               max((d.date[e]->skillval(SKILL_SCIENCE)-pool[p]->skillval(SKILL_SCIENCE))*4,0));
            pool[p]->train(SKILL_RELIGION,
               max((d.date[e]->skillval(SKILL_RELIGION)-pool[p]->skillval(SKILL_RELIGION))*4,0));
            pool[p]->train(SKILL_BUSINESS,
               max((d.date[e]->skillval(SKILL_BUSINESS)-pool[p]->skillval(SKILL_BUSINESS))*4,0));
            return 0;
         }
         if(c=='d')
         {
            delete d.date[e];
            d.date.erase(d.date.begin() + e);
            break;
         }
         if(c=='e'&&d.date[e]->align==-1&&!pool[p]->clinic)
         {
            
            set_color(COLOR_YELLOW,COLOR_BLACK,1);
            int bonus=0;
            move(17,0);
            addstr(pool[p]->name);
            char str[30];
            getweaponfull(str,pool[p]->weapon.type,1);

            if(pool[p]->weapon.ranged())
            {
               addstr(" comes back from the bathroom toting the ");
               addstr(str);
               move(18,0);
               addstr("and threatens to blow the Conservative's brains out!");
               bonus=5;
            }
            else if(pool[p]->weapon.type!=WEAPON_NONE)
            {
               addstr(" grabs the Conservative from behind, holding the ");
               addstr(str);
               move(18,0);
               addstr("to the corporate slave's throat!");
               bonus=2;
            }
            else
            {
               addstr(" seizes the Conservative swine from behind and warns it");
               move(18,0);
               if(law[LAW_FREESPEECH]!=-2)addstr("not to fuck around!");
               else addstr("not to [resist]!");
            }

            refresh();
            getch();


            // *JDS* Kidnap succeeds if the conservative isn't very dangerous,
            // but might fail (1 in 3 chance) if the conservative is
            // tough stuff.
            if((d.date[e]->type!=CREATURE_AGENT&&
                d.date[e]->type!=CREATURE_COP&&
                d.date[e]->type!=CREATURE_SWAT&&
                d.date[e]->type!=CREATURE_GANGUNIT&&
                d.date[e]->type!=CREATURE_DEATHSQUAD&&
                d.date[e]->type!=CREATURE_SOLDIER&&
                d.date[e]->type!=CREATURE_VETERAN&&
                d.date[e]->type!=CREATURE_HARDENED_VETERAN&&
                d.date[e]->type!=CREATURE_CCS_VIGILANTE&&
                d.date[e]->type!=CREATURE_CCS_ARCHCONSERVATIVE&&
                d.date[e]->type!=CREATURE_CCS_MOLOTOV&&
                d.date[e]->type!=CREATURE_CCS_SNIPER&&
                d.date[e]->type!=CREATURE_MERC&&
                LCSrandom(15))||
                LCSrandom(1+bonus))
            {
               
               set_color(COLOR_GREEN,COLOR_BLACK,1);
               move(20,0);
               addstr(d.date[e]->name);
               if(bonus)
               {
                  addstr(" doesn't resist.");
               }
               else
               {
                  addstr(" struggles and yells for help, but nobody comes.");
               }

               refresh();
               getch();
               move(22,0);
               addstr(pool[p]->name);
               addstr(" kidnaps the Conservative!");
               refresh();
               getch();

               generate_name(d.date[e]->name);
               strcpy(d.date[e]->propername,d.date[e]->name);

               d.date[e]->location=pool[p]->location;
               d.date[e]->base=pool[p]->base;
               d.date[e]->flag|=CREATUREFLAG_MISSING;

               //Kidnapped wearing normal clothes and no weapon
               d.date[e]->weapon.ammo = ARMOR_NONE;
               d.date[e]->weapon.type = WEAPON_NONE;
               d.date[e]->armor.type = ARMOR_CLOTHES;

               //Create interrogation data
               d.date[e]->activity.arg=reinterpret_cast<long>(new interrogation);

               erase();

               set_color(COLOR_WHITE,COLOR_BLACK,1);
               move(0,0);
               addstr("The Education of ");
               addstr(d.date[e]->propername);

               move(2,0);
               set_color(COLOR_WHITE,COLOR_BLACK,0);
               addstr("What name will you use for this ");
               char str[80];
               getrecruitcreature(str,d.date[e]->type);
               addstr(str);
               addstr(" in its presence?");

               move(3,0);
               addstr("If you do not enter anything, their real name will be used.");

               move(4,0);
               enter_name(d.date[e]->name,CREATURE_NAMELEN,d.date[e]->propername);

               pool.push_back(d.date[e]);
               stat_kidnappings++;
               d.date.erase(d.date.begin() + e);
               break;
            }
            else
            {
               int y=20;
               if(LCSrandom(2))
               {
                  set_color(COLOR_MAGENTA,COLOR_BLACK,1);
                  move(y,0);y++;
                  addstr(d.date[e]->name);
                  addstr(" manages to get away on the way back to the safehouse!");
                  refresh();
                  getch();
                  move(++y,0);y++;
                  addstr(pool[p]->name);
                  addstr(" has failed to kidnap the Conservative.");

                  // Charge with kidnapping
                  criminalize(*pool[p],LAWFLAG_KIDNAPPING);

                  refresh();
                  getch();
                  
                  delete d.date[e];
                  d.date.erase(d.date.begin() + e);
                  break;
               }
               else
               {
                  set_color(COLOR_RED,COLOR_BLACK,1);
                  move(y,0);y++;
                  addstr(d.date[e]->name);
                  addstr("'s fist is the last thing ");
                  addstr(pool[p]->name);
                  addstr(" remembers seeing!");
                  refresh();
                  getch();
                  move(++y,0);y++;
                  addstr("The Liberal wakes up in the police station...");

                  // Find the police station
                  int ps=-1;
                  for(int l=0;l<location.size();l++)
                  {
                     if(location[l]->type==SITE_GOVERNMENT_POLICESTATION)
                     {
                        ps=l;
                     }
                  }
                     
                  // Arrest the Liberal
                  removesquadinfo(*pool[p]);
                  pool[p]->carid=-1;
                  pool[p]->location=ps;
                  pool[p]->weapon.type=WEAPON_NONE;
                  pool[p]->weapon.ammo=0;
                  pool[p]->activity.type=ACTIVITY_NONE;

                  // Charge with kidnapping
                  criminalize(*pool[p],LAWFLAG_KIDNAPPING);

                  refresh();
                  getch();
                  
                  delete d.date[e];
                  d.date.erase(d.date.begin() + e);
                  return 1;
               }
            }
         }
      }while(1);
   }

   if(d.date.size()>0)
   {
      d.timeleft=0;
      return 0;
   }
   else return 1;
}
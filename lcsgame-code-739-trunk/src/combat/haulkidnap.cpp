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

//TODO: This file probably shouldn't have any nextMessage calls in it.
//That will depend on the chase and fight code, I think...And the code that
//handles the squad moving around the site. For now, this'll do. I must remember
//to come back here and finish the job.

//#include <includes.h>
#include <externs.h>



/* prompt after you've said you want to kidnap someone */
void kidnapattempt(void)
{
   short kidnapper=-1;

   party_status=-1;

   int available=0;
   char availslot[6]={0,0,0,0,0,0};
   for(int p=0;p<6;p++)
   {
      if(activesquad->squad[p]!=NULL)
      {
         if(activesquad->squad[p]->alive&&activesquad->squad[p]->prisoner==NULL)
         {
            available++;
            availslot[p]=1;
         }
      }
   }

   if(!available)
   {
      set_color(COLOR_WHITE,COLOR_BLACK,1);
      move(16,1);
      addstr("No one can do the job.            ");
      move(17,1);
      addstr("                                  ");

      refresh();
      getch();

      return;
   }

   do
   {
      printparty();

      move(8,20);
      set_color(COLOR_WHITE,COLOR_BLACK,1);
      addstr("Choose a Liberal squad member to do the job.");

      refresh();

      int c=getch();
      translategetch(c);

      if(c==10||c==ESC)return;

      if(c>='1'&&c<='6')
      {
         if(availslot[c-'1'])
         {
            kidnapper=c-'1';
            break;
         }
      }

   }while(1);

   if(kidnapper>=0)
   {
      vector<int> target;

      for(int e=0;e<ENCMAX;e++)
      {
         if(encounter[e].exists&&encounter[e].alive&&encounter[e].align==-1&&
            (encounter[e].animalgloss==ANIMALGLOSS_NONE||law[LAW_ANIMALRESEARCH]==2))
         {
            if((encounter[e].get_weapon().protects_against_kidnapping()&&
               encounter[e].blood>20) || encounter[e].animalgloss==ANIMALGLOSS_TANK)continue;
            target.push_back(e);
         }
      }

      if(target.size()>0)
      {
         int t=target[0];

         if(target.size()>1)
         {
            do
            {
               clearcommandarea();
               clearmessagearea();
               clearmaparea();

               set_color(COLOR_WHITE,COLOR_BLACK,1);
               move(9,1);
               addstr("Kidnap whom?");

               int x=1,y=11;
               for(int t2=0;t2<(int)target.size();t2++)
               {
                  move(y,x);
                  addch(t2+'A');
                  addstr(" - ");
                  addstr(encounter[target[t2]].name);

                  y++;
                  if(y==17)
                  {
                     y=11;
                     x+=30;
                  }
               }

               refresh();
               int c=getch();
               translategetch(c);
               if(c>='a'&&c<='z')
               {
                  t=c-'a';
                  if(t>=(int)target.size())t=-1;
                  else {t=target[t];}break;
               }
               if(c==10||c==ESC||c==32){t=-1;break;}
            }while(1);
         }

         if(t!=-1)
         {
            char amateur=0;

            if(kidnap(*activesquad->squad[kidnapper],encounter[t],amateur))
            {
               delenc(t,0);

               int time=20+LCSrandom(10);
               if(time<1)time=1;
               if(sitealarmtimer>time||sitealarmtimer==-1)sitealarmtimer=time;
            }
            else sitealarm=1;

            if(amateur)
            {
               char present=0;
               for(int e=0;e<ENCMAX;e++)
               {
                  if(encounter[e].exists&&encounter[e].alive){present=1;break;}
               }

               if(present)
               {
                  alienationcheck(1);
                  if(!sitealarm)sitealarm=1;
                  sitecrime+=5;
                  criminalizeparty(LAWFLAG_KIDNAPPING);
                  if(activesquad->squad[kidnapper]->prisoner!=NULL)
                  {
                     if(activesquad->squad[kidnapper]->prisoner->type==CREATURE_RADIOPERSONALITY)offended_amradio=1;
                     if(activesquad->squad[kidnapper]->prisoner->type==CREATURE_NEWSANCHOR)offended_cablenews=1;
                  }
                  else
                  {
                     if(encounter[t].type==CREATURE_RADIOPERSONALITY)offended_amradio=1;
                     if(encounter[t].type==CREATURE_NEWSANCHOR)offended_cablenews=1;
                  }
               }
            }

            if(sitealarm)enemyattack();
            creatureadvance();
         }
      }
      else
      {
         set_color(COLOR_WHITE,COLOR_BLACK,1);
         move(16,1);
         addstr("All of the targets are too dangerous.                ");
         move(17,1);
         addstr("                                                     ");

         refresh();
         getch();
      }
   }
   else
   {
      set_color(COLOR_WHITE,COLOR_BLACK,1);
      move(16,1);
      addstr("No one can do the job.                                  ");
      move(17,1);
      addstr("                                                        ");

      refresh();
      getch();
   }
}

/* prompt after you've said you want to release someone */
void releasehostage(void)
{
   short kidnapper=-1;

   party_status=-1;

   int available=0;
   char availslot[6]={0,0,0,0,0,0};
   for(int p=0;p<6;p++)
   {
      if(activesquad->squad[p]!=NULL)
      {
         if(activesquad->squad[p]->alive&&activesquad->squad[p]->prisoner!=NULL&&activesquad->squad[p]->prisoner->align!=ALIGN_LIBERAL)
         {
            available++;
            availslot[p]=1;
         }
      }
   }

   if(!available)
   {
      set_color(COLOR_WHITE,COLOR_BLACK,1);
      move(16,1);
      addstr("No hostages are being held.       ");
      move(17,1);
      addstr("                                  ");

      refresh();
      getch();

      return;
   }

   do
   {
      printparty();

      move(8,20);
      set_color(COLOR_WHITE,COLOR_BLACK,1);
      addstr("Choose a Liberal squad member to release their hostage.");

      refresh();

      int c=getch();
      translategetch(c);

      if(c==10||c==ESC)return;

      if(c>='1'&&c<='6')
      {
         if(availslot[c-'1'])
         {
            kidnapper=c-'1';
            break;
         }
      }

   }while(1);

   activesquad->squad[kidnapper]->prisoner->cantbluff=2;
   freehostage(*(activesquad->squad[kidnapper]), 2);

   if(!sitealarm)
   {
      set_color(COLOR_WHITE,COLOR_BLACK,1);
      move(16,1);
      addstr("The hostage shouts for help!      ", gamelog);
      gamelog.nextMessage(); //Next message.
      move(17,1);
      addstr("                                  ");

      getch();
      sitealarm=1;
      alienationcheck(1);
   }
}



/* roll on the kidnap attempt and show the results */
char kidnap(Creature &a,Creature &t,char &amateur)
{
   if(!a.get_weapon().can_take_hostages())
   {
      amateur=1;

      //BASIC ROLL
      int aroll=a.skill_roll(SKILL_HANDTOHAND);
      int droll=t.attribute_check(ATTRIBUTE_AGILITY,true);

      a.train(SKILL_HANDTOHAND,droll);

      clearmessagearea();

      //HIT!
      if(aroll>droll)
      {
         set_color(COLOR_WHITE,COLOR_BLACK,1);
         move(16,1);
         addstr(a.name, gamelog);
         addstr(" snatches ", gamelog);
         addstr(t.name, gamelog);
         addstr("!", gamelog);
         gamelog.newline(); //New line.

         a.prisoner=new Creature;
         *a.prisoner=t;

         refresh();
         getch();

         set_color(COLOR_RED,COLOR_BLACK,1);
         move(17,1);
         addstr(t.name, gamelog);
         addstr(" is struggling and screaming!", gamelog);
         gamelog.newline(); //New line.

         refresh();
         getch();

         gamelog.newline();
         return 1;
      }
      else
      {
         set_color(COLOR_MAGENTA,COLOR_BLACK,1);
         move(16,1);
         addstr(a.name, gamelog);
         addstr(" grabs at ", gamelog);
         addstr(t.name, gamelog);
         gamelog.newline(); //New line.
         move(17,1);
         addstr("but ", gamelog);
         addstr(t.name, gamelog);
         addstr(" writhes away!", gamelog);
         gamelog.newline(); //New line.

         refresh();
         getch();


         gamelog.newline();
         return 0;
      }
   }
   else
   {
      clearmessagearea();

      set_color(COLOR_WHITE,COLOR_BLACK,1);
      move(16,1);
      addstr(a.name, gamelog);
      addstr(" shows ", gamelog);
      addstr(t.name, gamelog);
      addstr(" the ", gamelog);
      addstr(a.get_weapon().get_name(2).c_str(), gamelog);
      addstr(" " , gamelog);
      move(17,1);
      addstr("and says, ", gamelog);
      set_color(COLOR_GREEN,COLOR_BLACK,1);
      if(law[LAW_FREESPEECH]==-2)addstr("\"[Please], be cool.\"", gamelog);
      else addstr("\"Bitch, be cool.\"", gamelog);

      a.prisoner=new Creature;
      *a.prisoner=t;

      refresh();
      getch();

      gamelog.newline();
      return 1;
   }
}



/* hostage freed due to host unable to haul */
void freehostage(Creature &cr,char situation)
{
   if(cr.prisoner==NULL)return;

   if(cr.prisoner->alive)
   {
      if(situation==0)
      {
         if(cr.prisoner->squadid==-1)addstr(" and a hostage is freed", gamelog);
         else
         {
            addstr(" and ", gamelog);
            addstr(cr.prisoner->name, gamelog);
            if(cr.prisoner->flag & CREATUREFLAG_JUSTESCAPED)addstr(" is recaptured", gamelog);
            else addstr(" is captured", gamelog);
         }
         gamelog.newline(); //New line.
      }
      else if(situation==1)
      {
         clearmessagearea();
         set_color(COLOR_WHITE,COLOR_BLACK,1);
         move(16,1);
         if(cr.prisoner->squadid==-1)addstr("A hostage escapes!", gamelog);
         else
         {
            addstr(cr.prisoner->name, gamelog);
            if(cr.prisoner->flag & CREATUREFLAG_JUSTESCAPED)addstr(" is recaptured.", gamelog);
            else addstr(" is captured.", gamelog);
         }
         gamelog.newline(); //New line.
      }
      else if(situation==2)
      {
              //Don't print anything.
      }

      if(cr.prisoner->squadid==-1)
      {
         for(int e=0;e<ENCMAX;e++)
         {
            if(encounter[e].exists==0)
            {
               encounter[e]=*cr.prisoner;
               encounter[e].exists=1;
               conservatise(encounter[e]);
               break;
            }
         }
         delete cr.prisoner;
      }
      else capturecreature(*cr.prisoner);
   }
   else
   {
      if(cr.prisoner->squadid!=-1)
      {
         //MUST DELETE PARTY MEMBER FROM POOL COMPLETELY
         //(That may not be the case any longer -jds)
         for(int pl=0;pl<(int)pool.size();pl++)
         {
            if(pool[pl]==cr.prisoner)
            {
               removesquadinfo(*pool[pl]);
               pool[pl]->die();
               pool[pl]->location=-1;
               //delete_and_remove(pool,pl);
               break;
            }
         }
      }
   }

   cr.prisoner=NULL;

   if(situation==1)
   {
      printparty();
      if(mode==GAMEMODE_CHASECAR||
                  mode==GAMEMODE_CHASEFOOT)printchaseencounter();
      else printencounter();
      refresh();
      getch();
   }
}



/* haul dead/paralyzed */
void squadgrab_immobile(char dead)
{
   int p;
   //DRAGGING PEOPLE OUT IF POSSIBLE
   int hostslots=0;
   for(p=0;p<6;p++)
   {
      if(activesquad->squad[p]!=NULL)
      {
         if(activesquad->squad[p]->alive&&
            (activesquad->squad[p]->canwalk()||
            (activesquad->squad[p]->flag & CREATUREFLAG_WHEELCHAIR))&&
            activesquad->squad[p]->prisoner==NULL)
         {
            hostslots++;
         }
         else if((!activesquad->squad[p]->alive||
            (!activesquad->squad[p]->canwalk()&&
            !(activesquad->squad[p]->flag & CREATUREFLAG_WHEELCHAIR)))&&
            activesquad->squad[p]->prisoner!=NULL)
         {
            clearmessagearea();
            set_color(COLOR_YELLOW,COLOR_BLACK,1);
            move(16,1);
            addstr(activesquad->squad[p]->name, gamelog);
            addstr(" can no longer handle ", gamelog);
            addstr(activesquad->squad[p]->prisoner->name, gamelog);
            addstr(".", gamelog);
            gamelog.newline(); //New line.
            refresh();
            getch();

            freehostage(*activesquad->squad[p]->prisoner,1);
         }
      }
   }

   for(p=5;p>=0;p--)
   {
      if(activesquad->squad[p]!=NULL)
      {
         if((!activesquad->squad[p]->alive&&dead)||
            (activesquad->squad[p]->alive&&
            !(activesquad->squad[p]->flag & CREATUREFLAG_WHEELCHAIR)&&
            !activesquad->squad[p]->canwalk()&&!dead))
         {
            if(hostslots==0)
            {
               if(!activesquad->squad[p]->alive)
               {
                  clearmessagearea();
                  set_color(COLOR_YELLOW,COLOR_BLACK,1);
                  move(16,1);
                  addstr("Nobody can carry Martyr ", gamelog);
                  addstr(activesquad->squad[p]->name, gamelog);
                  addstr(".", gamelog);
                  gamelog.newline();

                  //DROP LOOT
                  makeloot(*activesquad->squad[p],groundloot);

                  //MUST DELETE PARTY MEMBER FROM POOL COMPLETELY
                  //(that may not be the case any longer -jds)
                  for(int pl=0;pl<(int)pool.size();pl++)
                  {
                     if(pool[pl]==activesquad->squad[p])
                     {
                        pool[pl]->die();
                        pool[pl]->location=-1;
                        //delete_and_remove(pool,pl);
                        break;
                     }
                  }

                  //Check if that was the last living squad member.
                  //bool squad_dead = true;
                  for(int i = 5; i >= 0; --i)
                  {
                     //This check to make sure we don't look at nonexistent
                     //data, causing segfaults and the like.
                     if(activesquad->squad[i] != NULL)
                     {
                        //Check if alive.
                        if(activesquad->squad[i]->alive)
                        {
                           //IT LIVES! Squad is not dead.
                           //squad_dead = false;
                           break; //No reason to continue
                        }
                     }
                  }
               }
               else
               {
                  clearmessagearea();
                  set_color(COLOR_YELLOW,COLOR_BLACK,1);
                  move(16,1);
                  addstr(activesquad->squad[p]->name, gamelog);
                  addstr(" is left to be captured.", gamelog);
                  gamelog.newline(); //New line.

                  capturecreature(*activesquad->squad[p]);
               }
            }
            else
            {
               for(int p2=0;p2<6;p2++)
               {
                  if(p2==p)continue;
                  if(activesquad->squad[p2]!=NULL)
                  {
                     if(activesquad->squad[p2]->alive&&
                        (activesquad->squad[p2]->canwalk()||
                        (activesquad->squad[p2]->flag & CREATUREFLAG_WHEELCHAIR))&&
                        activesquad->squad[p2]->prisoner==NULL)
                     {
                        activesquad->squad[p2]->prisoner=activesquad->squad[p];

                        clearmessagearea();
                        set_color(COLOR_YELLOW,COLOR_BLACK,1);
                        move(16,1);
                        addstr(activesquad->squad[p2]->name, gamelog);
                        addstr(" hauls ", gamelog);
                        addstr(activesquad->squad[p]->name, gamelog);
                        addstr(".", gamelog);
                        gamelog.newline(); //New line.
                        break;
                     }
                  }
               }

               hostslots--;
            }

            //SHUFFLE SQUAD
            char flipstart=0;
            for(int pt=0;pt<6;pt++)
            {
               if(pt==p-1)continue;
               if(pt==p){flipstart=1;}
               if(flipstart&&pt<5)activesquad->squad[pt]=activesquad->squad[pt+1];
            }
            if(flipstart)activesquad->squad[5]=NULL;

            printparty();
            refresh();
            getch();
         }
      }
   }
}



/* names the new hostage and stashes them in your base */
void kidnaptransfer(Creature &cr)
{
   Creature *newcr=new Creature;
      *newcr=cr;
   newcr->namecreature();

   newcr->location=activesquad->squad[0]->base;
   newcr->base=activesquad->squad[0]->base;
   newcr->flag|=CREATUREFLAG_MISSING;

   //disarm them and stash their weapon back at the base
   newcr->drop_weapons_and_clips(&(location[newcr->location]->loot));

   //Create interrogation data
   newcr->activity.arg=reinterpret_cast<long>(new interrogation);

   erase();

   set_color(COLOR_WHITE,COLOR_BLACK,1);
   move(0,0);
   addstr("The Education of ");
   addstr(newcr->propername);

   move(2,0);
   set_color(COLOR_WHITE,COLOR_BLACK,0);
   addstr("What name will you use for this ");
   addstr(newcr->get_type_name());
   addstr(" in its presence?");

   move(3,0);
   addstr("If you do not enter anything, their real name will be used.");
   move(4,0);
   enter_name(newcr->name,CREATURE_NAMELEN,newcr->propername);

   pool.push_back(newcr);
   stat_kidnappings++;
}

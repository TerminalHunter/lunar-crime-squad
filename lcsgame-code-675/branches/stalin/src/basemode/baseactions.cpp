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


/* base - burn the flag */
void burnflag(void)
{
   int flagparts=112;
   long flag[16][7][4];
   int x;
   int y;

   for(int p=0;p<7;p++)
   {
      if(p<3)
      {
         for(x=0;x<6;x++)
         {
            flag[x][p][0]=':';
            flag[x][p][1]=COLOR_WHITE;
            flag[x][p][2]=COLOR_BLUE;
            flag[x][p][3]=1;
         }

         for(x=6;x<16;x++)
         {
            flag[x][p][0]=CH_LOWER_HALF_BLOCK;
            flag[x][p][3]=1;
            flag[x][p][1]=COLOR_WHITE;
            flag[x][p][2]=COLOR_RED;
         }
      }
      else
      {
         for(x=0;x<16;x++)
         {
            if(p<6)
            {
               flag[x][p][0]=CH_LOWER_HALF_BLOCK;
               flag[x][p][1]=COLOR_WHITE;
               flag[x][p][2]=COLOR_RED;
               flag[x][p][3]=1;
            }
            else
            {
               flag[x][p][0]=CH_UPPER_HALF_BLOCK;
               flag[x][p][1]=COLOR_RED;
               flag[x][p][2]=COLOR_BLACK;
               flag[x][p][3]=0;
            }
         }
      }
   }

   x=LCSrandom(16);
   y=LCSrandom(7);
   flag[x][y][0]=CH_DARK_SHADE;
   flag[x][y][1]=COLOR_YELLOW;
   flag[x][y][2]=COLOR_BLACK;
   flag[x][y][3]=1;

   char first=1;

   while(flagparts>0)
   {
      if(!first)
      {
         for(x=0;x<16;x++)
         {
            for(y=0;y<7;y++)
            {
               if(flag[x][y][0]==CH_BOX_DRAWINGS_LIGHT_VERTICAL)flag[x][y][0]=CH_DARK_SHADE;
               else if(flag[x][y][0]==CH_DARK_SHADE)
               {
                  flag[x][y][0]=CH_MEDIUM_SHADE;
                  flag[x][y][1]=COLOR_RED;
                  flag[x][y][2]=COLOR_BLACK;
                  flag[x][y][3]=0;
               }
               else if(flag[x][y][0]==CH_MEDIUM_SHADE)
               {
                  flag[x][y][0]=CH_LIGHT_SHADE;
                  flag[x][y][1]=COLOR_BLACK;
                  flag[x][y][2]=COLOR_BLACK;
                  flag[x][y][3]=1;
               }
               else if(flag[x][y][0]==CH_LIGHT_SHADE)
               {
                  flagparts--;
                  flag[x][y][0]=' ';
                  flag[x][y][1]=COLOR_BLACK;
                  flag[x][y][2]=COLOR_BLACK;
                  flag[x][y][3]=0;
               }
            }
         }
      }
      else first=0;

      for(x=0;x<16;x++)
      {
         for(y=0;y<7;y++)
         {
            move(y+10,x+32);
            set_color(short(flag[x][y][1]),short(flag[x][y][2]),char(flag[x][y][3]));
            addch(flag[x][y][0]);
         }
      }
      refresh();

      pause_ms(10);

      char gotnew=0;
      while(!gotnew&&flagparts>3)
      {
         x=LCSrandom(16);
         y=LCSrandom(7);
         char conf=0;
         if(flag[x][y][0]==':'||flag[x][y][0]==CH_UPPER_HALF_BLOCK||flag[x][y][0]==CH_LOWER_HALF_BLOCK)
         {
            if(x>0)
            {
               if(flag[x-1][y][0]!=':'&&
                  flag[x-1][y][0]!=CH_UPPER_HALF_BLOCK&&
                  flag[x-1][y][0]!=CH_LOWER_HALF_BLOCK)conf=1;
            }
            if(x<15)
            {
               if(flag[x+1][y][0]!=':'&&
                  flag[x+1][y][0]!=CH_UPPER_HALF_BLOCK&&
                  flag[x+1][y][0]!=CH_LOWER_HALF_BLOCK)conf=1;
            }
            if(y>0)
            {
               if(flag[x][y-1][0]!=':'&&
                  flag[x][y-1][0]!=CH_UPPER_HALF_BLOCK&&
                  flag[x][y-1][0]!=CH_LOWER_HALF_BLOCK)conf=1;
            }
            if(y<6)
            {
               if(flag[x][y+1][0]!=':'&&
                  flag[x][y+1][0]!=CH_UPPER_HALF_BLOCK&&
                  flag[x][y+1][0]!=CH_LOWER_HALF_BLOCK)conf=1;
            }
            if(conf)
            {
               flag[x][y][0]=CH_BOX_DRAWINGS_LIGHT_VERTICAL;
               flag[x][y][1]=COLOR_YELLOW;
               flag[x][y][2]=COLOR_BLACK;
               flag[x][y][3]=1;
               gotnew=1;
            }
         }
      }
   }
}

/* base - new slogan */
void getslogan(void)
{
   set_color(COLOR_WHITE,COLOR_BLACK,0);

   move(15,0);
   addstr("What is your new slogan?");
   move(16,0);
   addstr("                                                                                          ");

   keypad(stdscr,FALSE);
   raw_output(FALSE);
   echo();
   curs_set(1);
   move(16,0);
   enter_name(slogan,SLOGAN_LEN);

   curs_set(0);
   noecho();
   raw_output(TRUE);
   keypad(stdscr,TRUE);
}



/* base - reorder party */
void orderparty(void)
{
   if(activesquad==NULL)return;

   int partysize=0;
   for(int p=0;p<6;p++)
   {
      if(activesquad->squad[p]!=NULL)partysize++;
   }

   if(partysize<=1)return;

   int spot=0;

   do
   {
      printparty();

      move(8,20);
      set_color(COLOR_WHITE,COLOR_BLACK,1);
      addstr("Choose a Liberal squad member for Place ");
      char num[20];
      itoa(spot+1,num,10);
      addstr(num);
      addstr(".");

      refresh();

      int c=getch();
      translategetch(c);

      if(c==10)return;

      if(c>=spot+'1'&&c<=partysize+'1'-1)
      {
         Creature *swap=activesquad->squad[spot];
         activesquad->squad[spot]=activesquad->squad[c-'1'];
         activesquad->squad[c-'1']=swap;
         spot++;
      }
   }while(spot<partysize-1);
}



/* base - go forth to stop evil */
void stopevil(void)
{
   int l = 0;
   int p = 0;

   if(activesquad==NULL)return;

   char havecar=0;
   for(p=0;p<6;p++)
   {
      if(activesquad->squad[p]!=NULL)
      {
         if(activesquad->squad[p]->pref_carid!=-1)
         {
            havecar=1;
            break;
         }
      }
   }

   int page=0;
   int loc=-1;

   vector<long> temploc;
   for(l=0;l<location.size();l++)
   {
      if(location[l]->parent==loc&&location[l]->renting>=0&&!location[l]->hidden)temploc.push_back(l);
   }
   for(l=0;l<location.size();l++)
   {
      //locationst* loc2 = location[l]; //what was this for?
      if(location[l]->parent==loc&&location[l]->renting==-2&&!location[l]->hidden)temploc.push_back(l);
   }
   for(l=0;l<location.size();l++)
   {
      if(location[l]->parent==loc&&location[l]->renting==-1&&!location[l]->hidden)temploc.push_back(l);
   }

   do
   {
      erase();

      set_color(COLOR_WHITE,COLOR_BLACK,0);
      move(0,0);
      addstr("Where will the Squad go?");

      printparty();

      if(loc!=-1)
      {
         set_color(COLOR_WHITE,COLOR_BLACK,0);
         move(8,0);
         addlocationname(location[loc]);
      }

      move(12,50);
      addstr("Z - Toggle Squad Stance");
      switch(activesquad->stance)
      {
      case SQUADSTANCE_ANONYMOUS:
         move(13,50);
         set_color(COLOR_WHITE,COLOR_BLACK,1);
         addstr("ANONYMOUS ACTION");
         move(14,50);
         set_color(COLOR_BLACK,COLOR_BLACK,1);
         addstr("Unlikely to make the news");
         break;
      case SQUADSTANCE_STANDARD:
         move(13,50);
         set_color(COLOR_GREEN,COLOR_BLACK,1);
         addstr("CLAIMED ACTION");
         move(14,50);
         set_color(COLOR_BLACK,COLOR_BLACK,1);
         addstr("Likely to make the news");
         break;
      case SQUADSTANCE_BATTLECOLORS:
         move(13,50);
         set_color(COLOR_GREEN,COLOR_BLACK,1);
         addstr("GREEN ARMBANDS");
         move(14,50);
         set_color(COLOR_BLACK,COLOR_BLACK,1);
         addstr("(No Stealth, High Profile)");
         break;
      }


      int y=10;
      for(p=page*11;p<temploc.size()&&p<page*11+11;p++)
      {
         set_color(COLOR_WHITE,COLOR_BLACK,0);
         move(y,0);
         addch(y-10+(int)'A');
         addstr(" - ");

         addlocationname(location[temploc[p]]);

         if(temploc[p]==activesquad->squad[0]->location)
         {
            set_color(COLOR_WHITE,COLOR_BLACK,1);
            addstr(" (Current Location)");
         }
         else if(location[temploc[p]]->renting>=0)
         {
            set_color(COLOR_GREEN,COLOR_BLACK,1);
            addstr(" (Safe House)");
         }
         if(location[temploc[p]]->renting==-2)
         {
            set_color(COLOR_RED,COLOR_BLACK,1);
            addstr(" (Enemy Safe House)");
         }
         if(location[temploc[p]]->closed)
         {
            set_color(COLOR_RED,COLOR_BLACK,1);
            addstr(" (Closed Down)");
         }
         if(location[temploc[p]]->highsecurity)
         {
            set_color(COLOR_MAGENTA,COLOR_BLACK,1);
            addstr(" (High Security)");
         }
         if(location[temploc[p]]->needcar>0&&!havecar)
         {
            set_color(COLOR_YELLOW,COLOR_BLACK,1);
            addstr(" (Need Car)");
         }
         if(location[temploc[p]]->siege.siege>0)
         {
            set_color(COLOR_RED,COLOR_BLACK,0);
            addstr(" (Under Siege)");
         }
         y++;
      }

      set_color(COLOR_WHITE,COLOR_BLACK,0);
      //PAGE UP
      if(page>0)
      {
         move(10,60);
         addprevpagestr();
      }
      //PAGE DOWN
      if((page+1)*11<temploc.size())
      {
         move(20,60);
         addnextpagestr();
      }

      set_color(COLOR_WHITE,COLOR_BLACK,0);
      move(24,1);
      if(loc!=-1)addstr("Enter - Back one step.");
      else addstr("Enter - The squad is not yet Liberal enough.");

      refresh();

      int c=getch();
      translategetch(c);

      //PAGE UP
      if((c==interface_pgup||c==KEY_UP||c==KEY_LEFT)&&page>0)page--;
      //PAGE DOWN
      if((c==interface_pgdn||c==KEY_DOWN||c==KEY_RIGHT)&&(page+1)*11<temploc.size())page++;

      if(c>='a'&&c<='k')
      {
         int sq=page*11+(int)(c-'a');
         if(sq<temploc.size()&&sq>=0)
         {
            int oldloc=loc;
            loc=temploc[sq];
            temploc.clear();

            for(l=0;l<location.size();l++)
            {
               if(location[l]->parent==loc&&location[l]->renting>=0&&!location[l]->hidden)temploc.push_back(l);
            }
            for(l=0;l<location.size();l++)
            {
               if(location[l]->parent==loc&&location[l]->renting==-2&&!location[l]->hidden)temploc.push_back(l);
            }
            for(l=0;l<location.size();l++)
            {
               if(location[l]->parent==loc&&location[l]->renting==-1&&!location[l]->hidden)temploc.push_back(l);
            }

            if(temploc.size()==0)
            {
               if(!location[loc]->closed&&
                  (location[loc]->needcar==0||havecar))
               {
                  activesquad->activity.type=ACTIVITY_VISIT;
                  activesquad->activity.arg=loc;
                  return;
               }
               else
               {
                  loc=oldloc;
                  for(l=0;l<location.size();l++)
                  {
                     if(location[l]->parent==loc&&location[l]->renting>=0&&!location[l]->hidden)temploc.push_back(l);
                  }
                  for(l=0;l<location.size();l++)
                  {
                     if(location[l]->parent==loc&&location[l]->renting==-2&&!location[l]->hidden)temploc.push_back(l);
                  }
                  for(l=0;l<location.size();l++)
                  {
                     if(location[l]->parent==loc&&location[l]->renting==-1&&!location[l]->hidden)temploc.push_back(l);
                  }
               }
            }
         }
      }

      if(c=='z')
      {
         activesquad->stance++;
         if(activesquad->stance>SQUADSTANCE_STANDARD)
            activesquad->stance=0;
      }

      if(c==10&&loc!=-1)
      {
         loc=location[loc]->parent;
         temploc.clear();
         for(l=0;l<location.size();l++)
         {
            if(location[l]->parent==loc&&location[l]->renting>=0)temploc.push_back(l);
         }
         for(l=0;l<location.size();l++)
         {
            if(location[l]->parent==loc&&location[l]->renting==-1)temploc.push_back(l);
         }
      }
      else if(c==10)
      {
         activesquad->activity.type=ACTIVITY_NONE; // Clear squad activity
         break;
      }

   }while(1);
}



/* base - invest in this location */
void investlocation(void)
{
   int loc=selectedsiege;

   do
   {
      erase();

      locheader();
      printlocation(loc);

      if(!(location[loc]->compound_walls & COMPOUND_BASIC))
      {
         if(funds>=2000)set_color(COLOR_WHITE,COLOR_BLACK,0);
         else set_color(COLOR_BLACK,COLOR_BLACK,1);
         move(8,1);
         addstr("W - Fortify the Compound for a Siege ($2000)");
      }

      if(!(location[loc]->compound_walls & COMPOUND_CAMERAS))
      {
         if(funds>=2000)set_color(COLOR_WHITE,COLOR_BLACK,0);
         else set_color(COLOR_BLACK,COLOR_BLACK,1);
         move(9,1);
         addstr("C - Place Security Cameras around the Compound ($2000)");
      }

      if(!(location[loc]->compound_walls & COMPOUND_TRAPS))
      {
         if(funds>=3000)set_color(COLOR_WHITE,COLOR_BLACK,0);
         else set_color(COLOR_BLACK,COLOR_BLACK,1);
         move(10,1);
         addstr("B - Place Booby Traps throughout the Compound ($3000)");
      }

      if(!(location[loc]->compound_walls & COMPOUND_TANKTRAPS))
      {
         if(funds>=3000)set_color(COLOR_WHITE,COLOR_BLACK,0);
         else set_color(COLOR_BLACK,COLOR_BLACK,1);
         move(11,1);
         addstr("T - Ring the Compound with Tank Traps ($3000)");
      }

      if(!(location[loc]->compound_walls & COMPOUND_GENERATOR))
      {
         if(funds>=3000)set_color(COLOR_WHITE,COLOR_BLACK,0);
         else set_color(COLOR_BLACK,COLOR_BLACK,1);
         move(12,1);
         addstr("G - Buy a Generator for Electricity ($3000)");
      }

      if(!(location[loc]->compound_walls & COMPOUND_PRINTINGPRESS))
      {
         if(funds>=3000)set_color(COLOR_WHITE,COLOR_BLACK,0);
         else set_color(COLOR_BLACK,COLOR_BLACK,1);
         move(13,1);
         addstr("P - Buy a Printing Press ($3000)");
      }

      if(location[loc]->front_business==-1)
      {
         if(funds>=3000)set_color(COLOR_WHITE,COLOR_BLACK,0);
         else set_color(COLOR_BLACK,COLOR_BLACK,1);
         move(14,1);
         addstr("F - Setup a Business Front ($3000)");
      }

      if(funds>=150)set_color(COLOR_WHITE,COLOR_BLACK,0);
      else set_color(COLOR_BLACK,COLOR_BLACK,1);
      move(15,1);
      addstr("R - Buy 20 more daily rations ($150)");

      move(16,1);
      addstr("Enter - Done");

      int c=getch();
      translategetch(c);

      if(c==10)break;

      if(c=='w')
      {
         if(!(location[loc]->compound_walls & COMPOUND_BASIC)&&funds>=2000)
         {
            funds-=2000;
            stat_spent+=2000;
            moneylost_compound+=2000;
            location[loc]->compound_walls|=COMPOUND_BASIC;
         }
      }

      if(c=='c')
      {
         if(!(location[loc]->compound_walls & COMPOUND_CAMERAS)&&funds>=2000)
         {
            funds-=2000;
            stat_spent+=2000;
            moneylost_compound+=2000;
            location[loc]->compound_walls|=COMPOUND_CAMERAS;
         }
      }

      if(c=='b')
      {
         if(!(location[loc]->compound_walls & COMPOUND_TRAPS)&&funds>=3000)
         {
            funds-=3000;
            stat_spent+=3000;
            moneylost_compound+=3000;
            location[loc]->compound_walls|=COMPOUND_TRAPS;
         }
      }

      if(c=='t')
      {
         if(!(location[loc]->compound_walls & COMPOUND_TANKTRAPS)&&funds>=3000)
         {
            funds-=3000;
            stat_spent+=3000;
            moneylost_compound+=3000;
            location[loc]->compound_walls|=COMPOUND_TANKTRAPS;
         }
      }

      if(c=='g')
      {
         if(!(location[loc]->compound_walls & COMPOUND_GENERATOR)&&funds>=3000)
         {
            funds-=3000;
            stat_spent+=3000;
            moneylost_compound+=3000;
            location[loc]->compound_walls|=COMPOUND_GENERATOR;
         }
      }

      if(c=='p')
      {
         if(!(location[loc]->compound_walls & COMPOUND_PRINTINGPRESS)&&funds>=3000)
         {
            funds-=3000;
            stat_spent+=3000;
            moneylost_compound+=3000;
            location[loc]->compound_walls|=COMPOUND_PRINTINGPRESS;
            break;
         }
      }

      if(c=='r')
      {
         if(funds>=150)
         {
            funds-=150;
            stat_spent+=150;
            moneylost_compound+=150;
            location[loc]->compound_stores+=20;
         }
      }

      if(c=='f')
      {
         if(location[loc]->front_business==-1&&funds>=3000)
         {
            funds-=3000;
            stat_spent+=3000;
            moneylost_compound+=3000;
            location[loc]->front_business=LCSrandom(BUSINESSFRONTNUM);
            lastname(location[loc]->front_name);
            strcat(location[loc]->front_name," ");
            switch(location[loc]->front_business)
            {
               case BUSINESSFRONT_INSURANCE:
                  switch(LCSrandom(3))
                  {
                     case 0:strcat(location[loc]->front_name,"Auto");break;
                     case 1:strcat(location[loc]->front_name,"Life");break;
                     case 2:strcat(location[loc]->front_name,"Health");break;
                  }
                  strcat(location[loc]->front_name," Insurance");
                  break;
               case BUSINESSFRONT_TEMPAGENCY:
                  switch(LCSrandom(2))
                  {
                     case 0:strcat(location[loc]->front_name,"Temp Agency");break;
                     case 1:strcat(location[loc]->front_name,"Manpower, LLC");break;
                  }
                  break;
            }
         }
      }
   }while(1);
}



/* base - assign a vehicle to this squad */
void setvehicles(void)
{
   int p, l;
   if(activesquad==NULL)return;

   int page=0;

   do
   {
      erase();

      set_color(COLOR_WHITE,COLOR_BLACK,1);
      move(0,0);
      addstr("Choosing the Right Liberal Vehicle");

      printparty();

      int x=1,y=10;
      char str[200],str2[200];

      for(l=page*18;l<vehicle.size()&&l<page*18+18;l++)
      {
         set_color(COLOR_WHITE,COLOR_BLACK,0);
         for(p=0;p<6;p++)
         {
            if(activesquad->squad[p]==NULL)continue;
            if(activesquad->squad[p]->alive&&
               activesquad->squad[p]->pref_carid==vehicle[l]->id)
            {
               set_color(COLOR_GREEN,COLOR_BLACK,1);
               break;
            }
         }
         for(p=0;p<pool.size();p++)
         {
            if(pool[p]->squadid!=-1&&pool[p]->alive&&
               pool[p]->pref_carid==vehicle[l]->id&&pool[p]->squadid!=activesquad->id)
            {
               set_color(COLOR_YELLOW,COLOR_BLACK,1);
               break;
            }
         }

         getcarfull(str2,*vehicle[l],1);
         str[0]=l-page*18+'A';
         str[1]='\x0';
         strcat(str," - ");
         strcat(str,str2);

         move(y,x);
         addstr(str);

         x+=26;
         if(x>53)
         {
            x=1;
            y++;
         }
      }

      //PAGE UP
      if(page>0)
      {
         move(17,1);
         addprevpagestr();
      }
      //PAGE DOWN
      if((page+1)*18<vehicle.size())
      {
         move(17,53);
         addnextpagestr();
      }

      set_color(COLOR_WHITE,COLOR_BLACK,0);
      move(18,1);
      addstr("Press a letter to specify passengers for that Liberal vehicle");
      move(29,1);
      addstr("Capitalize the letter to designate a driver.");
      move(20,1);
      addstr("Press a number to remove that squad member from a vehicle.");
      move(21,1);
      addstr("Note:  Vehicles in yellow have already been selected by another squad");
      move(22,1);
      addstr("       These cars may be used by both squads but not on the same day.");

      set_color(COLOR_WHITE,COLOR_BLACK,0);
      move(24,1);
      addstr("X - Done");

      refresh();

      int c=getch();
      translategetch_cap(c);

      if(c>='A'&&c<='R')
      {
         int slot=c-'A'+page*18;

         if(slot>=0&&slot<vehicle.size())
         {
            move(8,20);
            set_color(COLOR_WHITE,COLOR_BLACK,1);
            addstr("Choose a Liberal squad member drive it.");

            refresh();

            int c=getch();
            translategetch(c);

            if(c>='1'&&c<='6')
            {
               if(activesquad->squad[c-'1']!=NULL)
               {
                  activesquad->squad[c-'1']->pref_carid=vehicle[slot]->id;
                  if(activesquad->squad[c-'1']->canwalk())
                  {
                     activesquad->squad[c-'1']->pref_is_driver=1;
                  }
                  else activesquad->squad[c-'1']->pref_is_driver=0;
               }
            }
         }
      }

      if(c>='a'&&c<='r')
      {
         int slot=c-'a'+page*18;

         if(slot>=0&&slot<vehicle.size())
         {
            move(8,20);
            set_color(COLOR_WHITE,COLOR_BLACK,1);
            addstr("Choose a Liberal squad member be a passenger.");

            refresh();

            int c=getch();
            translategetch(c);

            if(c>='1'&&c<='6')
            {
               if(activesquad->squad[c-'1']!=NULL)
               {
                  activesquad->squad[c-'1']->pref_carid=vehicle[slot]->id;
                  activesquad->squad[c-'1']->pref_is_driver=0;
               }
            }
         }
      }

      //SAV - adding way to remove people from vehicles.
      
      if(c>='1'&&c<='6')
      {
      // 1. Is there someone there?
        if(activesquad->squad[c-'1']!=NULL)
        {
      // 2. Are they in a vehicle? Someday we'll want to enforce car capacity
          int vin=activesquad->squad[c-'1']->pref_carid;
          if ( vin > 0)
          {
            activesquad->squad[c-'1']->pref_carid=0;
            activesquad->squad[c-'1']->pref_is_driver=0;
          }
        }
      }
      
      //SAV - End add

      if(c=='x')return;

      //PAGE UP
      if((c==interface_pgup||c==KEY_UP||c==KEY_LEFT)&&page>0)page--;
      //PAGE DOWN
      if((c==interface_pgdn||c==KEY_DOWN||c==KEY_RIGHT)&&(page+1)*18<vehicle.size())page++;

   }while(1);
}
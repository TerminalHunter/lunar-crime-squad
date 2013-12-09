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



void advanceday(char &clearformess,char canseethings)
{
   int p;
   showcarprefs=0;
   int w=0;
   int l2;

   //Save the game to autosave.dat each day.
   //autosave();
   //*JDS* Save the game to save.dat each day. :)
   save();

   //CLEAR CAR STATES
   vector<long> caridused;
   for(p=0;p<pool.size();p++)pool[p]->carid=-1;

   //SHUFFLE AROUND THE SQUADLESS
   int homes=-1;
   for(int l=0;l<location.size();l++)
   {
      if(location[l]->type==SITE_RESIDENTIAL_SHELTER)
      {
         homes=l;
         break;
      }
   }
      if (homes==-1)
      {
         //TODO: Error unable to find location
         homes=0;
      }
      
   for(p=0;p<pool.size();p++)
   {
      if(disbanding)break;

      if(!pool[p]->alive)continue;
      if(pool[p]->clinic)continue;
      if(pool[p]->dating)continue;
      if(pool[p]->hiding)continue;
      if(pool[p]->squadid!=-1)continue;
      if(pool[p]->location==-1)continue;

      if(location[pool[p]->location]->type!=SITE_GOVERNMENT_POLICESTATION&&
                  location[pool[p]->location]->type!=SITE_GOVERNMENT_COURTHOUSE&&
                  location[pool[p]->location]->type!=SITE_GOVERNMENT_PRISON)
      {

         if(location[pool[p]->base]->siege.siege)
         {
            pool[p]->base=homes;
         }

         pool[p]->location=pool[p]->base;
      }
   }

   //ADVANCE SQUADS
   squadst *oactivesquad=activesquad;
   for(int sq=0;sq<squad.size();sq++)
   {
      if(disbanding)break;

      //MAKE SURE MEMBERS DON'T ACT IF SQUAD DOES

      if(squad[sq]->activity.type!=ACTIVITY_NONE)
      {
         for(int p=0;p<6;p++)
         {
            if(squad[sq]->squad[p]!=NULL)
            {
               if(squad[sq]->squad[p]->activity.type!=ACTIVITY_NONE)
               {
                  if(clearformess)erase();
                  else
                  {
                     makedelimiter(8,0);
                  }
                  set_color(COLOR_WHITE,COLOR_BLACK,1);
                  move(8,1);
                  addstr(squad[sq]->squad[p]->name);
                  addstr(" acted with ");
                  addstr(squad[sq]->name);
                  addstr(" instead of ");
                  char str[200];
                  getactivity(str,squad[sq]->squad[p]->activity);
                  addstr(str);
                  addstr(".");

                  refresh();
                  getch();

                  squad[sq]->squad[p]->activity.type=ACTIVITY_NONE;
               }
            }
         }
      }

      if(squad[sq]->activity.type==ACTIVITY_VISIT)
      {
         //TURN AWAY SQUADS FROM RECENTLY CLOSED OR SIEGED SITES
         if(location[squad[sq]->activity.arg]->closed||
            location[squad[sq]->activity.arg]->siege.siege)
         {
            if(clearformess)erase();
            else
            {
               makedelimiter(8,0);
            }
            set_color(COLOR_WHITE,COLOR_BLACK,1);
            move(8,1);
            addstr(squad[sq]->name);
            addstr(" decided ");
            addlocationname(location[squad[sq]->activity.arg]);
            addstr(" was too hot to risk.");

            refresh();
            getch();

            //ON TO THE NEXT SQUAD
            squad[sq]->activity.type=ACTIVITY_NONE;
            continue;
         }

         //CAR UP AS NECESSARY
         vector<long> wantcar;

         for(p=0;p<6;p++)
         {
            if(squad[sq]->squad[p]!=NULL)
            {
               long wid=squad[sq]->squad[p]->pref_carid;
               if(wid==-1)continue;
               for(w=0;w<wantcar.size();w++)
               {
                  if(wantcar[w]==wid)break;
               }
               if(w==wantcar.size())wantcar.push_back(wid);
            }
         }

         if(wantcar.size()>0)
         {
            //CULL UNAVAILABLE CARS
            for(int c=wantcar.size()-1;c>=0;c--)
            {
               for(int c2=0;c2<caridused.size();c2++)
               {
                  if(wantcar[c]==caridused[c2])
                  {
                     if(clearformess)erase();
                     else
                     {
                        makedelimiter(8,0);
                     }
                     set_color(COLOR_WHITE,COLOR_BLACK,1);
                     move(8,1);
                     addstr(squad[sq]->name);
                     addstr(" couldn't use the ");
                     long v=id_getcar(caridused[c2]);
                     char str[80];
                     getcarfull(str,*vehicle[v]);
                     addstr(str);
                     addstr(".");

                     refresh();
                     getch();

                     wantcar.erase(wantcar.begin() + c);
                  }
               }
            }
         }

         //ASSIGN AVAILABLE CARS
         if(wantcar.size()>0)
         {
            vector<int> driver;
            vector<int> passenger;
            for(w=0;w<wantcar.size();w++)
            {
               driver.clear();
               passenger.clear();
               caridused.push_back(wantcar[w]);

               //FILL CAR WITH DESIGNATED DRIVERS AND PASSENGERS
               for(p=0;p<6;p++)
               {
                  if(squad[sq]->squad[p]!=NULL)
                  {
                     if(squad[sq]->squad[p]->pref_carid==wantcar[w])
                     {
                        squad[sq]->squad[p]->carid=wantcar[w];
                        squad[sq]->squad[p]->is_driver=squad[sq]->squad[p]->pref_is_driver;
                        if(squad[sq]->squad[p]->is_driver)driver.push_back(p);
                        else passenger.push_back(p);
                     }
                  }
               }

               //NO DRIVER?
               if(driver.size()==0)
               {
                  //MAKE BEST DRIVING PASSENGER INTO A DRIVER
                  if(passenger.size()>0)
                  {
                     int max=0;
                     for(p=0;p<passenger.size();p++)
                     {
                        long v=id_getcar(squad[sq]->squad[passenger[p]]->carid);
                        if(v >= 0) 
                        {
                              if(driveskill(*squad[sq]->squad[passenger[p]],vehicle[v])>max&&
                                 squad[sq]->squad[passenger[p]]->canwalk())
                              {
                                    max=driveskill(*squad[sq]->squad[passenger[p]],vehicle[v]);
                           }
                        }
                     }
                     vector<int> goodp;
                     for(p=0;p<passenger.size();p++)
                     {
                        long v=id_getcar(squad[sq]->squad[passenger[p]]->carid);
                        if(v >= 0) 
                        {
                              if(driveskill(*squad[sq]->squad[passenger[p]],vehicle[v])==max&&
                              squad[sq]->squad[passenger[p]]->canwalk())
                           {
                              goodp.push_back(passenger[p]);
                           }
                        }
                     }

                     if(goodp.size()>0)
                     {
                        int p=goodp[LCSrandom(goodp.size())];
                        squad[sq]->squad[p]->is_driver=1;
                     }
                  }
               }
               //TOO MANY DRIVERS?
               else if(driver.size()>1)
               {
                  //TOSS ALL BUT THE BEST
                  int max=0;
                  for(p=0;p<driver.size();p++)
                  {
                     long v=id_getcar(squad[sq]->squad[driver[p]]->carid);
                     if (v >= 0)
                     {
                        if(driveskill(*squad[sq]->squad[driver[p]],vehicle[v])>max)
                     {
                           max=driveskill(*squad[sq]->squad[driver[p]],vehicle[v]);
                     }
                     }
                  }
                  vector<int> goodp;
                  for(p=0;p<driver.size();p++)
                  {
                     long v=id_getcar(squad[sq]->squad[driver[p]]->carid);
                     if (v >= 0)
                     {
                        if(driveskill(*squad[sq]->squad[driver[p]],vehicle[v])==max)
                        {
                           goodp.push_back(p);
                        }
                     }
                  }

                  if(goodp.size()>0)
                  {
                     int p=goodp[LCSrandom(goodp.size())];
                     for(int p2=0;p2<driver.size();p2++)
                     {
                        if(p2==p)continue;
                        squad[sq]->squad[driver[p2]]->is_driver=0;
                     }
                  }
               }
            }

            //PUT PEOPLE WITHOUT CARS INTO RANDOM CARS
               //THESE PEOPLE WILL NOT DRIVE
            for(int p=0;p<6;p++)
            {
               if(squad[sq]->squad[p]!=NULL)
               {
                  if(squad[sq]->squad[p]->carid==-1)
                  {
                     squad[sq]->squad[p]->carid=wantcar[LCSrandom(wantcar.size())];
                     squad[sq]->squad[p]->is_driver=0;
                  }
               }
            }
         }

         //IF NEED CAR AND DON'T HAVE ONE...
            //NOTE: SQUADS DON'T TAKE FREE CARS
         if(location[squad[sq]->activity.arg]->needcar&&
            squad[sq]->squad[0]!=NULL)
         {
            if(squad[sq]->squad[0]->carid==-1)
            {
               if(clearformess)erase();
               else
               {
                  makedelimiter(8,0);
               }
               set_color(COLOR_WHITE,COLOR_BLACK,1);
               move(8,1);
               addstr(squad[sq]->name);
               addstr(" didn't have a car to get to ");
               addlocationname(location[squad[sq]->activity.arg]);
               addstr(".");

               refresh();
               getch();

               //ON TO THE NEXT SQUAD
               squad[sq]->activity.type=ACTIVITY_NONE;
               continue;
            }
         }

         //GO PLACES
         switch(location[squad[sq]->activity.arg]->type)
         {
            case SITE_BUSINESS_DEPTSTORE:
            case SITE_BUSINESS_HALLOWEEN:
            case SITE_BUSINESS_PAWNSHOP:
               if(clearformess)erase();
               else
               {
                  makedelimiter(8,0);
               }
               set_color(COLOR_WHITE,COLOR_BLACK,1);
               move(8,1);
               addstr(squad[sq]->name);
               addstr(" has arrived at ");
               addlocationname(location[squad[sq]->activity.arg]);
               addstr(".");

               refresh();
               getch();

               activesquad=squad[sq];
               showcarprefs=-1;
               switch(location[squad[sq]->activity.arg]->type)
               {
                  case SITE_BUSINESS_DEPTSTORE:
                     deptstore(squad[sq]->activity.arg);
                     break;
                  case SITE_BUSINESS_HALLOWEEN:
                     halloweenstore(squad[sq]->activity.arg);
                     break;
                  case SITE_BUSINESS_PAWNSHOP:
                     pawnshop(squad[sq]->activity.arg);
                     break;
               }
               showcarprefs=0;
               if(activesquad->squad[0]!=NULL)
               {
                  locatesquad(activesquad,activesquad->squad[0]->base);
               }
               clearformess=1;
               break;
            case SITE_HOSPITAL_UNIVERSITY:
            case SITE_HOSPITAL_CLINIC:
               if(clearformess)erase();
               else
               {
                  makedelimiter(8,0);
               }
               set_color(COLOR_WHITE,COLOR_BLACK,1);
               move(8,1);
               addstr(squad[sq]->name);
               addstr(" has arrived at ");
               addlocationname(location[squad[sq]->activity.arg]);
               addstr(".");

               refresh();
               getch();

               activesquad=squad[sq];
               hospital(squad[sq]->activity.arg);
               if(activesquad->squad[0]!=NULL)
               {
                  locatesquad(activesquad,activesquad->squad[0]->base);
               }
               clearformess=1;
               break;
            default:
            {
               if(clearformess)erase();
               else
               {
                  makedelimiter(8,0);
               }

               set_color(COLOR_WHITE,COLOR_BLACK,1);
               move(8,1);
               if(squad[sq]->squad[0]->base==squad[sq]->activity.arg)
               {
                  addstr(squad[sq]->name);
                  addstr(" looks around ");
                  addlocationname(location[squad[sq]->activity.arg]);
                  addstr(".");
               }
               else
               {
                  addstr(squad[sq]->name);
                  addstr(" has arrived at ");
                  addlocationname(location[squad[sq]->activity.arg]);
                  addstr(".");
               }

               refresh();
               getch();

               if(clearformess)erase();
               else
               {
                  makedelimiter(8,0);
               }

               int c='t';

               if(location[squad[sq]->activity.arg]->renting>=0&&
                  location[squad[sq]->activity.arg]->type==SITE_INDUSTRY_WAREHOUSE)
               {
                  c='s';
               }
               else if(location[squad[sq]->activity.arg]->renting>=0&&
                  squad[sq]->squad[0]->base!=squad[sq]->activity.arg)
               {
                  set_color(COLOR_WHITE,COLOR_BLACK,1);
                  move(8,1);
                  addstr("Why is the squad here?   (S)afe House, to cause (T)rouble, or (B)oth?");

                  c=getch();
                  translategetch(c);
               }

               if(c=='s'||c=='b')basesquad(squad[sq],squad[sq]->activity.arg);
               if(c=='t'||c=='b')
               {
                  activesquad=squad[sq];
                  newsstoryst *ns=new newsstoryst;
                     ns->type=NEWSSTORY_SQUAD_SITE;
                     ns->positive=1;
                     ns->loc=squad[sq]->activity.arg;
                  newsstory.push_back(ns);
                  mode_site(squad[sq]->activity.arg);
               }
               if(squad[sq]->squad[0]!=NULL)
               {
                  locatesquad(squad[sq],squad[sq]->squad[0]->base);
               }

               clearformess=1;
               break;
            }
         }
         squad[sq]->activity.type=ACTIVITY_NONE;
      }
   }
   activesquad=oactivesquad;

   //HOSTAGES
   for(p=pool.size()-1;p>=0;p--)
   {
      if(disbanding)break;

      if(!pool[p]->alive)continue;
      if(pool[p]->align!=1)
      {
         tendhostage(pool[p],clearformess);
      }
   }

   //ACTIVITIES FOR INDIVIDUALS
   for(p=0;p<pool.size();p++)
   {
      if(disbanding)break;

      if(!pool[p]->alive)continue;
      if(pool[p]->clinic)continue;
      if(pool[p]->dating)continue;
      if(pool[p]->hiding)continue;

      //CLEAR ACTIONS FOR PEOPLE UNDER SIEGE
      if(location[pool[p]->location]->siege.siege)
      {
         pool[p]->activity.type=ACTIVITY_NONE;
      }
      switch(pool[p]->activity.type)
      {
         case ACTIVITY_REPAIR_ARMOR:
            repairarmor(*pool[p],clearformess);
            break;
         case ACTIVITY_MAKE_ARMOR:
            makearmor(*pool[p],clearformess);
            pool[p]->activity.type=ACTIVITY_NONE;
            break;
         case ACTIVITY_WHEELCHAIR:
            getwheelchair(*pool[p],clearformess);
            if(pool[p]->flag & CREATUREFLAG_WHEELCHAIR)pool[p]->activity.type=ACTIVITY_NONE;
            break;
         case ACTIVITY_STEALCARS:
            if(stealcar(*pool[p],clearformess))
            {
               pool[p]->activity.type=ACTIVITY_NONE;
            }
            break;
         case ACTIVITY_POLLS:
            if(clearformess)erase();
            else
            {
               makedelimiter(8,0);
            }

            set_color(COLOR_WHITE,COLOR_BLACK,1);
            move(8,1);
            addstr(pool[p]->name);
            addstr(" surfs the Net for recent opinion polls.");

            refresh();
            getch();

            pool[p]->skill_ip[SKILL_COMPUTERS]++;

            survey(pool[p]);
            pool[p]->activity.type=ACTIVITY_NONE;

            clearformess=1;
            break;
      }
   }

   funds_and_trouble(clearformess);

   //DISPERSAL CHECK
   dispersalcheck(clearformess);

   //DO RENT
   if(day==3&&!disbanding)
   {
      for(int l=0;l<location.size();l++)
      {
         if(location[l]->renting>0&&
            !location[l]->newrental)
         {
            if(funds>=location[l]->renting)
            {
               funds-=location[l]->renting;
               moneylost_rent+=location[l]->renting;
               stat_spent+=location[l]->renting;
            }
            //EVICTED!!!!!!!!!
            else
            {
               if(clearformess)erase();
               else
               {
                  makedelimiter(8,0);
               }

               set_color(COLOR_WHITE,COLOR_BLACK,1);
               move(8,1);
               addstr("EVICTION NOTICE: ");
               addstr(location[l]->name);
               addstr(".  Possessions go to the shelter.");

               refresh();
               getch();

               location[l]->renting=-1;

               //MOVE ALL ITEMS AND SQUAD MEMBERS
               int hs=0;
               for(l2=0;l2<location.size();l2++)
               {
                  if(location[l2]->type==SITE_RESIDENTIAL_SHELTER)
                  {
                     hs=l2;
                     break;
                  }
               }
               for(int p=0;p<pool.size();p++)
               {
                  if(pool[p]->location==l)pool[p]->location=hs;
                  if(pool[p]->base==l)pool[p]->base=hs;
               }
               for(l2=0;l2<location[l]->loot.size();l2++)
               {
                  location[hs]->loot.push_back(location[l]->loot[l2]);
               }
               location[l]->loot.clear();

               location[l]->compound_walls=0;
               location[l]->compound_stores=0;
               location[l]->front_business=-1;
            }
         }
      }
   }

   //DO DATES
   for(int d=date.size()-1;d>=0;d--)
   {
      if(disbanding)break;

      int p=getpoolcreature(date[d]->mac_id);
      // Stand up dates if 1) dater does not exist, or 2) dater was not able to return to a safehous today (and is not in the hospital)
      if(p!=-1&&pool[p]->location!=-1&&(location[pool[p]->location]->renting!=-1||
         location[pool[p]->location]->type==SITE_HOSPITAL_CLINIC||
         location[pool[p]->location]->type==SITE_HOSPITAL_UNIVERSITY)||
         date[d]->timeleft)
      {
         //VACATION
         if(date[d]->timeleft>0)
         {
            date[d]->timeleft--;

            pool[p]->dating=date[d]->timeleft;

            if(date[d]->timeleft==0)
            {
               int hs=-1;
               for(int l=0;l<location.size();l++)
               {
                  if(location[l]->type==SITE_RESIDENTIAL_SHELTER)
                  {
                     hs=l;
                     break;
                  }
               }
               if (hs==-1)
               {
                  //TODO: Error unable to find location
                  hs=0;
               }

               if(location[pool[p]->base]->siege.siege)
               {
                  pool[p]->base=hs;
               }

               pool[p]->location=pool[p]->base;

               if(completevacation(*date[d],p,clearformess))
               {
                  delete date[d];
                  date.erase(date.begin() + d);
                  continue;
               }
            }
         }
         //DO A DATE
         else
         {
            //TERMINATE NULL DATES
            if(location[pool[p]->location]->siege.siege)
            {
               delete date[d];
               date.erase(date.begin() + d);
               continue;
            }
            //DO DATE
            else if(completedate(*date[d],p,clearformess))
            {
               delete date[d];
               date.erase(date.begin() + d);
               continue;
            }
            else
            {
               pool[p]->dating=date[d]->timeleft;
               if(pool[p]->dating>0)
               {
                  long sq=-1;
                  
                  //IF YOU ARE THE LAST PERSON IN YOUR SQUAD
                  //YOU HAVE TO DROP OFF THE EQUIPMENT WHEREVER YOUR BASE IS
                  //BECAUSE YOUR SQUAD IS ABOUT TO BE DESTROYED
                  if(pool[p]->squadid!=-1)
                     sq=getsquad(pool[p]->squadid);

                  //NOW KICK THE DATER OUT OF THE SQUAD AND LOCATION
                  removesquadinfo(*pool[p]);
                  pool[p]->location=-1;

                  if(sq!=-1)
                  {
                     testsquadclear(*squad[sq], pool[p]->base);
                  }
               }
            }
         }
      }
      else
      {
         delete date[d];
         date.erase(date.begin() + d);
         continue;
      }
   }

   //AGE THINGS
   day++;
   for(p=0;p<pool.size();p++)
   {
      if(pool[p]->hiding>0)
      {
         pool[p]->hiding--;
         if(pool[p]->hiding==0)
         {
            pool[p]->location=pool[p]->base;

            if(clearformess)erase();
            else
            {
               makedelimiter(8,0);
            }

            set_color(COLOR_WHITE,COLOR_BLACK,1);
            move(8,1);
            addstr(pool[p]->name);
            addstr(" slinks back to the homeless shelter.");

            refresh();
            getch();
         }
      }

      if((pool[p]->flag & CREATUREFLAG_MISSING)&&
         !(pool[p]->flag & CREATUREFLAG_KIDNAPPED))
      {
         if(LCSrandom(10)+2<pool[p]->joindays)
         {
            pool[p]->flag|=CREATUREFLAG_KIDNAPPED;

            newsstoryst *ns=new newsstoryst;
               ns->type=NEWSSTORY_KIDNAPREPORT;
               ns->cr=pool[p];
            newsstory.push_back(ns);
         }
      }

      pool[p]->joindays++;

      if(!pool[p]->alive)
      {
         pool[p]->deathdays++;
         continue;
      }

      for(int s=0;s<SKILLNUM;s++)
      {
         while(pool[p]->skill_ip[s]>=100*((10+pool[p]->skill[s])/10)&&
               pool[p]->skill[s]<pool[p]->attval(skillatt(s))*2)
         {
            pool[p]->skill_ip[s]-=100*((10+pool[p]->skill[s])/10);
            pool[p]->skill[s]+=1;
         }
      }
   }
   
   //DO REPORTING BY MAJOR NEWSPAPERS
   majornewspaper(clearformess,canseethings);

   //CLEAN UP GONE SQUADS
   cleangonesquads();

   //SIEGE?
   siegeturn(clearformess);
   siegecheck(canseethings);

   //CLEAN GONE SQUADS AGAIN
   cleangonesquads();

   showcarprefs=1;
}



/* squad members with no chain of command lose contact */
void dispersalcheck(char &clearformess)
{
int p = 0;
   //NUKE DISPERSED SQUAD MEMBERS WHOSE MASTERS ARE NOT AVAILABLE
   if(pool.size()>0)
   {
      // *JDS* I'm documenting this algorithm carefully because it
      // took me awhile to figure out what exactly was going on here.
      //
      // nukeme tracks whether each person has a secure chain of command.
      // 
      // if nukeme == 1, no confirmation of contact has been made
      // if nukeme == 0, confirmation that THEY are safe is given,
      //    but it is still needed to check whether their subordinates
      //    can reach them.
      // if nukeme == -1, confirmation has been made that this squad
      //    member is safe, and their immediate subordinates have also
      //    checked.
      //
      // The way the algorithm works, everyone starts at nukeme = 1.
      // Then we start at the top of the chain of command and walk
      // down it slowly, marking people 0 and then -1 as we sweep
      // down the chain. If someone is dead or in an unreachable state,
      // they block progression down the chain to their subordinates,
      // preventing everyone who requires contact with that person
      // from being marked safe. After everyone reachable has been
      // reached and marked safe, all remaining squad members are nuked.
      vector<int> nukeme;
      nukeme.resize(pool.size());

      for(p=pool.size()-1;p>=0;p--)
      {
         // If member is dead or has no boss (founder level), mark
         // them nukeme = 0, using them as a starting point at the top
         // of the chain.
         if(!pool[p]->alive||pool[p]->hireid==-1)nukeme[p]=0;
         // All remaining members are marked nukeme = 1.
         else nukeme[p]=1;
      }

      char changed;

      do // while(changed!=0)
      {
         changed=0;

         char inprison, alive;

         // Go through the entire pool to locate people at nukeme = 0,
         // so we can verify that their subordinates can reach them.
         for(p=pool.size()-1;p>=0;p--)
         {
            if(pool[p]->location!=-1&&location[pool[p]->location]->type==SITE_GOVERNMENT_PRISON)
               inprison=1;
            else inprison=0;

            if(pool[p]->alive)alive=1;
            else alive=0;

            // If in prison or unreachable due to a member of the command structure
            // above being in prison
            if(nukeme[p]==0&&alive&&inprison||nukeme[p]==2)
            {
               // If you're here because you're unreachable, mark as checked and unreachable
               if(!inprison)
               {
                  // Roll to see if you go into hiding or not
                  if(!pool[p]->hiding&&
                     pool[p]->attval(ATTRIBUTE_HEART)*5+pool[p]->juice<LCSrandom(200))
                  {
                     nukeme[p]=1;
                  }
                  else nukeme[p]=3;
               }
               else nukeme[p]=-1; // Else you're in prison; you're guaranteed contactable
               
               // Find all subordinates if you didn't lose contact completely
               if(nukeme[p]!=1)
               {
                  for(int p2=pool.size()-1;p2>=0;p2--)
                  {
                     if(pool[p2]->hireid==pool[p]->id)
                     {
                        nukeme[p2]=2; // Mark them as unreachable
                        changed=1; // Need another iteration
                     }
                  }
               }
            }
            // Otherwise, if they're both alive and reachable
            else if(nukeme[p]==0&&alive&&!inprison)
            {
               // Start looking through the pool again.
               for(int p2=pool.size()-1;p2>=0;p2--)
               {
                  // Locate each of this person's subordinates.
                  if(pool[p2]->hireid==pool[p]->id)
                  {
                     // Protect them from being dispersed -- their boss is
                     // safe. Their own subordinates will then be considered
                     // in the next loop iteration.
                     nukeme[p2]=0;
                     // If they're hiding indefinately and their boss isn't
                     // hiding at all, then have them discreetly return in a
                     // couple of weeks
                     if(pool[p2]->hiding==-1&&!pool[p]->hiding)
                     {
                        pool[p2]->hiding=LCSrandom(10)+3;
                     }
                     changed=1; // Take note that another iteration is needed.
                  }
               }
               // Now that we've dealt with this person's subordinates, mark
               // them so that we don't look at them again in this loop.
               nukeme[p]=-1;
            }
         }
      }while(changed); // If another iteration is needed, continue the loop.

      // After checking through the entire command structure, proceed
      // to nuke all squad members who are unable to make contact with
      // the LCS.
      for(p=pool.size()-1;p>=0;p--)
      {
         if(nukeme[p]==1||nukeme[p]==3)
         {
            if(clearformess)
            {
               erase();
            }
            else
            {
               makedelimiter(8,0);
            }

            if(!pool[p]->hiding&&nukeme[p]==3)
            {
               set_color(COLOR_WHITE,COLOR_BLACK,1);
               move(8,1);
               addstr(pool[p]->name);
               addstr(" has lost touch with the Liberal Crime Squad.");
               move(9,1);
               addstr("The Liberal has gone into hiding...");
               refresh();
               getch();
            }
            else if(nukeme[p]==1)
            {
               set_color(COLOR_WHITE,COLOR_BLACK,1);
               move(8,1);
               addstr(pool[p]->name);
               addstr(" has lost touch with the Liberal Crime Squad.");
               refresh();
               getch();
            }


            removesquadinfo(*pool[p]);
            if(nukeme[p]==1)
            {
               delete pool[p];
               pool.erase(pool.begin() + p);
            }
            else
            {
               int hs=0;
               for(int l=0;l<location.size();l++)
               {
                  if(location[l]->type==SITE_RESIDENTIAL_SHELTER)
                  {
                     hs=l;
                     break;
                  }
               }
               pool[p]->location=-1;
               pool[p]->base=hs;
               pool[p]->activity.type=ACTIVITY_NONE;
               pool[p]->hiding=-1; // Hide indefinately
            }
         }
      }
   }

   //MUST DO AN END OF GAME CHECK HERE BECAUSE OF DISPERSAL
   endcheck(END_DISPERSED);

   cleangonesquads();
}



/* daily - manages too hot timer and when a site map should be re-seeded and renamed */
void advancelocations(void)
{
   //ADVANCE LOCATIONS
   for(int l=0;l<location.size();l++)
   {
      if(location[l]->closed>0)
      {
         location[l]->closed--;
         if(location[l]->closed==0)
         {
            switch(LCSrandom(2))
            {
               case 0:
                  if(securityable(location[l]->type))location[l]->highsecurity=1;
                  else initlocation(*location[l]);
                  break;
               case 1:
                  initlocation(*location[l]);
                  break;
            }
         }
      }
   }
}



/* daily - returns true if the site type supports high security */
char securityable(int type)
{
   switch(type)
   {
      case SITE_BUSINESS_CIGARBAR:
      case SITE_RESIDENTIAL_APARTMENT_UPSCALE:
      case SITE_LABORATORY_COSMETICS:
      case SITE_INDUSTRY_NUCLEAR:
      case SITE_LABORATORY_GENETIC:
      case SITE_GOVERNMENT_POLICESTATION:
      case SITE_GOVERNMENT_COURTHOUSE:
      case SITE_GOVERNMENT_PRISON:
      case SITE_GOVERNMENT_INTELLIGENCEHQ:
      case SITE_INDUSTRY_SWEATSHOP:
      case SITE_INDUSTRY_POLLUTER:
      case SITE_CORPORATE_HEADQUARTERS:
      case SITE_CORPORATE_HOUSE:
      case SITE_MEDIA_AMRADIO:
      case SITE_MEDIA_CABLENEWS:
         return 1;
   }

   return 0;
}



/* daily - seeds and names a site (will re-seed and rename if used after start) */
void initlocation(locationst &loc)
{
   loc.init();

   char str[80];

   switch(loc.type)
   {
      case SITE_GOVERNMENT_POLICESTATION:
         strcpy(loc.name,"Police Station");
         strcpy(loc.shortname,"Police Station");
         break;
      case SITE_GOVERNMENT_COURTHOUSE:
         if(law[LAW_DEATHPENALTY]==-2)
         {
            strcpy(loc.name,"Halls of Ultimate Judgement");
            strcpy(loc.shortname,"Judge Hall");
         }
         else
         {
            strcpy(loc.name,"Court House");
            strcpy(loc.shortname,"Court House");
         }
         break;
      case SITE_GOVERNMENT_PRISON:
         if(law[LAW_DEATHPENALTY]==-2&&
            law[LAW_POLICEBEHAVIOR]==-2)
         {
            switch(LCSrandom(5))
            {
               case 0:strcpy(loc.name,"Happy");break;
               case 1:strcpy(loc.name,"Cheery");break;
               case 2:strcpy(loc.name,"Quiet");break;
               case 3:strcpy(loc.name,"Green");break;
               case 4:strcpy(loc.name,"Nectar");break;
            }
            strcat(loc.name," ");
            switch(LCSrandom(5))
            {
               case 0:strcat(loc.name,"Valley");break;
               case 1:strcat(loc.name,"Meadow");break;
               case 2:strcat(loc.name,"Hills");break;
               case 3:strcat(loc.name,"Glade");break;
               case 4:strcat(loc.name,"Forest");break;
            }
            strcat(loc.name," Re-education Camp");
            strcpy(loc.shortname,"Re-ed Camp");
         }
         else
         {
            lastname(str);
            strcpy(loc.name,str);
            strcat(loc.name," Prison");
            strcpy(loc.shortname,"Prison");
         }
         break;
      case SITE_INDUSTRY_NUCLEAR:
         strcpy(loc.name,"Nuclear Power Plant");
         strcpy(loc.shortname,"NPower Plant");
         break;
      case SITE_GOVERNMENT_INTELLIGENCEHQ:
         if(law[LAW_PRIVACY]==-2&&
            law[LAW_POLICEBEHAVIOR]==-2)
         {
            strcpy(loc.name,"Ministry of Love");
            strcpy(loc.shortname,"Min. Love");
         }
         else
         {
            strcpy(loc.name,"Intelligence HQ");
            strcpy(loc.shortname,"Int. HQ");
         }
         break;
      case SITE_CORPORATE_HEADQUARTERS:
         strcpy(loc.name,"Corporate HQ");
         strcpy(loc.shortname,"Corp. HQ");
         break;
      case SITE_CORPORATE_HOUSE:
         if(law[LAW_CORPORATE]==-2&&
				law[LAW_TAX]==-2)
         {
            strcpy(loc.name,"CEO Castle");
            strcpy(loc.shortname,"CEO Castle");
         }
         else
         {
            strcpy(loc.name,"CEO Residence");
            strcpy(loc.shortname,"CEO House");
         }
         break;
      case SITE_RESIDENTIAL_SHELTER:
         strcpy(loc.name,"Homeless Shelter");
         strcpy(loc.shortname,"Shelter");
         break;
      case SITE_INDUSTRY_WAREHOUSE:
      {
         strcpy(loc.name,"Abandoned ");
         char str[50];
         lastname(str);
         strcat(loc.name,str);
         strcat(loc.name," ");

         switch(LCSrandom(10))
         {
            case 0:
               strcat(loc.name,"Meat Plant");
               strcpy(loc.shortname,"Meat Plant");
               break;
            case 1:
               strcat(loc.name,"Warehouse");
               strcpy(loc.shortname,"Warehouse");
               break;
            case 2:
               strcat(loc.name,"Paper Mill");
               strcpy(loc.shortname,"Paper Mill");
               break;
            case 3:
               strcat(loc.name,"Cement Factory");
               strcpy(loc.shortname,"Cement");
               break;
            case 4:
               strcat(loc.name,"Fertilizer Plant");
               strcpy(loc.shortname,"Fertilizer");
               break;
            case 5:
               strcat(loc.name,"Drill Factory");
               strcpy(loc.shortname,"Drill");
               break;
            case 6:
               strcat(loc.name,"Steel Plant");
               strcpy(loc.shortname,"Steel");
               break;
            case 7:
               strcat(loc.name,"Tire Factory");
               strcpy(loc.shortname,"Tire");
               break;
            case 8:
               strcat(loc.name,"Toy Factory");
               strcpy(loc.shortname,"Toy");
               break;
            case 9:
               strcat(loc.name,"Building Site");
               strcpy(loc.shortname,"Building");
               break;
         }
         break;
      }
      case SITE_INDUSTRY_POLLUTER:
         switch(LCSrandom(5))
         {
            case 0:
               strcpy(loc.name,"Aluminum Factory");
               strcpy(loc.shortname,"Alum Fact");
               break;
            case 1:
               strcpy(loc.name,"Plastic Factory");
               strcpy(loc.shortname,"Plast Fact");
               break;
            case 2:
               strcpy(loc.name,"Oil Refinery");
               strcpy(loc.shortname,"Refinery");
               break;
            case 3:
               strcpy(loc.name,"Auto Plant");
               strcpy(loc.shortname,"Auto Plant");
               break;
            case 4:
               strcpy(loc.name,"Chemical Factory");
               strcpy(loc.shortname,"Chem Fact");
               break;
         }
         break;
      case SITE_MEDIA_CABLENEWS:
         strcpy(loc.name,"Cable News Station");
         strcpy(loc.shortname,"News Station");
         break;
      case SITE_MEDIA_AMRADIO:
         strcpy(loc.name,"AM Radio Station");
         strcpy(loc.shortname,"Radio Station");
         break;
      case SITE_RESIDENTIAL_APARTMENT_UPSCALE:
         lastname(str);
         strcpy(loc.name,str);
         strcat(loc.name," Apartments");
         strcpy(loc.shortname,str);
         strcat(loc.shortname," Apts");
         break;
      case SITE_RESIDENTIAL_APARTMENT:
         lastname(str);
         strcpy(loc.name,str);
         strcat(loc.name," Apartments");
         strcpy(loc.shortname,str);
         strcat(loc.shortname," Apts");
         break;
      case SITE_RESIDENTIAL_TENEMENT:
         lastname(str);
         strcpy(loc.name,str);
         strcat(loc.name," Apartments");
         strcpy(loc.shortname,str);
         strcat(loc.shortname," Apts");
         break;
      case SITE_HOSPITAL_UNIVERSITY:
         strcpy(loc.name,"The University Hospital");
         strcpy(loc.shortname,"U Hospital");
         break;
      case SITE_HOSPITAL_CLINIC:
         strcpy(loc.name,"The Free CLINIC");
         strcpy(loc.shortname,"CLINIC");
         break;
      case SITE_LABORATORY_GENETIC:
         lastname(str);
         strcpy(loc.name,str);
         strcat(loc.name," Genetics");
         strcpy(loc.shortname,"Genetics Lab");
         break;
      case SITE_LABORATORY_COSMETICS:
         lastname(str);
         strcpy(loc.name,str);
         strcat(loc.name," Cosmetics");
         strcpy(loc.shortname,"Cosmetics Lab");
         break;
      case SITE_BUSINESS_PAWNSHOP:
         lastname(str);
         strcpy(loc.name,str);
         strcat(loc.name," Cash & Loans");
         strcpy(loc.shortname,"Pawnshop");
         break;
      case SITE_BUSINESS_DEPTSTORE:
         lastname(str);
         strcpy(loc.name,str);
         strcat(loc.name,"'s Department Store");
         strcpy(loc.shortname,"Dept. Store");
         break;
      case SITE_BUSINESS_HALLOWEEN:
         lastname(str);
         strcpy(loc.name,"The Oubliette");
         strcpy(loc.shortname,"Oubliette");
         break;
      case SITE_INDUSTRY_SWEATSHOP:
         lastname(str);
         strcpy(loc.name,str);
         strcat(loc.name," Garment Makers");
         strcpy(loc.shortname,"Sweatshop");
         break;
      case SITE_BUSINESS_CRACKHOUSE:
         lastname(str);
         strcpy(loc.name,str);
         strcat(loc.name," St. Crack House");
         strcpy(loc.shortname,"Crack House");
         break;
      case SITE_BUSINESS_JUICEBAR:
         strcpy(loc.name,"");
         switch(LCSrandom(5))
         {
            case 0:strcat(loc.name,"Natural");break;
            case 1:strcat(loc.name,"Harmonious");break;
            case 2:strcat(loc.name,"Restful");break;
            case 3:strcat(loc.name,"Healthy");break;
            case 4:strcat(loc.name,"New You");break;
         }
         strcat(loc.name," ");
         switch(LCSrandom(5))
         {
            case 0:strcat(loc.name,"Diet");break;
            case 1:strcat(loc.name,"Methods");break;
            case 2:strcat(loc.name,"Plan");break;
            case 3:strcat(loc.name,"Orange");break;
            case 4:strcat(loc.name,"Carrot");break;
         }
         strcat(loc.name," Juice Bar");
         strcpy(loc.shortname,"Juice Bar");
         break;
      case SITE_BUSINESS_VEGANCOOP:
         strcpy(loc.name,"");
         switch(LCSrandom(5))
         {
            case 0:strcat(loc.name,"Asparagus");break;
            case 1:strcat(loc.name,"Tofu");break;
            case 2:strcat(loc.name,"Broccoli");break;
            case 3:strcat(loc.name,"Radish");break;
            case 4:strcat(loc.name,"Eggplant");break;
         }
         strcat(loc.name," ");
         switch(LCSrandom(5))
         {
            case 0:strcat(loc.name,"Forest");break;
            case 1:strcat(loc.name,"Rainbow");break;
            case 2:strcat(loc.name,"Garden");break;
            case 3:strcat(loc.name,"Farm");break;
            case 4:strcat(loc.name,"Meadow");break;
         }
         strcat(loc.name," Vegan Co-op");
         strcpy(loc.shortname,"Vegan Co-op");
         break;
      case SITE_BUSINESS_INTERNETCAFE:
         strcpy(loc.name,"");
         switch(LCSrandom(5))
         {
            case 0:strcat(loc.name,"Electric");break;
            case 1:strcat(loc.name,"Wired");break;
            case 2:strcat(loc.name,"Nano");break;
            case 3:strcat(loc.name,"Micro");break;
            case 4:strcat(loc.name,"Techno");break;
         }
         strcat(loc.name," ");
         switch(LCSrandom(5))
         {
            case 0:strcat(loc.name,"Panda");break;
            case 1:strcat(loc.name,"Troll");break;
            case 2:strcat(loc.name,"Latte");break;
            case 3:strcat(loc.name,"Unicorn");break;
            case 4:strcat(loc.name,"Pixie");break;
         }
         strcat(loc.name," Internet Cafe");
         strcpy(loc.shortname,"Net Cafe");
         break;
      case SITE_BUSINESS_CIGARBAR:
         lastname(str);
         strcpy(loc.name,"The ");
         strcat(loc.name,str);
         strcat(loc.name," Gentlemen's Club");
         strcpy(loc.shortname,"Cigar Bar");
         break;
      case SITE_BUSINESS_LATTESTAND:
         strcpy(loc.name,"");
         switch(LCSrandom(5))
         {
            case 0:strcat(loc.name,"Frothy");break;
            case 1:strcat(loc.name,"Milky");break;
            case 2:strcat(loc.name,"Caffeine");break;
            case 3:strcat(loc.name,"Morning");break;
            case 4:strcat(loc.name,"Evening");break;
         }
         strcat(loc.name," ");
         switch(LCSrandom(5))
         {
            case 0:strcat(loc.name,"Mug");break;
            case 1:strcat(loc.name,"Cup");break;
            case 2:strcat(loc.name,"Jolt");break;
            case 3:strcat(loc.name,"Wonder");break;
            case 4:strcat(loc.name,"Express");break;
         }
         strcat(loc.name," Latte Stand");
         strcpy(loc.shortname,"Latte Stand");
         break;
   }
}



/* daily - returns the number of days in the current month */
int monthday(void)
{
   switch(month)
   {
      case 4:return 30;
      case 6:return 30;
      case 9:return 30;
      case 11:return 30;
      case 2:
         if(year%4==0)return 29;
         else return 28;
      default:
         return 31;
   }
}
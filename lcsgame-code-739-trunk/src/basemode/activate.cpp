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

vector<Creature *> activatable_liberals()
{
   vector<Creature *> temppool;
   int sq;
   for(int p=0;p<(int)pool.size();p++)
   {
      if(pool[p]->alive&&
         pool[p]->align==1&&
         pool[p]->clinic==0&&
         pool[p]->dating==0&&
         pool[p]->hiding==0&&
         !(pool[p]->flag & CREATUREFLAG_SLEEPER))
      {
         if(!location[pool[p]->location]->part_of_justice_system())
         {
            if(pool[p]->squadid!=-1)
            {
               sq=getsquad(pool[p]->squadid);
               if(sq!=-1)
               {
                  if(squad[sq]->activity.type!=ACTIVITY_NONE)continue;
               }
            }
            temppool.push_back(pool[p]);
         }
      }
   }
   return temppool;
}

/* base - activate the uninvolved */
void activate(void)
{
   vector<Creature *> temppool = activatable_liberals();

   if(temppool.size()==0)return;

   sortliberals(temppool,activesortingchoice[SORTINGCHOICE_ACTIVATE]);

   int page=0;

   char str[80];
   char num[20];

   do
   {
      erase();

      set_color(COLOR_WHITE,COLOR_BLACK,0);
      printfunds(0,1,"Money: ");


      move(0,0);
      addstr("Activate Uninvolved Liberals");
      move(1,0);
      addstr("----CODE NAME------------SKILL---HEALTH---LOCATION------------------------------");
      move(1,57);
      addstr("ACTIVITY");

      int y=2;
      for(int p=page*19;p<(int)temppool.size()&&p<page*19+19;p++)
      {
         set_color(COLOR_WHITE,COLOR_BLACK,0);
         move(y,0);
         addch(y+'A'-2);addstr(" - ");
         addstr(temppool[p]->name);

         char bright=0;
         int skill=0;
         for(int sk=0;sk<SKILLNUM;sk++)
         {
            skill+=temppool[p]->get_skill(sk);
            if(temppool[p]->get_skill_ip(sk)>=100+(10*temppool[p]->get_skill(sk))&&
               temppool[p]->get_skill(sk)<temppool[p]->skill_cap(sk,true))bright=1;
         }

         set_color(COLOR_WHITE,COLOR_BLACK,bright);

         move(y,25);
         itoa(skill,num,10);
         addstr(num);

         printhealthstat(*temppool[p],y,33,TRUE);

         if(mode==REVIEWMODE_JUSTICE)set_color(COLOR_YELLOW,COLOR_BLACK,1);
         else set_color(COLOR_WHITE,COLOR_BLACK,0);
         move(y,42);
         addstr(location[temppool[p]->location]->getname(true, true));

         move(y,57);
         // Let's add some color here...
         set_activity_color(temppool[p]->activity.type);
         getactivity(str,temppool[p]->activity);
         addstr(str);

         y++;
      }

      set_color(COLOR_WHITE,COLOR_BLACK,0);
      move(22,0);
      addstr("Press a Letter to Assign an Activity.");
      move(23,0);
      addpagestr();
      addstr(" T to sort people.");
      move(24,0);
      addstr("Press Z to assign simple tasks in bulk.");

      refresh();

      int c=getch();
      translategetch(c);

      //PAGE UP
      if((c==interface_pgup||c==KEY_UP||c==KEY_LEFT)&&page>0)page--;
      //PAGE DOWN
      if((c==interface_pgdn||c==KEY_DOWN||c==KEY_RIGHT)&&(page+1)*19<(int)temppool.size())page++;

      if(c>='a'&&c<='s')
      {
         int p=page*19+(int)(c-'a');
         if(p<(int)temppool.size())activate(temppool[p]);
      }

      if(c=='t')
      {
         sorting_prompt(SORTINGCHOICE_ACTIVATE);
         sortliberals(temppool,activesortingchoice[SORTINGCHOICE_ACTIVATE],true);
      }

      if(c=='z')
      {
         activatebulk();
      }

      if(c==10||c==ESC)break;
   }while(1);
}

int classlist = 0;
void listclasses(Creature *cr)
{
     set_color(COLOR_WHITE,COLOR_BLACK,0);
     move(10,40);
     addstr("Classes cost $60 a day. Study what?");
    if (classlist == 0)
    {
       set_color(COLOR_WHITE,COLOR_BLACK,cr->activity.type==ACTIVITY_STUDY_DEBATING);
       move(12,40);
       addstr("1 - Public Policy");

       set_color(COLOR_WHITE,COLOR_BLACK,cr->activity.type==ACTIVITY_STUDY_BUSINESS);
       move(13,40);
       addstr("2 - Economics");

       set_color(COLOR_WHITE,COLOR_BLACK,cr->activity.type==ACTIVITY_STUDY_PSYCHOLOGY);
       move(14,40);
       addstr("3 - Psychology");

       set_color(COLOR_WHITE,COLOR_BLACK,cr->activity.type==ACTIVITY_STUDY_LAW);
       move(15,40);
       addstr("4 - Criminal Law");

       set_color(COLOR_WHITE,COLOR_BLACK,cr->activity.type==ACTIVITY_STUDY_SCIENCE);
       move(16,40);
       addstr("5 - Physics");

       set_color(COLOR_WHITE,COLOR_BLACK,0);
       move(17,40);
       addstr("6 - Other classes");
    }
    else if (classlist == 1)
    {
       set_color(COLOR_WHITE,COLOR_BLACK,cr->activity.type==ACTIVITY_STUDY_DRIVING);
       move(12,40);
       addstr("1 - Drivers Ed");

       set_color(COLOR_WHITE,COLOR_BLACK,cr->activity.type==ACTIVITY_STUDY_FIRST_AID);
       move(13,40);
       addstr("2 - First Aid");


       set_color(COLOR_WHITE,COLOR_BLACK,cr->activity.type==ACTIVITY_STUDY_ART);
       move(14,40);
       addstr("3 - Painting");

       set_color(COLOR_WHITE,COLOR_BLACK,cr->activity.type==ACTIVITY_STUDY_DISGUISE);
       move(15,40);
       addstr("4 - Theatre");

       set_color(COLOR_WHITE,COLOR_BLACK,cr->activity.type==ACTIVITY_STUDY_MARTIAL_ARTS);
       move(16,40);
       addstr("5 - Kung Fu");

       set_color(COLOR_WHITE,COLOR_BLACK,0);
       move(17,40);
       addstr("6 - Other classes");
    }
    else if (classlist == 2)
    {
       set_color(COLOR_WHITE,COLOR_BLACK,cr->activity.type==ACTIVITY_STUDY_GYMNASTICS);
       move(12,40);
       addstr("1 - Gymnastics");

       set_color(COLOR_WHITE,COLOR_BLACK,cr->activity.type==ACTIVITY_STUDY_WRITING);
       move(13,40);
       addstr("2 - Writing");

       set_color(COLOR_WHITE,COLOR_BLACK,cr->activity.type==ACTIVITY_STUDY_TEACHING);
       move(14,40);
       addstr("3 - Education");

       set_color(COLOR_WHITE,COLOR_BLACK,cr->activity.type==ACTIVITY_STUDY_MUSIC);
       move(15,40);
       addstr("4 - Music");

       set_color(COLOR_WHITE,COLOR_BLACK,cr->activity.type==ACTIVITY_STUDY_LOCKSMITHING);
       move(16,40);
       addstr("5 - Locksmithing");

       set_color(COLOR_WHITE,COLOR_BLACK,0);
       move(17,40);
       addstr("6 - Other classes");
    }
}

void updateclasschoice(Creature *cr, char choice)
{
   if (choice == '6')
   {
      classlist = (classlist + 1) % 3;
      listclasses(cr);
   }
   else
   {
      if (classlist == 0)
      {
         switch(choice)
         {
            case '1':cr->activity.type=ACTIVITY_STUDY_DEBATING;break;
            case '2':cr->activity.type=ACTIVITY_STUDY_BUSINESS;break;
            case '3':cr->activity.type=ACTIVITY_STUDY_PSYCHOLOGY;break;
            case '4':cr->activity.type=ACTIVITY_STUDY_LAW;break;
            case '5':cr->activity.type=ACTIVITY_STUDY_SCIENCE;break;
         }
      }
      else if (classlist == 1)
      {
         switch(choice)
         {
            case '1':cr->activity.type=ACTIVITY_STUDY_DRIVING;break;
            case '2':cr->activity.type=ACTIVITY_STUDY_FIRST_AID;break;
            case '3':cr->activity.type=ACTIVITY_STUDY_ART;break;
            case '4':cr->activity.type=ACTIVITY_STUDY_DISGUISE;break;
            case '5':cr->activity.type=ACTIVITY_STUDY_MARTIAL_ARTS;break;
         }
      }
      else if (classlist == 2)
      {
         switch(choice)
         {
            case '1':cr->activity.type=ACTIVITY_STUDY_GYMNASTICS;break;
            case '2':cr->activity.type=ACTIVITY_STUDY_WRITING;break;
            case '3':cr->activity.type=ACTIVITY_STUDY_TEACHING;break;
            case '4':cr->activity.type=ACTIVITY_STUDY_MUSIC;break;
            case '5':cr->activity.type=ACTIVITY_STUDY_LOCKSMITHING;break;
         }
      }
      listclasses(cr);
   }
}

void activate(Creature *cr)
{
   int hostagecount=0;
   int state=0;
   int choice=0;
   char havedead=0;
   for(int p=0;p<(int)pool.size();p++)
   {
      if(pool[p]->alive&&pool[p]->align!=1&&pool[p]->location==cr->location)hostagecount++;
      if(!pool[p]->alive)havedead=1;
   }

   do
   {
      erase();

      set_color(COLOR_WHITE,COLOR_BLACK,0);
      printfunds(0,1,"Money: ");

      move(0,0);
      if (cr->income)
      {
         addstr(cr->name);
         addstr(" made $");
         char num[20];
         itoa(cr->income,num,10);
         addstr(num);
         addstr(" yesterday. What now?");
      }
      else
      {
         addstr("Taking Action: What will ");
         addstr(cr->name);
         addstr(" be doing today?");
      }

      printcreatureinfo(cr);

      makedelimiter(8,0);

      set_color(COLOR_WHITE,COLOR_BLACK,state=='a');
      mvaddstr(10,1,"A - Engaging in Liberal Activism");

      set_color(COLOR_WHITE,COLOR_BLACK,state=='b');
      mvaddstr(11,1,"B - Legal Fundraising");

      set_color(COLOR_WHITE,COLOR_BLACK,state=='c');
      mvaddstr(12,1,"C - Illegal Fundraising");

      set_color(COLOR_WHITE,COLOR_BLACK,state=='d');
      mvaddstr(13,1,"D - Recruitment and Acquisition");

      set_color(COLOR_WHITE,COLOR_BLACK,state=='t');
      mvaddstr(14,1,"T - Teaching Other Liberals");

      if(hostagecount>0)set_color(COLOR_WHITE,COLOR_BLACK,cr->activity.type==ACTIVITY_HOSTAGETENDING&&state==0);
      else set_color(COLOR_BLACK,COLOR_BLACK,1);
      mvaddstr(15,1,"I - Tend to a Conservative hostage");

      set_color(COLOR_WHITE,COLOR_BLACK,state=='l');
      mvaddstr(16,1,"L - Learn in the University District");

      if(clinictime(*cr)) set_color(COLOR_WHITE,COLOR_BLACK,cr->activity.type==ACTIVITY_CLINIC&&state==0);
      else set_color(COLOR_BLACK,COLOR_BLACK,1);
      mvaddstr(17,1,"M - Move to the Free Clinic");

      if(cr->get_skill(SKILL_FIRSTAID)!=0)
         set_color(COLOR_WHITE,COLOR_BLACK,(cr->activity.type==ACTIVITY_HEAL||cr->activity.type==ACTIVITY_NONE)&&state==0);
      else
         set_color(COLOR_BLACK,COLOR_BLACK,1);
      mvaddstr(18,1,"H - Heal Liberals");

      if(havedead)set_color(COLOR_WHITE,COLOR_BLACK,cr->activity.type==ACTIVITY_BURY&&state==0);
      else set_color(COLOR_BLACK,COLOR_BLACK,1);
      mvaddstr(19,1,"Z - Dispose of bodies");

      siegest *siege=NULL;
      if(selectedsiege!=-1)siege=&location[selectedsiege]->siege;
      if(activesquad!=NULL && activesquad->squad[0]->location!=-1)siege=&location[activesquad->squad[0]->location]->siege;
      char sieged=0;
      if(siege!=NULL)sieged=siege->siege;
      /*char underattack=0;
      if(siege!=NULL&&sieged)underattack=siege->underattack;*/

      if (!sieged)
      {
         set_color(COLOR_WHITE,COLOR_BLACK,0);
         mvaddstr(20,1,"E - Equip this Liberal");
      }

      if(state == 'a' || state == 'b' || state == 'c' || state == 'd' )
      {
         set_color(COLOR_WHITE,COLOR_BLACK,0);
         mvaddstr(19,40,"? - Help");
      }

      set_color(COLOR_WHITE,COLOR_BLACK,0);
      mvaddstr(20,40,"Enter - Confirm Selection");

      set_color(COLOR_WHITE,COLOR_BLACK,0);
      mvaddstr(21,1,"X - Nothing for Now");

      switch(state)
      {
      case 'a':
         set_color(COLOR_WHITE,COLOR_BLACK,cr->activity.type==ACTIVITY_COMMUNITYSERVICE);
         mvaddstr(10,40,"1 - Community Service");

         set_color(COLOR_WHITE,COLOR_BLACK,cr->activity.type==ACTIVITY_TROUBLE);
         mvaddstr(11,40,"2 - Liberal Disobedience");

         set_color(COLOR_WHITE,COLOR_BLACK,cr->activity.type==ACTIVITY_GRAFFITI);
         mvaddstr(12,40,"3 - Graffiti");

         set_color(COLOR_WHITE,COLOR_BLACK,cr->activity.type==ACTIVITY_POLLS);
         mvaddstr(13,40,"4 - Search Opinion Polls");

         //set_color(COLOR_WHITE,COLOR_BLACK,cr->activity.type==ACTIVITY_DOS_ATTACKS);
         //mvaddstr(14,40,"5 - Harass Websites");

         set_color(COLOR_WHITE,COLOR_BLACK,cr->activity.type==ACTIVITY_HACKING);
         mvaddstr(14,40,"5 - Hacking");

         set_color(COLOR_WHITE,COLOR_BLACK,cr->activity.type==ACTIVITY_WRITE_LETTERS);
         mvaddstr(15,40,"6 - Write to Newspapers");

         if(cr->location!=-1&&
            location[cr->location]->compound_walls & COMPOUND_PRINTINGPRESS)
         {
            set_color(COLOR_WHITE,COLOR_BLACK,cr->activity.type==ACTIVITY_WRITE_GUARDIAN);
            mvaddstr(16,40,"7 - Write for The Liberal Guardian");
         }
         break;
      case 'b':
         set_color(COLOR_WHITE,COLOR_BLACK,cr->activity.type==ACTIVITY_DONATIONS);
         mvaddstr(10,40,"1 - Solicit Donations");

         set_color(COLOR_WHITE,COLOR_BLACK,cr->activity.type==ACTIVITY_SELL_TSHIRTS);
         move(11,40);
         if(cr->get_skill(SKILL_TAILORING)>=8)
            addstr("2 - Sell Liberal T-Shirts");
         else if(cr->get_skill(SKILL_TAILORING)>=4)
            addstr("2 - Sell Embroidered Shirts");
         else
            addstr("2 - Sell Tie-Dyed T-Shirts");

         set_color(COLOR_WHITE,COLOR_BLACK,cr->activity.type==ACTIVITY_SELL_ART);
         move(12,40);
         if(cr->get_skill(SKILL_ART)>=8)
            addstr("3 - Sell Liberal Art");
         else if(cr->get_skill(SKILL_ART)>=4)
            addstr("3 - Sell Paintings");
         else
            addstr("3 - Sell Portrait Sketches");

         set_color(COLOR_WHITE,COLOR_BLACK,cr->activity.type==ACTIVITY_SELL_MUSIC);
         move(13,40);
         if(cr->get_skill(SKILL_MUSIC)>8)
            addstr("4 - Play Liberal Music");
         else
            addstr("4 - Play Street Music");
         break;
      case 'c':
         set_color(COLOR_WHITE,COLOR_BLACK,cr->activity.type==ACTIVITY_SELL_DRUGS);
         mvaddstr(10,40,"1 - Sell Brownies");

         set_color(COLOR_WHITE,COLOR_BLACK,cr->activity.type==ACTIVITY_PROSTITUTION);
         if(cr->age < 18)
            set_color(COLOR_BLACK, COLOR_BLACK, 1);    //Grayed out for minors
         mvaddstr(11,40,"2 - Prostitution");

         set_color(COLOR_WHITE,COLOR_BLACK,cr->activity.type==ACTIVITY_CCFRAUD);
         mvaddstr(12,40,"3 - Steal Credit Card Numbers");

         /*set_color(COLOR_WHITE,COLOR_BLACK,cr->activity.type==ACTIVITY_DOS_RACKET);
         mvaddstr(13,40,"4 - Electronic Protection Racket");*/
         break;
      case 'd':
         set_color(COLOR_WHITE,COLOR_BLACK,cr->activity.type==ACTIVITY_RECRUITING);
         mvaddstr(10,40,"1 - Recruiting");

         set_color(COLOR_WHITE,COLOR_BLACK,cr->activity.type==ACTIVITY_MAKE_ARMOR);
         mvaddstr(11,40,"2 - Make Clothing");

         set_color(COLOR_WHITE,COLOR_BLACK,cr->activity.type==ACTIVITY_REPAIR_ARMOR);
         mvaddstr(12,40,"3 - Repair Clothing");

         if(cr->canwalk()) {
            set_color(COLOR_WHITE,COLOR_BLACK,cr->activity.type==ACTIVITY_STEALCARS);
            mvaddstr(13,40,"4 - Steal a Car");
         } else {
            if(!(cr->flag & CREATUREFLAG_WHEELCHAIR))set_color(COLOR_WHITE,COLOR_BLACK,cr->activity.type==ACTIVITY_WHEELCHAIR);
            else set_color(COLOR_BLACK,COLOR_BLACK,1);
            mvaddstr(13,40,"4 - Procure a Wheelchair");
         }
         break;
      case 't':
         set_color(COLOR_WHITE,COLOR_BLACK,0);
         move(10,40);
         addstr("Teach Liberals About What?");

         set_color(COLOR_WHITE,COLOR_BLACK,cr->activity.type==ACTIVITY_TEACH_POLITICS);
         move(12,40);
         addstr("1 - Political Activism");

         set_color(COLOR_WHITE,COLOR_BLACK,cr->activity.type==ACTIVITY_TEACH_COVERT);
         move(13,40);
         addstr("2 - Infiltration");

         set_color(COLOR_WHITE,COLOR_BLACK,cr->activity.type==ACTIVITY_TEACH_FIGHTING);
         move(14,40);
         addstr("3 - Urban Warfare");
       break;
     case 'l':
       listclasses(cr);
       break;
      }

      set_color(COLOR_WHITE,COLOR_BLACK,0);
      switch(cr->activity.type)
      {
      case ACTIVITY_COMMUNITYSERVICE:
         move(22,3);
         addstr(cr->name);
         addstr(" will help the elderly, local library, anything");
         move(23,1);
         addstr("  that is liberal.");
         break;
      case ACTIVITY_TROUBLE:
         move(22,3);
         addstr(cr->name);
         addstr(" will create public disturbances. ");
         break;
      case ACTIVITY_GRAFFITI:
         move(22,3);
         addstr(cr->name);
         addstr(" will spray political graffiti. Art and Heart will");
         move(23,1);
         addstr("  enhance the liberal effect.");
         break;
      case ACTIVITY_POLLS:
         move(22,3);
         addstr(cr->name);
         addstr(" will search the internet for public opinion polls.");
         move(23,1);
         addstr("  Polls will give an idea on how the liberal agenda is going. Computers");
         move(24,1);
         addstr("  and intelligence will provide better results.");
         break;
      case ACTIVITY_DOS_ATTACKS:
         move(22,3);
         addstr(cr->name);
         addstr(" will harass Conservative websites. Computer skill");
         move(23,1);
         addstr("  will give greater effect.");
         break;
      case ACTIVITY_HACKING:
         move(22,3);
         addstr(cr->name);
         addstr(" will harass websites and hack private networks.");
         move(23,1);
         addstr("  Computer skill and intelligence will give more frequent results.");
         move(24,1);
         addstr("  Multiple hackers will increase chances of both success and detection.");
         break;
      case ACTIVITY_WRITE_LETTERS:
         move(22,3);
         addstr(cr->name);
         addstr(" will write letters to newspapers about current events.");
         break;
      case ACTIVITY_WRITE_GUARDIAN:
         move(22,3);
         addstr(cr->name);
         addstr(" will write articles for the LCS's newspaper.");
         break;
      case ACTIVITY_DONATIONS:
         move(22,3);
         addstr(cr->name);
         addstr(" will walk around and ask for donations to the LCS.");
         move(23,1);
         addstr("  Based on persuasion, public's view on the cause, and how well dressed the");
         move(24,1);
         addstr("  activist is.");
         break;
      case ACTIVITY_SELL_TSHIRTS:
         move(22,3);
         addstr(cr->name);
         if(cr->get_skill(SKILL_TAILORING)>=8)
            addstr(" will print and distribute shirts with Liberal slogans.");
         else if(cr->get_skill(SKILL_TAILORING)>=4)
            addstr(" will embroider shirts and sell them on the street.");
         else
            addstr(" will tie-dye T-shirts and sell them on the street.");
         break;
      case ACTIVITY_SELL_ART:
         move(22,3);
         addstr(cr->name);
         if(cr->get_skill(SKILL_ART)>=8)
            addstr(" will create and sell paintings embracing the Liberal agenda.");
         else if(cr->get_skill(SKILL_ART)>=4)
            addstr(" will make pretty paintings and sell them on the streets.");
         else
            addstr(" will sketch people and sell portraits back to them.");
         break;
      case ACTIVITY_SELL_MUSIC:
         move(22,3);
         addstr(cr->name);
         addstr(" will go out into the streets and drum on buckets,");
         move(23,1);
         addstr("  or play guitar if one is equipped.");
         break;
      case ACTIVITY_RECRUITING:
         move(22,3);
         addstr(cr->name);
         addstr(" will try to find someone to join the LCS.");
         break;
      case ACTIVITY_STEALCARS:
         move(22,3);
         addstr(cr->name);
         addstr(" will try to find and steal a car.");
         break;
      case ACTIVITY_SELL_DRUGS:
         move(22,3);
         addstr(cr->name);
         addstr(" will bake and sell special adult brownies that open");
         move(23,1);
         addstr("  magical shimmering doorways to the adamantium pits.");
         break;
      case ACTIVITY_PROSTITUTION:
         move(22,3);
         addstr(cr->name);
         addstr(" will trade sex for money.");
         break;
      case ACTIVITY_CCFRAUD:
         move(22,3);
         addstr(cr->name);
         addstr(" will commit credit card fraud online.");
         break;
      case ACTIVITY_DOS_RACKET:
         move(22,3);
         addstr(cr->name);
         addstr(" will demand money in exchange for not bringing down");
         move(23,1);
         addstr("major websites.");
         break;
      case ACTIVITY_TEACH_POLITICS:
         move(22,1);
         addstr("  Skills Trained: Writing, Persuasion, Law, Street Sense, Science,");
         move(23,1);
         addstr("    Religion, Business, Music, Art");
         move(24,1);
         addstr("  Classes cost up to $20/day to conduct. All Liberals able will attend.");
         break;
      case ACTIVITY_TEACH_COVERT:
         move(22,1);
         addstr("  Skills Trained: Computers, Security, Stealth, Disguise, Tailoring,");
         move(23,1);
         addstr("    Seduction, Psychology, Driving");
         move(24,1);
         addstr("  Classes cost up to $60/day to conduct. All Liberals able will attend.");
         break;
      case ACTIVITY_TEACH_FIGHTING:
         move(22,1);
         addstr("  Skills Trained: All Weapon Skills, Martial Arts, Dodge, First Aid");
         move(24,1);
         addstr("  Classes cost up to $100/day to conduct. All Liberals able will attend.");
         break;
      case ACTIVITY_STUDY_DEBATING:
      case ACTIVITY_STUDY_MARTIAL_ARTS:
      case ACTIVITY_STUDY_DRIVING:
      case ACTIVITY_STUDY_PSYCHOLOGY:
      case ACTIVITY_STUDY_FIRST_AID:
      case ACTIVITY_STUDY_LAW:
      case ACTIVITY_STUDY_DISGUISE:
      case ACTIVITY_STUDY_SCIENCE:
      case ACTIVITY_STUDY_BUSINESS:
      //case ACTIVITY_STUDY_COOKING:
      case ACTIVITY_STUDY_GYMNASTICS:
      case ACTIVITY_STUDY_WRITING:
      case ACTIVITY_STUDY_ART:
      case ACTIVITY_STUDY_MUSIC:
      case ACTIVITY_STUDY_TEACHING:
      case ACTIVITY_STUDY_LOCKSMITHING:
         move(22,3);
         addstr(cr->name);
         addstr(" will attend classes in the University District");
         move(23,1);
         addstr("  at a cost of $60 a day.");
         break;
      }

      refresh();
      int c=getch();
      translategetch(c);



      if(c>='a'&&c<='z'){state=c;}
      if((c>='a'&&c<='z') || (c>='1'&&c<='9'))
      {
         choice=c;
         switch(state)
         {
         case 'a':
            switch(choice)
            {
            case '1':cr->activity.type=ACTIVITY_COMMUNITYSERVICE;break;
            case '2':cr->activity.type=ACTIVITY_TROUBLE;break;
            case '3':cr->activity.type=ACTIVITY_GRAFFITI;
               cr->activity.arg=-1;
               break;
            case '4':cr->activity.type=ACTIVITY_POLLS;break;
            //case '5':cr->activity.type=ACTIVITY_DOS_ATTACKS;break;
            case '5':cr->activity.type=ACTIVITY_HACKING;break;
            case '6':cr->activity.type=ACTIVITY_WRITE_LETTERS;break;
            case '7':
               if(cr->location!=-1&&
                  location[cr->location]->compound_walls & COMPOUND_PRINTINGPRESS)
               {
                  cr->activity.type=ACTIVITY_WRITE_GUARDIAN;break;
               }
            default:
               if(cr->get_attribute(ATTRIBUTE_WISDOM,true)>7)
               {
                  cr->activity.type=ACTIVITY_COMMUNITYSERVICE;
                  choice='1';
               }
               else if(cr->get_attribute(ATTRIBUTE_WISDOM,true)>4)
               {
                  cr->activity.type=ACTIVITY_TROUBLE;
                  choice='2';
               }
               else
               {
                  if(cr->get_skill(SKILL_COMPUTERS)>2)
                  {
                     cr->activity.type=ACTIVITY_HACKING;
                     choice='5';
                  }
                  else if(cr->get_skill(SKILL_ART)>1)
                  {
                     cr->activity.type=ACTIVITY_GRAFFITI;
                     cr->activity.arg=-1;
                     choice='3';
                  }
                  else
                  {
                     cr->activity.type=ACTIVITY_TROUBLE;
                     choice='2';
                  }
               }
            }
            break;
         case 'b':
            switch(choice)
            {
            case '1':cr->activity.type=ACTIVITY_DONATIONS;break;
            case '2':cr->activity.type=ACTIVITY_SELL_TSHIRTS;break;
            case '3':cr->activity.type=ACTIVITY_SELL_ART;break;
            case '4':cr->activity.type=ACTIVITY_SELL_MUSIC;break;
            default:
               if(cr->get_skill(SKILL_ART)>1)
               {
                  cr->activity.type=ACTIVITY_SELL_ART;
                  choice='3';
               }
               else if(cr->get_skill(SKILL_TAILORING)>1)
               {
                  cr->activity.type=ACTIVITY_SELL_TSHIRTS;
                  choice='2';
               }
               else if(cr->get_skill(SKILL_MUSIC)>1)
               {
                  cr->activity.type=ACTIVITY_SELL_MUSIC;
                  choice='4';
               }
               else
               {
                  cr->activity.type=ACTIVITY_DONATIONS;
                  choice='1';
               }
            }
            break;
         case 'c':
            switch(choice)
            {
            case '1':cr->activity.type=ACTIVITY_SELL_DRUGS;break;
            case '2':
               if(cr->age >= 18) cr->activity.type=ACTIVITY_PROSTITUTION;break;
            case '3':cr->activity.type=ACTIVITY_CCFRAUD;break;
               //case '4':cr->activity.type=ACTIVITY_DOS_RACKET;break;
            default:
               if(cr->get_skill(SKILL_COMPUTERS)>1)
               {
                  cr->activity.type=ACTIVITY_CCFRAUD;
                  choice='3';
               }
               else if(cr->get_skill(SKILL_SEDUCTION)>1 && cr->age >= 18)
               {
                  cr->activity.type=ACTIVITY_PROSTITUTION;
                  choice='2';
               }
               else
               {
                  cr->activity.type=ACTIVITY_SELL_DRUGS;
                  choice='1';
               }
            }
            break;
         case 'd':
            switch(choice)
            {
            default:
            case '1':break;//cr->activity.type=ACTIVITY_RECRUITING;break;
            case '2':break;
            case '3':cr->activity.type=ACTIVITY_REPAIR_ARMOR;break;
            case '4':
               if(cr->canwalk())
                  cr->activity.type=ACTIVITY_STEALCARS;
               else if(!(cr->flag & CREATUREFLAG_WHEELCHAIR))
                  cr->activity.type=ACTIVITY_WHEELCHAIR;
               break;
            }
            break;
       case 'l':
          updateclasschoice(cr, choice);
          break;
         case 't':
            switch(choice)
            {
            case '1':cr->activity.type=ACTIVITY_TEACH_POLITICS;break;
            case '2':cr->activity.type=ACTIVITY_TEACH_COVERT;break;
            case '3':cr->activity.type=ACTIVITY_TEACH_FIGHTING;break;
            default:
               switch(cr->type)
               {
               // this first block are creatures with All Weapon Skills, Martial Arts, Dodge, and First Aid
               case CREATURE_ATHLETE:
               case CREATURE_BOUNCER: // for fighting skills
               case CREATURE_COP: // for fighting skills
               case CREATURE_CCS_ARCHCONSERVATIVE: // for fighting skills
               case CREATURE_CCS_MOLOTOV: // for fighting skills
               case CREATURE_CCS_SNIPER: // for fighting skills
               case CREATURE_CCS_VIGILANTE: // for fighting skills
               case CREATURE_DEATHSQUAD: // for fighting skills
               case CREATURE_DOCTOR: // for First Aid
               case CREATURE_FIREFIGHTER: // for fighting skills
               case CREATURE_GANGMEMBER: // for fighting skills
               case CREATURE_GANGUNIT: // for fighting skills
               case CREATURE_GUARDDOG:
               case CREATURE_GENETIC:
               case CREATURE_HARDENED_VETERAN: // for fighting skills
               case CREATURE_HICK:
               case CREATURE_MARTIALARTIST: // for fighting skills
               case CREATURE_MERC: // for fighting skills
               case CREATURE_MILITARYOFFICER: // for fighting skills
               case CREATURE_MILITARYPOLICE: // for fighting skills
               case CREATURE_MUTANT:
               case CREATURE_NURSE: // for First Aid
               case CREATURE_PRISONGUARD: // for fighting skills
               case CREATURE_SEAL: // for fighting skills
               case CREATURE_SECURITYGUARD: // for fighting skills
               case CREATURE_SOLDIER: // for fighting skills
               case CREATURE_SWAT: // for fighting skills
               case CREATURE_TANK: // fpr fighting skills
               case CREATURE_VETERAN: // for fighting skills
                  cr->activity.type=ACTIVITY_TEACH_FIGHTING;
                  choice='2';
                  break;
               // this second block are creatures with Computers, Security, Stealth, Disguise, Tailoring, Seduction, Psychology, & Driving
               case CREATURE_ACTOR: // for Disguise
               case CREATURE_AGENT: // for Driving & Psychology
               case CREATURE_AMATEURMAGICIAN:
               case CREATURE_BIKER: // for Driving
               case CREATURE_BUM:
               case CREATURE_CONSTRUCTIONWORKER: // for Driving
               case CREATURE_CRACKHEAD:
               case CREATURE_EDUCATOR: // for Psychology & Driving
               case CREATURE_FASHIONDESIGNER: // for Tailoring
               case CREATURE_GARBAGEMAN: // for Driving
               case CREATURE_HSDROPOUT:
               case CREATURE_LOCKSMITH: // for Security
               case CREATURE_MAILMAN:
               case CREATURE_PLUMBER:
               case CREATURE_PRISONER:
               case CREATURE_PROGRAMMER: // for Computers
               case CREATURE_PROSTITUTE: // for Seduction
               case CREATURE_PSYCHOLOGIST: // for Psychology
               case CREATURE_SECRET_SERVICE: // for Driving & Psychology
               case CREATURE_SEWERWORKER:
               case CREATURE_TAXIDRIVER: // for Driving
               case CREATURE_THIEF: // for Disguise, Security, & Stealth
               case CREATURE_TRUCKER: // for Driving
               case CREATURE_WORKER_FACTORY_CHILD:
               case CREATURE_WORKER_SERVANT:
               case CREATURE_WORKER_SWEATSHOP: // for Tailoring
                  cr->activity.type=ACTIVITY_TEACH_COVERT;
                  choice='3';
                  break;
               // this third block are creatures with Writing, Persuasion, Law, Street Sense, Science, Religion, Business, Music, & Art
               case CREATURE_AUTHOR: // for Writing & Persuasion
               case CREATURE_BANK_MANAGER: // for Business
               case CREATURE_BANK_TELLER:
               case CREATURE_CAMERAMAN: // for Art
               case CREATURE_CARSALESMAN: // for Business & Persuasion
               case CREATURE_CHEF:
               case CREATURE_CLERK:
               case CREATURE_COLLEGESTUDENT: // for Art, Music, Science, & Writing
               case CREATURE_CORPORATE_CEO: // for Business
               case CREATURE_CORPORATE_MANAGER: // for Business
               case CREATURE_CRITIC_ART: // for Writing, Persuasion, & Art
               case CREATURE_CRITIC_MUSIC: // for Writing, Persuasion, & Music
               case CREATURE_DANCER: // for Art & Music
               case CREATURE_ENGINEER: // for Science
               case CREATURE_FASTFOODWORKER:
               case CREATURE_FOOTBALLCOACH: // for Persuasion
               case CREATURE_HAIRSTYLIST: // for Art
               case CREATURE_HIPPIE: // for Art & Music
               case CREATURE_JOURNALIST: // for Writing & Persuasion
               case CREATURE_JUDGE_CONSERVATIVE: // for Law & Writing
               case CREATURE_JUDGE_LIBERAL: // for Law & Writing
               case CREATURE_JUROR:
               case CREATURE_LANDLORD: // for Business
               case CREATURE_LAWYER: // for Law & Persuasion
               case CREATURE_MATHEMATICIAN: // for Science
               case CREATURE_MUSICIAN: // for Music
               case CREATURE_NEWSANCHOR:
               case CREATURE_NUN: // for Religion
               case CREATURE_OFFICEWORKER: // for Business
               case CREATURE_PAINTER: // for Art
               case CREATURE_PHOTOGRAPHER: // for Art
               case CREATURE_POLITICALACTIVIST: // for Persuasion, Law, & Writing
               case CREATURE_POLITICIAN: // for Law & Persuasion
               case CREATURE_PRIEST: // for Religion
               case CREATURE_RADIOPERSONALITY: // for Persuasion
               case CREATURE_RETIREE:
               case CREATURE_SCIENTIST_EMINENT: // for Science
               case CREATURE_SCIENTIST_LABTECH: // for Science
               case CREATURE_SCULPTOR: // for Art
               case CREATURE_SOCIALITE: // for Persuasion, Art, & Music
               case CREATURE_TEACHER:
               case CREATURE_TEENAGER:
               case CREATURE_TELEMARKETER: // for Persuasion
               case CREATURE_WORKER_FACTORY_NONUNION:
               case CREATURE_WORKER_FACTORY_UNION:
               case CREATURE_WORKER_JANITOR:
               case CREATURE_WORKER_SECRETARY: // for Writing
               case CREATURE_YOGAINSTRUCTOR:
               default:
                  cr->activity.type=ACTIVITY_TEACH_POLITICS;
                  choice='1';
                  break;
               }
               break;
            }
            break;
         }
      }

      if(c=='h'&&cr->get_skill(SKILL_FIRSTAID)!=0)
      {
         cr->activity.type=ACTIVITY_HEAL;
         break;
      }
      // Pick type to recruit
      if(state=='d'&&choice=='1')
      {
         activityst oact=cr->activity;
         cr->activity.type=ACTIVITY_NONE;
         recruitSelect(*cr);
         if(cr->activity.type==ACTIVITY_RECRUITING)break;
         else cr->activity=oact;
      }
      // Pick clothing to make
      if(state=='d'&&choice=='2')
      {
         activityst oact=cr->activity;
         cr->activity.type=ACTIVITY_NONE;
         select_makeclothing(cr);
         if(cr->activity.type==ACTIVITY_MAKE_ARMOR)break;
         else cr->activity=oact;
      }
      if(c=='i'&&hostagecount>0)
      {
         activityst oact=cr->activity;
         cr->activity.type=ACTIVITY_NONE;
         select_tendhostage(cr);
         if(cr->activity.type==ACTIVITY_HOSTAGETENDING)break;
         else cr->activity=oact;
      }
      if (!sieged && c == 'e')
      {
         //create a temp squad containing just this liberal
         int oldsquadid = cr->squadid;
         squadst *oldactivesquad = activesquad;
         activesquad=new squadst;
         strcpy(activesquad->name, "Temporary Squad");
         activesquad->id=cursquadid;
         activesquad->squad[0]=cr;
         cr->squadid = activesquad->id;
         //go to equipment screen
         equip(location[activesquad->squad[0]->location]->loot,-1);
         //once you're done, restore original squad status.
         delete activesquad;
         activesquad = oldactivesquad;
         cr->squadid = oldsquadid;
      }
      /*if(c=='w'&&location[cr->location]->compound_walls==COMPOUND_PRINTINGPRESS)
      {
      activityst oact=cr->activity;
      cr->activity.type=ACTIVITY_NONE;
      if(select_view(cr,cr->activity.arg))
      cr->activity.type=ACTIVITY_WRITE_GUARDIAN;
      else cr->activity=oact;
      break;
      }*/
      if(c=='m'&&clinictime(*cr))
      {
         cr->activity.type=ACTIVITY_CLINIC;
         break;
      }
      if(c=='z'&&havedead)
      {
         cr->activity.type=ACTIVITY_BURY;
         break;
      }
      if(c=='x')
      {
         cr->activity.type=ACTIVITY_NONE;
         break;
      }
      // Enter pressed
      if(c==10||c==ESC)
      {
         break;
      }
      // ? Pressed
      if(c==63)
      {
         if(state == 'a' || state == 'b' || state == 'c' ||state == 'd' )
         {
            // Call activity help pages
            HelpActivities(cr->activity.type);
         }
      }
   }while(1);
}


void activatebulk(void)
{
   vector<Creature *> temppool = activatable_liberals();

   if(temppool.size()==0)return;

   int page=0;

   char str[80];

   int selectedactivity=0;

   do
   {
      erase();

      set_color(COLOR_WHITE,COLOR_BLACK,0);
      printfunds(0,1,"Money: ");

      set_color(COLOR_WHITE,COLOR_BLACK,0);
      move(0,0);
      addstr("Activate Uninvolved Liberals");
      move(1,0);
      addstr("----CODE NAME------------CURRENT ACTIVITY---------------------------------------");
      move(1,51);
      addstr("BULK ACTIVITY");

      if(selectedactivity==0)set_color(COLOR_WHITE,COLOR_BLACK,1);
      else set_color(COLOR_WHITE,COLOR_BLACK,0);
      move(2,51);
      addstr("1 - Engaging in Liberal Activism");
      if(selectedactivity==1)set_color(COLOR_WHITE,COLOR_BLACK,1);
      else set_color(COLOR_WHITE,COLOR_BLACK,0);
      move(3,51);
      addstr("2 - Legal Fundraising");
      if(selectedactivity==2)set_color(COLOR_WHITE,COLOR_BLACK,1);
      else set_color(COLOR_WHITE,COLOR_BLACK,0);
      move(4,51);
      addstr("3 - Illegal Fundraising");
      if(selectedactivity==3)set_color(COLOR_WHITE,COLOR_BLACK,1);
      else set_color(COLOR_WHITE,COLOR_BLACK,0);
      move(5,51);
      addstr("4 - Checking Polls");
      if(selectedactivity==4)set_color(COLOR_WHITE,COLOR_BLACK,1);
      else set_color(COLOR_WHITE,COLOR_BLACK,0);
      move(6,51);
      addstr("5 - Stealing Cars");
      if(selectedactivity==5)set_color(COLOR_WHITE,COLOR_BLACK,1);
      else set_color(COLOR_WHITE,COLOR_BLACK,0);
      move(7,51);
      addstr("6 - Community Service");

      int y=2;
      for(int p=page*19;p<(int)temppool.size()&&p<page*19+19;p++)
      {
         set_color(COLOR_WHITE,COLOR_BLACK,0);
         move(y,0);
         addch(y+'A'-2);addstr(" - ");
         addstr(temppool[p]->name);

         move(y,25);
         set_activity_color(temppool[p]->activity.type);
         getactivity(str,temppool[p]->activity);
         addstr(str);
         /*if(temppool[p]->activity.type==ACTIVITY_TROUBLE)
         {
         addstr(" ($");
         itoa(temppool[p]->activity.arg,num,10);
         addstr(num);
         addstr(")");
         }*/

         y++;
      }

      set_color(COLOR_WHITE,COLOR_BLACK,0);
      move(22,0);
      addstr("Press a Letter to Assign an Activity.  Press a Number to select an Activity.");
      move(23,0);
      addpagestr();

      refresh();

      int c=getch();
      translategetch(c);

      //PAGE UP
      if((c==interface_pgup||c==KEY_UP||c==KEY_LEFT)&&page>0)page--;
      //PAGE DOWN
      if((c==interface_pgdn||c==KEY_DOWN||c==KEY_RIGHT)&&(page+1)*19<(int)temppool.size())page++;

      if(c>='a'&&c<='s')
      {
         int p=page*19+(int)(c-'a');
         if(p<(int)temppool.size())
         {
            switch(selectedactivity)
            {
            case 0: //Activism
               if(temppool[p]->get_attribute(ATTRIBUTE_WISDOM,true)>7)
                  temppool[p]->activity.type=ACTIVITY_COMMUNITYSERVICE;
               else if(temppool[p]->get_attribute(ATTRIBUTE_WISDOM,true)>4)
                  temppool[p]->activity.type=ACTIVITY_TROUBLE;
               else
               {
                  if(temppool[p]->get_skill(SKILL_COMPUTERS)>2)
                     temppool[p]->activity.type=ACTIVITY_HACKING;
                  else if(temppool[p]->get_skill(SKILL_ART)>1)
                  {
                     temppool[p]->activity.type=ACTIVITY_GRAFFITI;
                     temppool[p]->activity.arg=-1;
                  }
                  else temppool[p]->activity.type=ACTIVITY_TROUBLE;
               }
               break;
            case 1: //Fundraising
               if(temppool[p]->get_skill(SKILL_ART)>1)
                  temppool[p]->activity.type=ACTIVITY_SELL_ART;
               else if(temppool[p]->get_skill(SKILL_MUSIC)>1)
                  temppool[p]->activity.type=ACTIVITY_SELL_MUSIC;
               else if(temppool[p]->get_skill(SKILL_TAILORING)>1)
                  temppool[p]->activity.type=ACTIVITY_SELL_TSHIRTS;
               else temppool[p]->activity.type=ACTIVITY_DONATIONS;
               break;
            case 2: //Illegal Fundraising
               if(temppool[p]->get_skill(SKILL_COMPUTERS)>1)
                  temppool[p]->activity.type=ACTIVITY_CCFRAUD;
#ifndef ZEROMORAL
                else if(temppool[p]->get_skill(SKILL_SEDUCTION)>1 && temppool[p]->age >=18)
#else
               else if(temppool[p]->get_skill(SKILL_SEDUCTION)>1)
#endif
                  temppool[p]->activity.type=ACTIVITY_PROSTITUTION;
               else
                  temppool[p]->activity.type=ACTIVITY_SELL_DRUGS;
               break;
            case 3: //Check polls
               temppool[p]->activity.type=ACTIVITY_POLLS;
               break;
            case 4: //Steal cars
               temppool[p]->activity.type=ACTIVITY_STEALCARS;
               break;
            case 5: //Volunteer
               temppool[p]->activity.type=ACTIVITY_COMMUNITYSERVICE;
               break;
            }
         }
      }
      if(c>='1'&&c<='6')
      {
         selectedactivity=c-'1';
      }

      if(c==10||c==ESC)break;
   }while(1);
}


/* base - activate - hostages */
void select_tendhostage(Creature *cr)
{
   vector<Creature *> temppool;

   for(int p=0;p<(int)pool.size();p++)
   {
      if(pool[p]->align!=1&&
         pool[p]->alive&&
         pool[p]->location==cr->location)
      {
         temppool.push_back(pool[p]);
      }
   }

   if(temppool.size()==0)return;
   if(temppool.size()==1)
   {
      cr->activity.type=ACTIVITY_HOSTAGETENDING;
      cr->activity.arg=temppool[0]->id;
      return;
   }


   int page=0;

   char num[20];

   do
   {
      erase();

      set_color(COLOR_WHITE,COLOR_BLACK,0);
      move(0,0);
      addstr("Which hostage will ");
      addstr(cr->name);
      addstr(" be watching over?");
      move(1,0);
      addstr("----CODE NAME------------SKILL---HEALTH---LOCATION------------------------------");
      move(1,57);
      addstr("DAYS IN CAPTIVITY");

      int y=2;
      for(int p=page*19;p<(int)temppool.size()&&p<page*19+19;p++)
      {
         set_color(COLOR_WHITE,COLOR_BLACK,0);
         move(y,0);
         addch(y+'A'-2);addstr(" - ");
         addstr(temppool[p]->name);

         char bright=0;
         int skill=0;
         for(int sk=0;sk<SKILLNUM;sk++)
         {
            skill+=temppool[p]->get_skill(sk);
            if(temppool[p]->get_skill_ip(sk)>=100+(10*temppool[p]->get_skill(sk))&&
               temppool[p]->get_skill(sk)<temppool[p]->skill_cap(sk,true))bright=1;
         }

         set_color(COLOR_WHITE,COLOR_BLACK,bright);

         move(y,25);
         itoa(skill,num,10);
         addstr(num);

         printhealthstat(*temppool[p],y,33,TRUE);

         if(mode==REVIEWMODE_JUSTICE)set_color(COLOR_YELLOW,COLOR_BLACK,1);
         else set_color(COLOR_WHITE,COLOR_BLACK,0);
         mvaddstr(y,42,location[temppool[p]->location]->getname(true, true));

         move(y,57);
         set_color(COLOR_MAGENTA,COLOR_BLACK,1);
         char num[20];
         itoa(temppool[p]->joindays,num,10);
         addstr(num);
         addstr(" ");
         if(temppool[p]->joindays>1)addstr("Days");
         else addstr("Day");

         y++;
      }

      set_color(COLOR_WHITE,COLOR_BLACK,0);
      move(22,0);
      addstr("Press a Letter to select a Conservative");
      move(23,0);
      addpagestr();

      refresh();

      int c=getch();
      translategetch(c);

      //PAGE UP
      if((c==interface_pgup||c==KEY_UP||c==KEY_LEFT)&&page>0)page--;
      //PAGE DOWN
      if((c==interface_pgdn||c==KEY_DOWN||c==KEY_RIGHT)&&(page+1)*19<(int)temppool.size())page++;

      if(c>='a'&&c<='s')
      {
         int p=page*19+(int)(c-'a');
         if(p<(int)temppool.size())
         {
            cr->activity.type=ACTIVITY_HOSTAGETENDING;
            cr->activity.arg=temppool[p]->id;
            return;
         }
      }

      if(c==10||c==ESC)break;
   }while(1);
}


long select_hostagefundinglevel(Creature *cr,Creature *hs)
{
   long flevel=-1;

   erase();



   set_color(COLOR_WHITE,COLOR_BLACK,0);
   printfunds(0,1,"Money: ");
   move(0,0);
   addstr("Select a Funding Level for this Operation:");

   move(2,0);
   addstr("A - Don't spend anything.  ");
   addstr(cr->name);
   addstr(" is just on watch duty.");

   move(3,0);
   addstr("B - Don't spend anything.  ");
   addstr(cr->name);
   addstr(" will turn the prisoner over time.");

   move(4,0);
   addstr("C - $20 per day.  Enough for some props.");

   move(5,0);
   addstr("D - $50 per day.  ");
   addstr(cr->name);
   addstr(" will go for a thrifty indoctrination.");

   move(6,0);
   addstr("E - $100 per day.  ");
   addstr(cr->name);
   addstr(" needs enough freedom to be creative.");

   move(7,0);
   addstr("F - $500 per day.  It is imperative that the Conservative be turned quickly.");

   move(8,0);
   addstr("K - This Conservative has become a liability and needs to be terminated.");

   move(10,0);
   addstr("Enter - On second thought, this isn't a job for ");
   addstr(cr->name);
   addstr(".");

   refresh();
   int c=getch();
   translategetch(c);

   if(c=='a')flevel=0;
   if(c=='b')flevel=1;
   if(c=='c')flevel=20;
   if(c=='d')flevel=50;
   if(c=='e')flevel=100;
   if(c=='f')flevel=500;
   if(c=='k')flevel=666;

   return flevel;
}

struct recruitData
{
   int type;
   char* name;
   int difficulty;
   recruitData(int type_, char* name_, int difficulty_):type(type_),name(name_),difficulty(difficulty_) {}
};

recruitData recruitable_creatures[] = {
   recruitData(CREATURE_VETERAN, (char*)"Army Veteran", 4),
   recruitData(CREATURE_ATHLETE, (char*)"Athlete", 4),
   recruitData(CREATURE_COLLEGESTUDENT, (char*)"College Student", 1),
   recruitData(CREATURE_PROGRAMMER, (char*)"Computer Programmer", 4),
   recruitData(CREATURE_DANCER, (char*)"Dancer", 4),
   recruitData(CREATURE_DOCTOR, (char*)"Doctor", 4),
   recruitData(CREATURE_FASHIONDESIGNER, (char*)"Fashion Designer", 6),
   recruitData(CREATURE_GANGMEMBER, (char*)"Gang Member", 2),
   recruitData(CREATURE_HIPPIE, (char*)"Hippie", 1),
   recruitData(CREATURE_JOURNALIST, (char*)"Journalist", 4),
   recruitData(CREATURE_JUDGE_LIBERAL, (char*)"Judge", 6),
   recruitData(CREATURE_LAWYER, (char*)"Lawyer", 4),
   recruitData(CREATURE_LOCKSMITH, (char*)"Locksmith", 6),
   recruitData(CREATURE_MARTIALARTIST, (char*)"Martial Artist", 4),
   recruitData(CREATURE_MUSICIAN, (char*)"Musician", 4),
   recruitData(CREATURE_MUTANT, (char*)"Mutant", 4),
   recruitData(CREATURE_PROSTITUTE, (char*)"Prostitute", 2),
   recruitData(CREATURE_PSYCHOLOGIST, (char*)"Psychologist", 4),
   recruitData(CREATURE_TAXIDRIVER, (char*)"Taxi Driver", 4),
   recruitData(CREATURE_TEACHER, (char*)"Teacher", 4)
};

// Return the difficulty of tracking this character type down, for the
// purpose of the activation menu. 0 is trivial, 10 is impossible.
int recruitFindDifficulty(int creatureType)
{
   for(int i=0; i<(int)(sizeof(recruitable_creatures)/sizeof(recruitData)); i++)
      if(recruitable_creatures[i].type == creatureType)
         return recruitable_creatures[i].difficulty;
   return 10; // No recruitData; assume impossible to recruit
}

char* recruitName(int creatureType)
{
   for(int i=0; i<(int)(sizeof(recruitable_creatures)/sizeof(recruitData)); i++)
      if(recruitable_creatures[i].type == creatureType)
         return recruitable_creatures[i].name;
   return (char*)"missingno";
}

void recruitSelect(Creature &cr)
{
   // Number of recruitable creatures
   int options = sizeof(recruitable_creatures) / sizeof(recruitData);
   for(int i=0; i<options; i++)
   {
      // Dynamic difficulty for certain creatures, recalculated each time the function is called
      if(recruitable_creatures[i].type == CREATURE_MUTANT)
      {
         if(law[LAW_NUCLEARPOWER] != -1 && law[LAW_POLLUTION] != -1)
            recruitable_creatures[i].difficulty = 9;
         else if(law[LAW_NUCLEARPOWER] != -1 && law[LAW_POLLUTION] != -1)
            recruitable_creatures[i].difficulty = 6;
         else
            recruitable_creatures[i].difficulty = 2;
      }
   }

   int page=0;
   //char str[200];
   do
   {
      erase();
      set_color(COLOR_WHITE,COLOR_BLACK,1);
      move(0,0);
      addstr("What type of person will ");
      addstr(cr.name);
      addstr(" try to meet and recruit today?");
      set_color(COLOR_WHITE,COLOR_BLACK,0);
      move(1,0);
      addstr("----TYPE-----------------------------------------DIFFICULTY TO ARRANGE MEETING--");

      int y=2,difficulty;
      for(int p=page*19;p<options&&p<page*19+19;p++)
      {
         set_color(COLOR_WHITE,COLOR_BLACK,0);
         move(y,0);
         addch(y+'A'-2);addstr(" - ");
         addstr(recruitable_creatures[p].name);

         move(y,49);
         difficulty=recruitable_creatures[p].difficulty;
         switch(difficulty)
         {
         case 0:
            set_color(COLOR_GREEN,COLOR_BLACK,1);
            addstr("Simple");
            break;
         case 1:
            set_color(COLOR_CYAN,COLOR_BLACK,1);
            addstr("Very Easy");
            break;
         case 2:
            set_color(COLOR_CYAN,COLOR_BLACK,0);
            addstr("Easy");
            break;
         case 3:
            set_color(COLOR_BLUE,COLOR_BLACK,1);
            addstr("Below Average");
            break;
         case 4:
            set_color(COLOR_WHITE,COLOR_BLACK,1);
            addstr("Average");
            break;
         case 5:
            set_color(COLOR_WHITE,COLOR_BLACK,0);
            addstr("Above Average");
            break;
         case 6:
            set_color(COLOR_YELLOW,COLOR_BLACK,1);
            addstr("Hard");
            break;
         case 7:
            set_color(COLOR_MAGENTA,COLOR_BLACK,0);
            addstr("Very Hard");
            break;
         case 8:
            set_color(COLOR_MAGENTA,COLOR_BLACK,1);
            addstr("Extremely Difficult");
            break;
         case 9:
            set_color(COLOR_RED,COLOR_BLACK,0);
            addstr("Nearly Impossible");
            break;
         default:
            set_color(COLOR_RED,COLOR_BLACK,1);
            addstr("Impossible");
            break;
         }

         y++;
      }

      set_color(COLOR_WHITE,COLOR_BLACK,0);
      move(22,0);
      addstr("Press a Letter to select a Profession");
      move(23,0);
      addpagestr();

      refresh();

      int c=getch();
      translategetch(c);

      //PAGE UP
      if((c==interface_pgup||c==KEY_UP||c==KEY_LEFT)&&page>0)page--;
      //PAGE DOWN
      if((c==interface_pgdn||c==KEY_DOWN||c==KEY_RIGHT)&&(page+1)*19<options)page++;

      if(c>='a'&&c<='s')
      {
         int p=page*19+(int)(c-'a');
         if(p<options)
         {
            cr.activity.type = ACTIVITY_RECRUITING;
            cr.activity.arg = recruitable_creatures[p].type;
            break;
         }
      }
   }while(1);

   return;
}

/* base - activate - make clothing */
void select_makeclothing(Creature *cr)
{
   vector<int> armortypei;
   for (int a = 0; a < (int)armortype.size(); ++a)
   {
      if (armortype[a]->get_make_difficulty() == 0)
         continue;

      if (armortype[a]->deathsquad_legality()
          && (law[LAW_POLICEBEHAVIOR]!=-2 || law[LAW_DEATHPENALTY]!=-2))
         continue;

      armortypei.push_back(a);
   }

   int page=0;

   //char str[200];
   //char num[20];

   do
   {
      erase();

      set_color(COLOR_WHITE,COLOR_BLACK,1);
      move(0,0);
      addstr("Which will ");
      addstr(cr->name);
      addstr(" try to make?   (Note: Half Cost if you have cloth)");
      set_color(COLOR_WHITE,COLOR_BLACK,0);
      move(1,0);
      addstr("----NAME-----------------------------DIFFICULTY-------------COST----------------");

      int y=2,difficulty;
      for(int p=page*19;p<(int)armortypei.size()&&p<page*19+19;p++)
      {
         difficulty=armor_makedifficulty(*armortype[armortypei[p]],cr);
         if(difficulty < 0) difficulty = 0;

         set_color(COLOR_WHITE,COLOR_BLACK,0);
         move(y,0);
         addch(y+'A'-2);addstr(" - ");
         addstr(armortype[armortypei[p]]->get_name().c_str());

         move(y,37);
         switch(difficulty)
         {
         case 0:
            set_color(COLOR_GREEN,COLOR_BLACK,1);
            addstr("Simple");
            break;
         case 1:
            set_color(COLOR_CYAN,COLOR_BLACK,1);
            addstr("Very Easy");
            break;
         case 2:
            set_color(COLOR_CYAN,COLOR_BLACK,0);
            addstr("Easy");
            break;
         case 3:
            set_color(COLOR_BLUE,COLOR_BLACK,1);
            addstr("Below Average");
            break;
         case 4:
            set_color(COLOR_WHITE,COLOR_BLACK,1);
            addstr("Average");
            break;
         case 5:
            set_color(COLOR_WHITE,COLOR_BLACK,0);
            addstr("Above Average");
            break;
         case 6:
            set_color(COLOR_YELLOW,COLOR_BLACK,1);
            addstr("Hard");
            break;
         case 7:
            set_color(COLOR_MAGENTA,COLOR_BLACK,0);
            addstr("Very Hard");
            break;
         case 8:
            set_color(COLOR_MAGENTA,COLOR_BLACK,1);
            addstr("Extremely Difficult");
            break;
         case 9:
            set_color(COLOR_RED,COLOR_BLACK,0);
            addstr("Nearly Impossible");
            break;
         default:
            set_color(COLOR_RED,COLOR_BLACK,1);
            addstr("Impossible");
            break;
         }

         set_color(COLOR_GREEN,COLOR_BLACK,1);
         string price = tostring(armortype[armortypei[p]]->get_make_price());
         move(y,63-price.length());
         addch('$');addstr(price.c_str());

         y++;
      }

      set_color(COLOR_WHITE,COLOR_BLACK,0);
      move(22,0);
      addstr("Press a Letter to select a Type of Clothing");
      move(23,0);
      addpagestr();

      refresh();

      int c=getch();
      translategetch(c);

      //PAGE UP
      if((c==interface_pgup||c==KEY_UP||c==KEY_LEFT)&&page>0)page--;
      //PAGE DOWN
      if((c==interface_pgdn||c==KEY_DOWN||c==KEY_RIGHT)&&(page+1)*19<(int)armortype.size())page++;

      if(c>='a'&&c<='s')
      {
         int p=page*19+(int)(c-'a');
         if(p<(int)armortypei.size())
         {
            cr->activity.type=ACTIVITY_MAKE_ARMOR;
            cr->activity.arg=armortypei[p]; //Use id name of armor type instead? -XML
            return;
         }
      }

      if(c==10||c==ESC)break;
   }while(1);
}

int armor_makedifficulty(Armor& type, Creature *cr)
{
   return armor_makedifficulty(*armortype[getarmortype(type.get_itemtypename())], cr);
}

int armor_makedifficulty(ArmorType& type,Creature *cr) //Make class method? -XML
{
   long basedif = type.get_make_difficulty();

   basedif-=cr->get_skill(SKILL_TAILORING)-3;
   if(basedif<0)basedif=0;

   return basedif;
}


/* base - activate - trouble */
long select_troublefundinglevel(Creature *cr)
{
   long flevel=-1;

   erase();

   set_color(COLOR_WHITE,COLOR_BLACK,0);
   printfunds(0,1,"Money: ");
   move(0,0);
   addstr("Select a Funding Level for this Operation:");

   move(2,0);
   addstr("A - Don't spend anything.  ");
   addstr(cr->name);
   addstr(" just needs something constructive to do.");

   move(3,0);
   addstr("B - $20 per day.  Some minor purchases are needed.");

   move(4,0);
   addstr("C - $50 per day.  Disobedience costs money.");

   move(5,0);
   addstr("D - $100 per day.  Enough to be really disobedient.");

   move(6,0);
   addstr("E - $500 per day.  The Machine will buckle under the weight of");
   move(7,0);
   addstr("    ");
   addstr(cr->name);
   addstr("'s Numerous and Varied Liberal Acts.");

   move(9,0);
   addstr("Enter - On second thought, this isn't a job for ");
   addstr(cr->name);
   addstr(".");

   refresh();
   int c=getch();
   translategetch(c);

   if(c=='a')flevel=0;
   if(c=='b')flevel=20;
   if(c=='c')flevel=50;
   if(c=='d')flevel=100;
   if(c=='e')flevel=500;

   return flevel;
}



/* base - activate - select a topic to write about */
char select_view(Creature *cr,int &v)
{
   int page=0;
   char str[80];

   do
   {
      erase();

      set_color(COLOR_WHITE,COLOR_BLACK,1);
      move(0,0);
      addstr("Write a news story if the LCS makes the news on the selected topic today, or");
      move(1,0);
      addstr("write editorials if there is no current news but there is public interest.");
      set_color(COLOR_WHITE,COLOR_BLACK,0);
      move(2,0);
      addstr("----TOPIC-----------------------------------INTEREST---------------------------");

      int y=3,x=0;
      for(int p=page*18;p<VIEWNUM-3&&p<page*18+18;p++)
      {
         set_color(COLOR_WHITE,COLOR_BLACK,0);
         move(y,x);
         addch((p-page*18)+'A');addstr(" - ");
         getview(str,p);
         addstr(str);

         move(y,44);
         if(public_interest[p]>100)
         {
            set_color(COLOR_RED,COLOR_BLACK,1);
            addstr("Extremely Controversial");
         }
         else if(public_interest[p]>50)
         {
            set_color(COLOR_YELLOW,COLOR_BLACK,1);
            addstr("Dinner Table Topic");
         }
         else if(public_interest[p]>10)
         {
            set_color(COLOR_WHITE,COLOR_BLACK,1);
            addstr("Significant Interest");
         }
         else if(public_interest[p]>0)
         {
            set_color(COLOR_WHITE,COLOR_BLACK,0);
            addstr("Minor Discussion");
         }
         else
         {
            set_color(COLOR_BLACK,COLOR_BLACK,1);
            addstr("Exhausted");
         }

         y++;
      }

      set_color(COLOR_WHITE,COLOR_BLACK,0);
      move(22,0);
      addstr("Press a Letter to select a Topic");
      move(23,0);
      addpagestr();

      refresh();

      int c=getch();
      translategetch(c);

      //PAGE UP
      if((c==interface_pgup||c==KEY_UP||c==KEY_LEFT)&&page>0)page--;
      //PAGE DOWN
      if((c==interface_pgdn||c==KEY_DOWN||c==KEY_RIGHT)&&(page+1)*16<VIEWNUM-3)page++;

      if(c>='a'&&c<='a'+18)
      {
         int p=page*18+(int)(c-'a');
         if(p<VIEWNUM-3)
         {
            v=p;
            return 1;
         }
      }

      if(c==10||c==ESC)break;
   }while(1);

   return 0;
}


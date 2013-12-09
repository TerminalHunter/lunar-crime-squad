/*
This file is part of Liberal Crime Squad.                                           //
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

/* monthly - sleeper behavior */

#include <includes.h>
#include <externs.h>

/**********************************************************************
** *JDS*
** ----- The sleeper system has been completely reworked.
** - Sleepers no longer directly influence the issues. They now affect
** the broad "liberal power" stats across many issues, which are used
** as a kind of monthly liberal roll akin to AM Radio and Cable News.
** - Each sleeper can affect one or more issue, throwing their power
** into the "abstracted debate" on that issue.
** - After all of the sleepers have contributed to the liberal power
** stats, a roll is made on each issue to see whether the liberals
** make background progress on those issues. 
** - Several sleepers have special abilities. Lawyers and Judges, as
** always, can aid your people in the legal system. Police officers,
** corporate managers, CEOs, and agents can all now leak secret
** documents of the appropriate types, and they will make a check
** each month. This will only happen if the homeless shelter is not
** under siege, and "canseethings" is enabled (eg, you're not in prison
** or disbanded or some other situation where your sleeper can't get
** in touch with anyone in your squad).
** - News Anchors and Radio Personalities remain the two most powerful
** sleepers.
**********************************************************************/
void sleepereffect(Creature &cr,char &clearformess,char canseethings,int *libpower)
{
   if(disbanding)cr.activity.type = ACTIVITY_SLEEPER_LIBERAL;
   
   switch(cr.activity.type)
   {
      case ACTIVITY_SLEEPER_CONSERVATIVE:
      case ACTIVITY_SLEEPER_LIBERAL:
         sleeper_influence(cr,clearformess,canseethings,libpower);
         break;
      case ACTIVITY_SLEEPER_EMBEZZLE:
         sleeper_embezzle(cr,clearformess,canseethings,libpower);
         break;
      case ACTIVITY_SLEEPER_STEAL:
         sleeper_steal(cr,clearformess,canseethings,libpower);
         break;
      case ACTIVITY_SLEEPER_RECRUIT:
         sleeper_recruit(cr,clearformess,canseethings,libpower);
         break;
      case ACTIVITY_SLEEPER_SPY:
         sleeper_spy(cr,clearformess,canseethings,libpower);
         break;
      case ACTIVITY_SLEEPER_SCANDAL:
         sleeper_scandal(cr,clearformess,canseethings,libpower);
         break;
      case ACTIVITY_NONE:
         // Improves infiltration
         if(cr.infiltration<0.4)
         {
            cr.infiltration+=0.02f;
            if(cr.infiltration>0.4)
               cr.infiltration=0.4f;
         }
         break;
      case ACTIVITY_SLEEPER_JOINLCS:
      default:
         break;
   }
}


/*********************************
**
**   SLEEPERS INFLUENCING
**     PUBLIC OPINION
**
**********************************/
void sleeper_influence(Creature &cr,char &clearformess,char canseethings,int *libpower)
{
   int power=(cr.attval(ATTRIBUTE_CHARISMA)+
               cr.attval(ATTRIBUTE_HEART)+
               cr.attval(ATTRIBUTE_INTELLIGENCE)+
               cr.skillval(SKILL_PERSUASION));

   // Profession specific skills
   switch(cr.type)
   {
      case CREATURE_CRITIC_ART:
         power+=cr.skillval(SKILL_WRITING);
      case CREATURE_PAINTER:
      case CREATURE_SCULPTOR:
         power+=cr.skillval(SKILL_ART);
         break;
      case CREATURE_CRITIC_MUSIC:
         power+=cr.skillval(SKILL_WRITING);
      case CREATURE_MUSICIAN:
         power+=cr.skillval(SKILL_MUSIC);
         break;
      case CREATURE_AUTHOR:
      case CREATURE_JOURNALIST:
         power+=cr.skillval(SKILL_WRITING);
         break;
      case CREATURE_JUDGE_CONSERVATIVE:
         power+=cr.skillval(SKILL_WRITING);
      case CREATURE_LAWYER:
         power+=cr.skillval(SKILL_LAW);
         break;
      case CREATURE_SCIENTIST_LABTECH:
      case CREATURE_SCIENTIST_EMINENT:
         power+=cr.skillval(SKILL_SCIENCE);
         break;
      case CREATURE_CORPORATE_CEO:
      case CREATURE_CORPORATE_MANAGER:
         power+=cr.skillval(SKILL_BUSINESS);
         break;
      case CREATURE_PRIEST:
      case CREATURE_NUN:
         power+=cr.skillval(SKILL_RELIGION);
         break;
      case CREATURE_EDUCATOR:
         power+=cr.skillval(SKILL_PSYCHOLOGY);
         break;
   }

   // Adjust power for super sleepers
   switch(cr.type)
   {
      case CREATURE_CORPORATE_CEO:
      case CREATURE_DEATHSQUAD:
      case CREATURE_EDUCATOR:
         power*=4;
         break;
      case CREATURE_SCIENTIST_EMINENT:
      case CREATURE_ACTOR:
      case CREATURE_GANGUNIT:
         power*=3;
         break;
      default:
         power*=2;
         break;
   }

   int homes=-1; // find homeless shelter
   for(int l=0;l<location.size();l++)
   {
      if(location[l]->type==SITE_RESIDENTIAL_SHELTER)
      {
         homes=l;
      }
   }

   // Advocating Conservative views should have negative effects on popular opinion
   switch(cr.activity.type)
   {
      case ACTIVITY_SLEEPER_CONSERVATIVE:
      {
         power = static_cast<int>(-power * (1-cr.infiltration))/4;
         // Improves infiltration
         if(cr.infiltration<0.6)
         {
            cr.infiltration+=0.05f;
            if(cr.infiltration>0.6)
               cr.infiltration=0.6f;
         }
      }
      break;
      case ACTIVITY_SLEEPER_LIBERAL:
      {
         power = static_cast<int>(power * cr.infiltration);
      }
      break;
   }

   switch(cr.type)
   {
      /* Cultural leaders block - small influence on everything */
	  /*Radio Personalities and News Anchors subvert Conservative news stations and convince people to watch
	  Liberal news stations. But once people watch Liberal news, nobody will want to listen to the
	  Radio Personalities and News Anchors anymore! They lose their ability to change Culture!*/
      case CREATURE_RADIOPERSONALITY:
		 if(attitude[VIEW_AMRADIO]<=50)
		 {
         change_public_opinion(VIEW_AMRADIO,1);
         for(int i=0;i<VIEWNUM-3;i++)
         {
            libpower[i]+=power/2;
         }
		 }
		 else
		 {
		libpower[VIEW_LIBERALCRIMESQUAD]+=power;
         libpower[VIEW_LIBERALCRIMESQUADPOS]+=power;
		 }
		 break;
      case CREATURE_NEWSANCHOR:
		  if(attitude[VIEW_CABLENEWS]<=50)
		  {
         change_public_opinion(VIEW_CABLENEWS,1);
         for(int i=0;i<VIEWNUM-3;i++)
         {
            libpower[i]+=power/2;
         }
		  }
		  else
		  {
			 libpower[VIEW_LIBERALCRIMESQUAD]+=power;
			 libpower[VIEW_LIBERALCRIMESQUADPOS]+=power;
		  }
         break;
		 /* Priests gain their cultural power through their ability to "creatively interpert" religion to promote
		 Liberal ideas, such as the seperation of church and state. But if the government strictly 
		 enforce the seperation of church and state (L+), prohibiting faith-based views from affecting public policy,
		 then Priests would be unable to use their Religious preachings to change the Culture of society!*/
      case CREATURE_PRIEST:
		  if(law[LAW_RELIGION]<=1)
		  {
			  change_public_opinion(VIEW_RELIGION,1);
			  for(int i=0;i<VIEWNUM-3;i++)
         {
            libpower[i]+=power/2;
         }
		  }
		  else
		  {
			 libpower[VIEW_LIBERALCRIMESQUAD]+=power;
			 libpower[VIEW_LIBERALCRIMESQUADPOS]+=power;
		  }
         break;
		 /* All these other Cultural Sleepers are inherently good at affecting culture, in fact, they get to produce
		 Culture! Nothing can be done to strip them of their power. */
      case CREATURE_PAINTER:
      case CREATURE_SCULPTOR:
      case CREATURE_AUTHOR:
      case CREATURE_JOURNALIST:
      case CREATURE_MUSICIAN:
      case CREATURE_CRITIC_ART:
      case CREATURE_CRITIC_MUSIC:
      case CREATURE_ACTOR:
	  case CREATURE_COMMISSAR:
         for(int i=0;i<VIEWNUM-3;i++)
         {
            libpower[i]+=power/2;
         }
         break;
      /* Legal block - influences an array of social issues */
      case CREATURE_LAWYER:
      case CREATURE_JUDGE_CONSERVATIVE:
         libpower[VIEW_WOMEN]+=power;
         libpower[VIEW_CIVILRIGHTS]+=power;
         libpower[VIEW_GAY]+=power;
         libpower[VIEW_DEATHPENALTY]+=power;
         libpower[VIEW_FREESPEECH]+=power;
         libpower[VIEW_JUSTICES]+=power;
         libpower[VIEW_INTELLIGENCE]+=power;
         libpower[VIEW_ANIMALRESEARCH]+=power;
		 libpower[VIEW_WELFARE]+=power;
         break;
      /* Scientists block */
      case CREATURE_SCIENTIST_LABTECH:
      case CREATURE_SCIENTIST_EMINENT:
         libpower[VIEW_NUCLEARPOWER]+=power;
         libpower[VIEW_ANIMALRESEARCH]+=power;
         libpower[VIEW_GENETICS]+=power;
         break;
      /* Corporate block */
      case CREATURE_CORPORATE_CEO:
      case CREATURE_CORPORATE_MANAGER:
	  case CREATURE_REDGUARD:
         libpower[VIEW_CEOSALARY]+=power;
         libpower[VIEW_TAXES]+=power;
         libpower[VIEW_CORPORATECULTURE]+=power;
         break;
      /* Law enforcement block */
      case CREATURE_DEATHSQUAD:
	  case CREATURE_CHEKA:
         libpower[VIEW_DEATHPENALTY]+=power; // No break
      case CREATURE_SWAT:
      case CREATURE_COP:
      case CREATURE_GANGUNIT:
         libpower[VIEW_POLICEBEHAVIOR]+=power;
         libpower[VIEW_DRUGS]+=power;
         break;
      /* Prison block */
      case CREATURE_EDUCATOR:
         libpower[VIEW_DEATHPENALTY]+=power; // no break intended
      case CREATURE_PRISONGUARD:
      case CREATURE_PRISONER:
         libpower[VIEW_PRISONS]+=power;
         libpower[VIEW_DRUGS]+=power;
         break;
      /* Intelligence block */
      case CREATURE_AGENT:
         libpower[VIEW_INTELLIGENCE]+=power;
         break;
      /* Military block */
      case CREATURE_SOLDIER:
      case CREATURE_VETERAN:
      case CREATURE_MERC:
         libpower[VIEW_MILITARY]+=power;
         break;
      /* Sweatshop workers */
      case CREATURE_WORKER_SWEATSHOP:
         libpower[VIEW_IMMIGRATION]+=power;
         libpower[VIEW_SWEATSHOPS]+=power;
         break;
      /* No influence at all block - for people were liberal anyway, or have no way of doing any good */
      case CREATURE_WORKER_FACTORY_CHILD:
      case CREATURE_GENETIC:
      case CREATURE_GUARDDOG:
      case CREATURE_BUM:
      case CREATURE_CRACKHEAD:
      case CREATURE_TANK:
      case CREATURE_HIPPIE: // too liberal to be a proper sleeper
      case CREATURE_WORKER_FACTORY_UNION: // same
      case CREATURE_JUDGE_LIBERAL: // more again
      case CREATURE_POLITICALACTIVIST: // ??!?!? impressive getting an LCS Member sleeper, but no effect
      case CREATURE_MUTANT:
         return;
      /* Talk up LCS block -- includes everyone else */
      case CREATURE_FIREFIGHTER:
         if(law[LAW_FREESPEECH]==-2)
         {
            libpower[VIEW_FREESPEECH]+=power;
            break;
         }
      default:
         libpower[VIEW_LIBERALCRIMESQUAD]+=power;
         libpower[VIEW_LIBERALCRIMESQUADPOS]+=power;
   }
}


/*********************************
**
**   SLEEPERS SNOOPING AROUND
**
**********************************/
void sleeper_spy(Creature &cr,char &clearformess,char canseethings,int *libpower)
{
   // Improves juice, as confidence improves
   if(cr.juice<100)
   {
      cr.juice += 2;
      if(cr.juice>100)cr.juice=100;
   }
   if(LCSrandom(static_cast<int>(10*cr.infiltration)+1) <= 0)return;

   location[cr.base]->interrogated = 1;

   int homes;
   for(homes=0;homes<location.size();homes++)
   {
      if(location[homes]->type == SITE_RESIDENTIAL_SHELTER)break;
   }

   bool pause=false;
   switch(cr.type)
   {
   case CREATURE_AGENT:
      // Agents can leak intelligence files to you
      if(!location[homes]->siege.siege&&canseethings) 
      {
         if(LCSrandom(law[LAW_PRIVACY] + 3)) break;
         itemst *it=new itemst;
         it->type=ITEM_LOOT;
         it->loottype=LOOT_SECRETDOCUMENTS;
         location[homes]->loot.push_back(it);

         erase();
         move(6,1);
         addstr("Sleeper ");
         addstr(cr.name);
         addstr(" has leaked secret intelligence files.");
         move(7,1);
         addstr("They are stashed at the homeless shelter.");
         pause=true;
      }
      break;
   case CREATURE_DEATHSQUAD:
   case CREATURE_SWAT:
   case CREATURE_COP:
   case CREATURE_GANGUNIT:
      // Cops can leak police files to you
      if(!location[homes]->siege.siege&&canseethings)
      {
         if(LCSrandom(law[LAW_POLICEBEHAVIOR] + 3)) break;
         itemst *it=new itemst;
         it->type=ITEM_LOOT;
         it->loottype=LOOT_POLICERECORDS;
         location[homes]->loot.push_back(it);

         erase();
         move(6,1);
         addstr("Sleeper ");
         addstr(cr.name);
         addstr(" has leaked secret police records.");
         move(7,1);
         addstr("They are stashed at the homeless shelter.");
         pause=true;
      }
      break;
   case CREATURE_CORPORATE_MANAGER:
   case CREATURE_CORPORATE_CEO:
      // Can leak corporate files to you
      if(!location[homes]->siege.siege&&canseethings) 
      {
         if(LCSrandom(law[LAW_CORPORATE] + 3)) break;
         itemst *it=new itemst;
         it->type=ITEM_LOOT;
         it->loottype=LOOT_CORPFILES;
         location[homes]->loot.push_back(it);

         erase();
         move(6,1);
         addstr("Sleeper ");
         addstr(cr.name);
         addstr(" has leaked secret corporate documents.");
         move(7,1);
         addstr("They are stashed at the homeless shelter.");
         pause=true;
      }
      break;
   case CREATURE_EDUCATOR:
   case CREATURE_PRISONGUARD:
      if(!location[homes]->siege.siege&&canseethings) 
      {
         if(LCSrandom(law[LAW_POLICEBEHAVIOR] + 3)) break;
         itemst *it=new itemst;
         it->type=ITEM_LOOT;
         it->loottype=LOOT_PRISONFILES;
         location[homes]->loot.push_back(it);

         erase();
         move(6,1);
         addstr("Sleeper ");
         addstr(cr.name);
         addstr(" has leaked internal prison records.");
         move(7,1);
         addstr("They are stashed at the homeless shelter.");
         pause=true;
      }
      break;
   case CREATURE_NEWSANCHOR:
      if(!location[homes]->siege.siege&&canseethings) 
      {
         // More likely to leak these documents the more restrictive
         // free speech is -- because the more free the society, the
         // less any particular action the media takes seems scandalous
         if(LCSrandom(law[LAW_FREESPEECH] + 3)) break;
         itemst *it=new itemst;
         it->type=ITEM_LOOT;
         it->loottype=LOOT_CABLENEWSFILES;
         location[homes]->loot.push_back(it);

         erase();
         move(6,1);
         addstr("Sleeper ");
         addstr(cr.name);
         addstr(" has leaked proof of systemic Cable News bias.");
         move(7,1);
         addstr("The papers are stashed at the homeless shelter.");
         pause=true;
      }
      break;
   case CREATURE_RADIOPERSONALITY:
      if(!location[homes]->siege.siege&&canseethings) 
      {
         // More likely to leak these documents the more restrictive
         // free speech is -- because the more free the society, the
         // less any particular action the media takes seems scandalous
         if(LCSrandom(law[LAW_FREESPEECH] + 3)) break;
         itemst *it=new itemst;
         it->type=ITEM_LOOT;
         it->loottype=LOOT_AMRADIOFILES;
         location[homes]->loot.push_back(it);

         erase();
         move(6,1);
         addstr("Sleeper ");
         addstr(cr.name);
         addstr(" has leaked proof of systemic AM Radio bias.");
         move(7,1);
         addstr("The papers are stashed at the homeless shelter.");
         pause=true;
      }
      break;
   case CREATURE_SCIENTIST_LABTECH:
   case CREATURE_SCIENTIST_EMINENT:
      if(!location[homes]->siege.siege&&canseethings) 
      {
         if(LCSrandom(law[LAW_ANIMALRESEARCH] + 3)) break;
         itemst *it=new itemst;
         it->type=ITEM_LOOT;
         it->loottype=LOOT_RESEARCHFILES;
         location[homes]->loot.push_back(it);

         erase();
         move(6,1);
         addstr("Sleeper ");
         addstr(cr.name);
         addstr(" has leaked internal animal research reports.");
         move(7,1);
         addstr("They are stashed at the homeless shelter.");
         pause=true;
      }
      break;
   case CREATURE_JUDGE_CONSERVATIVE:
      if(!location[homes]->siege.siege&&canseethings) 
      {
         if(LCSrandom(5)) break;
         itemst *it=new itemst;
         it->type=ITEM_LOOT;
         it->loottype=LOOT_JUDGEFILES;
         location[homes]->loot.push_back(it);

         erase();
         move(6,1);
         addstr("Sleeper ");
         addstr(cr.name);
         addstr(" has leaked proof of corruption in the judiciary.");
         move(7,1);
         addstr("The papers are stashed at the homeless shelter.");
         pause=true;
      }
      break;
   }
   if(pause)
   {
      refresh();
      getch();
   }
}

/*********************************
**
**   SLEEPERS EMBEZZLING FUNDS
**
**********************************/
void sleeper_embezzle(Creature &cr,char &clearformess,char canseethings,int *libpower)
{
   // Improves juice, as confidence improves
   if(cr.juice<100)
   {
      cr.juice += 3;
      if(cr.juice>100)cr.juice=100;
   }
   if(cr.infiltration < 0 ||
      !LCSrandom(static_cast<int>(cr.infiltration*40)+1))
   {
      erase();
      move(6,1);
      addstr("Sleeper ");
      addstr(cr.name);
      addstr(" has been caught embezzling funds for the LCS.");
      move(8,1);
      addstr("The LCS has no choice but to cut the sleeper loose.");
      
      refresh();
      getch();
      for(int p=0;p<pool.size();p++)
      {
         if(pool[p]->id == cr.id)
         {
            delete pool[p];
            pool.erase(pool.begin() + p);
            break;
         }
      }
      return;
   }
   switch(cr.type)
   {
      case CREATURE_SCIENTIST_EMINENT:
         funds+=static_cast<int>(3000*cr.infiltration);
         moneygained_embezzlement+=static_cast<int>(3000*cr.infiltration);
         break;
      case CREATURE_CORPORATE_CEO:
         funds+=static_cast<int>(10000*cr.infiltration);
         moneygained_embezzlement+=static_cast<int>(10000*cr.infiltration);
         break;
      case CREATURE_CORPORATE_MANAGER:
         funds+=static_cast<int>(2000*cr.infiltration);
         moneygained_embezzlement+=static_cast<int>(2000*cr.infiltration);
         break;
      case CREATURE_AGENT:
         funds+=static_cast<int>(1000*cr.infiltration);
         moneygained_embezzlement+=static_cast<int>(1000*cr.infiltration);
         break;
      default:
         funds+=static_cast<int>(100*cr.infiltration);
         moneygained_embezzlement+=static_cast<int>(100*cr.infiltration);
         return;
   }
}

/*********************************
**
**   SLEEPERS STEALING THINGS
**
**********************************/
void sleeper_steal(Creature &cr,char &clearformess,char canseethings,int *libpower)
{
   // Improves juice, as confidence improves
   if(cr.juice<100)
   {
      cr.juice += 3;
      if(cr.juice>100)cr.juice=100;
   }
   if(cr.infiltration < 0 || !LCSrandom(static_cast<int>(cr.infiltration*20+1)))
   {
      erase();
      move(6,1);
      addstr("Sleeper ");
      addstr(cr.name);
      addstr(" has been caught stealing things for the LCS.");
      move(8,1);
      addstr("The LCS has no choice but to cut the sleeper loose.");
      refresh();
      getch();
      for(int p=0;p<pool.size();p++)
      {
         if(pool[p]->id == cr.id)
         {
            delete pool[p];
            pool.erase(pool.begin() + p);
            break;
         }
      }
      return;
   }
   
   itemst *item;
   locationst *shelter=0;

   for(int l=0;l<location.size();l++)
   {
      if(location[l]->type == SITE_RESIDENTIAL_SHELTER)
      {
         shelter = location[l];
         break;
      }
   }

   int number_of_items = LCSrandom(10)+1;

   while(number_of_items--)
   {
      switch(location[cr.location]->type)
      {
      case SITE_RESIDENTIAL_TENEMENT:
         item=new itemst;item->type=ITEM_LOOT;
            item->loottype=LOOT_TV;
         shelter->loot.push_back(item);
         break;
      default:
      case SITE_RESIDENTIAL_APARTMENT:
      case SITE_RESIDENTIAL_APARTMENT_UPSCALE:
         item=new itemst;item->type=ITEM_LOOT;
            if(!LCSrandom(5))item->loottype=LOOT_CELLPHONE;
            else if(!LCSrandom(4))item->loottype=LOOT_SILVERWARE;
            else if(!LCSrandom(3))item->loottype=LOOT_PRINTER;
            else if(!LCSrandom(2))item->loottype=LOOT_TV;
            else item->loottype=LOOT_COMPUTER;
         shelter->loot.push_back(item);
         break;
      case SITE_LABORATORY_COSMETICS:
      case SITE_INDUSTRY_NUCLEAR:
      case SITE_LABORATORY_GENETIC:
         item=new itemst;item->type=ITEM_LOOT;
            if(!LCSrandom(2))item->loottype=LOOT_LABEQUIPMENT;
            else if(!LCSrandom(2))item->loottype=LOOT_TV;
            else if(!LCSrandom(5))item->loottype=LOOT_SCANNER;
            else if(!LCSrandom(3))item->loottype=LOOT_PRINTER;
            else if(!LCSrandom(5))item->loottype=LOOT_CHEMICAL;
            else item->loottype=LOOT_COMPUTER;
         shelter->loot.push_back(item);
         break;
      case SITE_GOVERNMENT_POLICESTATION:
         item=new itemst;
         if(!LCSrandom(3))
         {
            item->type=ITEM_WEAPON;
            if(!LCSrandom(4))item->loottype=WEAPON_SMG_MP5;
            else if(!LCSrandom(3))item->loottype=WEAPON_SEMIPISTOL_45;
            else if(!LCSrandom(2))item->loottype=WEAPON_SHOTGUN_PUMP;
            else item->loottype=WEAPON_SEMIRIFLE_AR15;
         }
         else if(!LCSrandom(2))
         {
            item->type=ITEM_ARMOR;
            if(!LCSrandom(3))item->loottype=ARMOR_POLICEUNIFORM;
            else if(!LCSrandom(2))item->loottype=ARMOR_SWATARMOR;
            else item->loottype=ARMOR_POLICEARMOR;
         }
         else
         {
            item=new itemst;item->type=ITEM_LOOT;
            if(!LCSrandom(3))item->loottype=LOOT_CELLPHONE;
            else if(!LCSrandom(2))item->loottype=LOOT_TV;
            else item->loottype=LOOT_COMPUTER;
         }
         shelter->loot.push_back(item);
         break;
      case SITE_GOVERNMENT_COURTHOUSE:
         item=new itemst;item->type=ITEM_LOOT;
            if(!LCSrandom(2))item->loottype=LOOT_TV;
            else item->loottype=LOOT_COMPUTER;
         shelter->loot.push_back(item);
         break;
      case SITE_GOVERNMENT_PRISON:
         item=new itemst;item->type=ITEM_WEAPON;
         item->weapon.type=WEAPON_SHANK;
         item->weapon.ammo=0;
         shelter->loot.push_back(item);
         break;
      case SITE_GOVERNMENT_INTELLIGENCEHQ:
         if(!LCSrandom(3))
         {
            item->type=ITEM_WEAPON;
            if(!LCSrandom(4))item->loottype=WEAPON_SMG_MP5;
            else if(!LCSrandom(3))item->loottype=WEAPON_AUTORIFLE_M16;
            else if(!LCSrandom(2))item->loottype=WEAPON_SHOTGUN_PUMP;
            else item->loottype=WEAPON_CARBINE_M4;
         }
         else if(!LCSrandom(2))
         {
            item->type=ITEM_ARMOR;
            item->loottype=ARMOR_BLACKSUIT;
         }
         else
         {
            item=new itemst;item->type=ITEM_LOOT;
            if(!LCSrandom(3))item->loottype=LOOT_CELLPHONE;
            else if(!LCSrandom(2))item->loottype=LOOT_TV;
            else item->loottype=LOOT_COMPUTER;
         }
         shelter->loot.push_back(item);
         break;
      case SITE_GOVERNMENT_FIRESTATION:
         item=new itemst;item->type=ITEM_LOOT;
            if(LCSrandom(2))item->loottype=LOOT_TV;
            else item->loottype=LOOT_COMPUTER;
         shelter->loot.push_back(item);
         break;
      case SITE_INDUSTRY_SWEATSHOP:
         item=new itemst;item->type=ITEM_LOOT;
            item->loottype=LOOT_FINECLOTH;
         shelter->loot.push_back(item);
         break;
      case SITE_INDUSTRY_POLLUTER:
         item=new itemst;item->type=ITEM_LOOT;
            if(!LCSrandom(4))item->loottype=LOOT_PRINTER;
            else if(!LCSrandom(3))item->loottype=LOOT_TV;
            else if(!LCSrandom(2))item->loottype=LOOT_CHEMICAL;
            else item->loottype=LOOT_COMPUTER;
         shelter->loot.push_back(item);
         break;
      case SITE_CORPORATE_HEADQUARTERS:
         item=new itemst;item->type=ITEM_LOOT;
            if(!LCSrandom(4))item->loottype=LOOT_PRINTER;
            else if(!LCSrandom(3))item->loottype=LOOT_TV;
            else if(!LCSrandom(2))item->loottype=LOOT_CELLPHONE;
            else item->loottype=LOOT_COMPUTER;
         shelter->loot.push_back(item);
         break;
      case SITE_CORPORATE_HOUSE:
         item=new itemst;item->type=ITEM_LOOT;
            if(!LCSrandom(5))item->loottype=LOOT_CELLPHONE;
            else if(!LCSrandom(4))item->loottype=LOOT_SILVERWARE;
            else if(!LCSrandom(3))item->loottype=LOOT_PRINTER;
            else if(!LCSrandom(2))item->loottype=LOOT_TV;
            else item->loottype=LOOT_COMPUTER;
         shelter->loot.push_back(item);
         break;
      case SITE_MEDIA_AMRADIO:
         item=new itemst;item->type=ITEM_LOOT;
            if(!LCSrandom(5))item->loottype=LOOT_CELLPHONE;
            else if(!LCSrandom(4))item->loottype=LOOT_BROADCASTINGEQUIPMENT;
            else if(!LCSrandom(3))item->loottype=LOOT_PRINTER;
            else if(!LCSrandom(2))item->loottype=LOOT_TV;
            else item->loottype=LOOT_COMPUTER;
         shelter->loot.push_back(item);
         break;
      case SITE_MEDIA_CABLENEWS:
         item=new itemst;item->type=ITEM_LOOT;
            if(!LCSrandom(5))item->loottype=LOOT_CELLPHONE;
            else if(!LCSrandom(4))item->loottype=LOOT_TVCAMERA;
            else if(!LCSrandom(3))item->loottype=LOOT_PRINTER;
            else if(!LCSrandom(2))item->loottype=LOOT_TV;
            else item->loottype=LOOT_COMPUTER;
         shelter->loot.push_back(item);
         break;
      }
   }
   erase();
   set_color(COLOR_WHITE,COLOR_BLACK,0);   move(6,1);
   addstr("Sleeper ");
   addstr(cr.name);
   addstr(" has dropped a package off at the homeless shelter.");
   refresh();
   getch();
}


/*********************************
**
**   SLEEPERS CREATING SCANDALS
**
**********************************/
void sleeper_scandal(Creature &cr,char &clearformess,char canseethings,int *libpower)
{
   // Add content here!
   return;
}


/*********************************
**
**   SLEEPERS RECRUITING
**
**********************************/
void sleeper_recruit(Creature &cr,char &clearformess,char canseethings,int *libpower)
{
   if(subordinatesleft(cr))
   {
      prepareencounter(location[cr.worklocation]->type,0);
      for(int e=0;e<18;e++)
      {
         if(encounter[e].exists == false)
            break;
         if(encounter[e].worklocation == cr.worklocation || !LCSrandom(5))
         {
            if(encounter[e].align!=1&&LCSrandom(5))continue;

            Creature* recruit = new Creature(encounter[e]);
            liberalize(*recruit,0);
            recruit->namecreature();
            recruit->hireid = cr.id;
            if(recruit->infiltration > cr.infiltration)
            {
               recruit->infiltration = cr.infiltration;
            }
            recruit->flag |= CREATUREFLAG_SLEEPER;
            location[recruit->worklocation]->interrogated=1;
            location[recruit->worklocation]->hidden=0;
            pool.push_back(recruit);

            erase();
            move(6,1);
            addstr("Sleeper ");
            addstr(cr.name);
            addstr(" has recruited a new ");
            char str[50];
            getrecruitcreature(str,recruit->type);
            addstr(str);
            addstr(".");
            move(8,1);
            addstr(recruit->name);
            addstr(" looks forward serving the Liberal cause!");
            refresh();
            getch();
            if(!subordinatesleft(cr))cr.activity.type = ACTIVITY_NONE;
            stat_recruits++;
            break;
         }
      }
   }
   return;
}
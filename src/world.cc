/*
 * Copyright (C) 2002 by the Widelands Development Team
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include "widelands.h"
#include "world.h"
#include "bob.h"
#include "worlddata.h"
#include "md5.h"
#include "profile.h"


static void sec_missing(const char *section)
{
	throw wexception("Section '%s' missing", section);
}

static void key_missing(const char *section, const char *key)
{
	throw wexception("Section %s: key '%s' missing", section, key);
}

void Resource_Descr::parse(Section *s)
{
	snprintf(name, sizeof(name), "%s", s->get_name());
   minh = s->get_int("min_height", 0);
   maxh = s->get_int("max_height", 255);
   importance = s->get_int("importance", 0);
}

// 
// class World
// 
World::World(void)
{
}

World::~World(void) {

}

// 
// This loads a sane world file
//
void World::load_world(const char* name)
{
	char directory[256];

	try
	{	
		snprintf(directory, sizeof(directory), "worlds/%s", name);
		
		parse_root_conf(directory, name);
		parse_resources(directory);
		parse_terrains(directory);
		parse_bobs(directory);		
	}
	catch(std::exception &e)
	{
		// tag with world name
		throw wexception("Error loading world %s: %s", name, e.what());
	}
} 

// 
// down here: Private functions for loading
// 

//
// read the <world-directory>/conf
//
void World::parse_root_conf(const char *directory, const char *name)
{
	char fname[256];
	
	snprintf(fname, sizeof(fname), "%s/conf", directory);

	try
	{	
		Profile prof(fname);
		Section* s;

		s = prof.get_section("world");
		if(!s)
			sec_missing("world");

		const char* str;

		str = s->get_string("name", name);
		snprintf(hd.name, sizeof(hd.name), "%s", str);

		str = s->get_string("author", 0);
		if(!str)
			key_missing("world", "author");
		snprintf(hd.author, sizeof(hd.author), "%s", str);

		str = s->get_string("descr", 0);
		if (!str)
			key_missing("world", "descr");
		snprintf(hd.descr, sizeof(hd.descr), "%s", str);
	}
	catch(std::exception &e) {
		throw wexception("%s: %s", fname, e.what());
	}
}

void World::parse_resources(const char *directory)
{
	char fname[256];
	
	snprintf(fname, sizeof(fname), "%s/resconf", directory);

	try
	{
		Profile prof(fname);
		Section* s;

		while((s = prof.get_next_section(0))) {
			Resource_Descr* r = new Resource_Descr();
			try {
				r->parse(s);
				res.add(r);
			} catch(...) {
				delete r;
				throw;
			}
		}
	}
	catch(std::exception &e) {
		throw wexception("%s: %s", fname, e.what());
	}
}

void World::parse_terrains(const char *directory)
{
	char fname[256];
	
	snprintf(fname, sizeof(fname), "%s/terrainconf", directory);
   
   Profile prof(fname);
   Section* s;

	while((s = prof.get_next_section(0)))
	{
		Terrain_Descr *ter = new Terrain_Descr();
		try {
			ter->read(directory, s);
			ters.add(ter);
		} catch(...) {
			delete ter;
			throw;
		}
	}
}

void World::parse_bobs(const char *directory)
{
	char subdir[256];
	filenameset_t dirs;
	
	snprintf(subdir, sizeof(subdir), "%s/bobs", directory);
	
	g_fs->FindFiles(subdir, "*", &dirs);
	
	for(filenameset_t::iterator it = dirs.begin(); it != dirs.end(); it++) {
		//cerr << "a bob in " << *it << endl;
		
		Logic_Bob_Descr *descr;
		try {
			descr = Logic_Bob_Descr::create_from_dir(it->c_str());
		} catch(std::exception &e) {
			cerr << *it << ": " << e.what() << " (garbage directory?)" << endl;
			descr = 0;
		}
		if (descr)
			bobs.add(descr);
	}
}

// 
// Down here: subclasses of world
// 

// 
// read a terrain description in
// 
void Terrain_Descr::read(const char *directory, Section *s)
{
	const char *str;

	snprintf(name, sizeof(name), "%s", s->get_name());

	// TODO: Implement the following fields
	// def_res = water|fish   (example)
	str = s->get_string("def_res", 0);
	if (str) {
		// def_stock = 10  (count)
		s->get_int("def_stock", 0);
	}
	s->get_string("resources");
	
   // switch is
   str = s->get_string("is", 0);
	if (!str)
		key_missing(name, "is");

   if(!strcasecmp(str, "dry")) {
      is = TERRAIN_DRY;
   } else if(!strcasecmp(str, "green")) {
      is = 0;
   } else if(!strcasecmp(str, "water")) {
      is = TERRAIN_WATER|TERRAIN_DRY|TERRAIN_UNPASSABLE;
   } else if(!strcasecmp(str, "acid")) {
      is = TERRAIN_ACID|TERRAIN_DRY|TERRAIN_UNPASSABLE;
   } else if(!strcasecmp(str, "mountain")) {
      is = TERRAIN_DRY|TERRAIN_MOUNTAIN;
   } else if(!strcasecmp(str, "dead")) {
      is = TERRAIN_DRY|TERRAIN_UNPASSABLE|TERRAIN_ACID;
   } else if(!strcasecmp(str, "unpassable")) {
      is = TERRAIN_DRY|TERRAIN_UNPASSABLE;
   } else
		throw wexception("%s: invalid type '%s'", name, str);

   // Load texture image(s)
	char fnametmpl[256];
	char fname[256];
	
	str = s->get_string("texture", 0);
	if (str)
		snprintf(fnametmpl, sizeof(fnametmpl), "%s/%s", directory, str);
	else
		snprintf(fnametmpl, sizeof(fnametmpl), "%s/pics/%s_??.bmp", directory, name);

	for(;;) {
		int nr = ntex;
		char *p;
		
		// create the file name by reverse-scanning for '?' and replacing
		strcpy(fname, fnametmpl);
		p = fname + strlen(fname);
		while(p > fname) {
			if (*--p != '?')
				continue;
			
			*p = '0' + (nr % 10);
			nr = nr / 10;
		}
		
		if (nr) // cycled up to maximum possible frame number
			break;
		
		// is the frame actually there?
		if (!g_fs->FileExists(fname))
			break;
	
		Pic* pic = new Pic();
		if (pic->load(fname)) {
			delete pic;
			break;
		}

		if(pic->get_w() != TEXTURE_W && pic->get_h() != TEXTURE_H) {
			delete pic;
			throw wexception("%s: texture must be %ix%i pixels big", fname, TEXTURE_W, TEXTURE_H);
		}
		
		ntex++;
		tex = (Pic**)realloc(tex, sizeof(Pic*)*ntex);
		tex[ntex-1] = pic;
	}
}

/* "attic" -- re-add this in when it's needed
   // parse resources
   str=s->get_string("resources", 0);
   if(str && strcasecmp("", str)) {
      nres=1;
      uint i=0;
      while(i < strlen(str)) { if(str[i]==',') { nres++; }  i++; }

      res=new uchar[nres];
      char temp[200];
      uint n=0;
      uint cur_res=0;
      i=0;
      Resource_Descr* rtemp;
      while(i<strlen(str)) {
         temp[n]=str[i];
         i++;
         n++;
         if(str[i]==',') {
            temp[n]='\0';
            n--;
            while(temp[n] == ',' || temp[n]==' ' || temp[n]=='\t') temp[n--]='\0';
            uint z=0;
            while(temp[z] == ' ' || temp[z] == '\t') z++;
            n=0;
            i++;
            rtemp=resf.exists(temp+z);
            if(!rtemp) {
               strcpy(err_sec,s->get_name());
               strcpy(err_key,"resource");
               strcpy(err_msg, temp+z);
               strcat(err_msg,": Resource does not exist!");
               return ERROR;  
            } 
            res[cur_res++]=resf.get_index(rtemp->get_name());
         }
      }
      temp[n]='\0';
      n--;
      while(temp[n] == ',' || temp[n]==' ' || temp[n]=='\t') temp[n--]='\0';
      uint z=0;
      while(temp[z] == ' ' || temp[z] == '\t') z++;
      rtemp=resf.exists(temp+z);
      if(!rtemp) {
         strcpy(err_sec,s->get_name());
         strcpy(err_key,"resource");
         strcpy(err_msg, temp+z);
         strcat(err_msg,": Resource does not exist!");
         return ERROR;  
      } 
      res[cur_res++]=resf.get_index(rtemp->get_name());
   }
*/

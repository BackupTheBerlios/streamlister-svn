/* Copyright (C) 2004  crass <crass@users.berlios.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

//~ #include <map>
#include <vector>
#include <utility>
#include <stdexcept>

#include <libxml++/libxml++.h>
#include <gtkmm.h>

#include "utility.h"

#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

class Configuration{
    public:
	typedef Glib::ustring string_type;
    
    public:
	Configuration(const string_type &file = string_type());
	
	void parse_config(const string_type &file = string_type());
	//~ void save_config(const string_type &file = string_type());
	void save_config();
	
	//~ Glib::ustring operator [](const string_type& k){
	    
	    //~ return configuration_data[k];
	//~ }
    
    private:
	void _parse_player_cmd(xmlpp::Element * const);
	void _parse_url(xmlpp::Element * const);
	void _parse_cache_pls(xmlpp::Element * const);
	void _parse_columns(xmlpp::Element * const);
	void _parse_ratings(xmlpp::Element * const);
    
    private:
	static void (Configuration::*_parse_fun_array[])(xmlpp::Element *);
    
    private:
	Glib::ustring m_configfile;
	Glib::ustring m_player_cmd;
	Glib::ustring m_url;
	Glib::ustring m_num_entries;
	
	bool m_cache_pls;
	
	std::vector<std::pair<string_type, bool> > m_columns;
	std::vector<std::pair<string_type, bool> > m_ratings;
	//~ __gnu_cxx::hash_set<string_type> columns;
	//~ __gnu_cxx::hash_set<string_type> ratings;
	
	//~ std::map<string_type, string_type> configuration_data;
    
	xmlpp::DomParser             m_xmldocument;
};

#endif


//~ #include <map>
#include <stdexcept>

#include <libxml++/libxml++.h>
#include <gtkmm.h>

#include "utility.h"

#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

class Configuration{
    public:
	Configuration(const Glib::ustring &file = Glib::ustring())
	    :m_configfile(file)
	{
	    if(m_configfile != ""){
		parse_config(m_configfile);
	    }
	    //~ for(std::map<Glib::ustring, Glib::ustring>::iterator i = configuration_data.being(); i != configuration_data.end(); i++){
		//~ std::cout <<  << << std::endl;
	    //~ }
	}
	
	void parse_config(const Glib::ustring &file = Glib::ustring()){
	    
	    if (file == ""){
		if (m_configfile == "")
		    throw std::runtime_error("No configfile specified");
	    }else{
		m_configfile = file;
	    }
	    
	    /* check to see if configfile exists, if not create it */
	    if(!file_exists(m_configfile)){
		return;
	    }
	    
	    std::cout << "Parsing configfile: " << m_configfile << std::endl;
	    
	    m_xmldocument.parse_file(m_configfile);
	    
	    xmlpp::Document *doc = m_xmldocument.get_document();
	    
	    xmlpp::Element *cur_element = doc->get_root_node();
	    
	    std::cout << "path: " << cur_element->get_path() << std::endl;
	    
	    xmlpp::Node::NodeList playlists = cur_element->get_children(std::string("playlist"));
	    
	    if (playlists.size() != 1){
		std::cout << "0 or more than 1 playlists: " << playlists.size() << std::endl;
		throw std::runtime_error("Incorrect Number of playlists");
	    }
	    
	    // points to playlist element
	    cur_element = static_cast<xmlpp::Element *>(*(playlists.begin()));
	    
	    xmlpp::Element::AttributeList attrlist = cur_element->get_attributes();
	    
	    xmlpp::Element::AttributeList::iterator i = attrlist.begin();
	    for(;i != attrlist.end(); i++){
		std::cout << "Name: " << (*i)->get_name() << ", Value: " << (*i)->get_value() << std::endl;
	    }
	    
	    xmlpp::NodeSet nset = cur_element->find(std::string("entry"));
	    
	}
	
	Glib::ustring operator [](const Glib::ustring& k){
	    
	    return configuration_data[k];
	}
    
    private:
	void _parse_config(){
	    
	}
    
    private:
	Glib::ustring m_configfile;
	
	std::map<Glib::ustring, Glib::ustring> configuration_data;
    
	xmlpp::DomParser             m_xmldocument;
};

#endif

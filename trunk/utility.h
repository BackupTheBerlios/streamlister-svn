
#include <string>
#include <sstream>
#include <cstring>

#if __GNUC__ == 3 && __GNUC_MINOR__ < 4
#  include <ext/stl_hash_fun.h>
#else
#  include <ext/hash_fun.h>
#endif

#include <glibmm.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#ifndef __UTILITY_H__
#define __UTILITY_H__

/*** Make hash functor for hashing std::strings  ***/
namespace __gnu_cxx
{
    template<> struct hash<std::string> {
      size_t operator()(std::string __x) const { return __gnu_cxx::hash<const std::string::value_type*>()(__x.c_str()); }
    };
    template<> struct hash<Glib::ustring> {
      //~ size_t operator()(Glib::ustring __x) const { return __gnu_cxx::hash<const Glib::ustring::value_type*>()(__x.c_str()); }
      size_t operator()(Glib::ustring __x) const { return __gnu_cxx::hash<const char*>()(__x.c_str()); }
    };
    //~ template<> struct hash<const gunichar*> {
      //~ size_t operator()(const gunichar* __x) const { return __gnu_cxx::hash<const char*>()(__x.c_str()); }
    //~ };
} // END namespace __gnu_cxx

enum {LEFT_MOUSE_BUTTON=1, MIDDLE_MOUSE_BUTTON=2, RIGHT_MOUSE_BUTTON=3};

/* debug class for output, just a tiny wrapper that doesn't provide all the interface (unless they aare virtual) */
/* http://www.allegro.cc/forums/view_thread.php?_id=438115#target */
class dstream : public std::ostream {
    /* FIXME: The prefix doesn't quite work the way I'd like it to, but I'm not going to use it for now */
    public:
	dstream(int dl = default_level, bool p = false)
	    :std::ostream(std::cerr.rdbuf()),debug_level(dl),cur_level(dl),
	     prefix(p){}
	
	template <typename T>
	dstream& operator << (const T& arg){
	    if(cur_level <= debug_level && debug_level && cur_level){
		if(prefix)
		    static_cast<dstream&>((*static_cast<std::ostream*>(this) <<("<")) << cur_level << ">");
		return static_cast<dstream&>(*static_cast<std::ostream*>(this) <<(arg));
	    }else
		return *this;
	}
	dstream& operator << (std::ostream& (*f)(std::ostream&)){
	    if(cur_level <= debug_level && debug_level && cur_level){
		if(prefix)
		    static_cast<dstream&>((*static_cast<std::ostream*>(this) <<("<")) << cur_level << ">");
		return static_cast<dstream&>((f(*this)));
	    }else
		return *this;
	}
	/* debug_lvl should be on scale 0-9, 9 meaning most verbose, 0 is no output */
	dstream& operator () (int dl = default_level){
	    cur_level = dl;
	    return *this;
	}
	
	int get_debug_level() const {return debug_level;}
	int set_debug_level(int l=default_level) {return (debug_level=l,debug_level);}
	
	bool get_prefix() {return prefix;}
	bool set_prefix(bool s) {bool t=prefix;prefix=s; return t;}
	
    private:
	static const int default_level = 5;
	int debug_level, cur_level;
	bool prefix;
	//~ debugout();
	
	/* no copy constructor */
	dstream(const dstream&);
};

extern dstream dout;

template <typename T>
bool file_exists(const T& file){
    std::stringstream ss;
    struct stat buf;
    
    if(file.substr(0, 2) == T("~/")){
	ss << getenv("HOME") << file.substr(1);
    }else{
	ss << file;
    }
    
    //~ dout(8) << "file_exists: " << ss.str() << std::endl;
    
    if((stat(ss.str().c_str(), &buf)) == -1){
	dout(6) << strerror(errno) << std::endl;
	return false;
    }
    
    return S_ISREG(buf.st_mode) == 1; // don't check other perms, don't care at this point
}

//~ template <class T = int, int S = 0>
//~ void test_func(T arg = S){
    //~ std::cout << "test_func(): " << arg << std::endl;
    //~ std::cout << "int(): " << int() << std::endl;
//~ }

#if 0
template <typename T, const char *S = "%s">
#else
template <typename T>
#define S "%s"
#endif
T string_subst(const T& str, const T& replace, const T& search=T(S)){
    typename T::size_type pos = 0;
    T ret_str(str);
    while((pos = ret_str.find(search, pos)) != T::npos){
	ret_str.replace(pos, search.size(), replace);
    }
    //~ std::cout << "string_subst(): " << ret_str << std::endl;
    return ret_str;
}
#undef S

#endif /* __UTILITY_H__ */

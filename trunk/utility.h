
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

#endif /* __UTILITY_H__ */

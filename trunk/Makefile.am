bin_PROGRAMS = streamlister
streamlister_SOURCES = main.cxx dialog_boxes.cxx playlist_widgets.cxx playlist.cxx \
		 dialog_boxes.h playlist_widgets.h playlist.h utility.h
streamlister_CXXFLAGS = $(GTKMM_CFLAGS) $(LIBXMLPP_CFLAGS) $(CURLPP_CFLAGS) $(CFLAGS)
streamlister_LDFLAGS = $(GTKMM_LIBS) $(LIBXMLPP_LIBS) $(CURLPP_LIBS) $(LDFLAGS)

#maintainer-clean-local: clean
super-clean: maintainer-clean
	rm -f aclocal.m4
	rm -f Makefile.in depcomp install-sh missing
	rm -f configure

OBJS= ReadGluing.o IdentifyGluings.o triangulations.o unix_cusped_census.o isometry.o
identifyManifold : $(OBJS)
	c++ $(CPPFLAGS) $(OBJS) -o $@ -lsnappea
	

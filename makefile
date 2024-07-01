todo: spinningcube

LDLIBS=-losg -losgViewer -losgDB -losgGA

clean:
	rm -f *.o *~

cleanall: clean
	rm -f test hellotriangle helloviewport adaptviewport movingtriangle \
		spinningcube hellotexture hellotexture2 multitex multitex2

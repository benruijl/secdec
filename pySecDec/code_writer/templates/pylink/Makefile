include ../Makefile.conf

../$(NAME)_pylink.so : pylink.o ../lib$(NAME).a
	$(CXX) -shared -o $@ $+ $(LDFLAGS)

clean:
	rm -f *.o lib$(NAME).a lib$(NAME).so

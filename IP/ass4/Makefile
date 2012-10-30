all: ass3 paper

ass3:
	$(MAKE) -C assignment_3_servers

paper:
	$(MAKE) -C paper

clean:
	$(MAKE) clean -C paper
	$(MAKE) clean -C assignment_3_servers
	rm -f *~
	rm -f *.gch

.PHONY: ass3 paper

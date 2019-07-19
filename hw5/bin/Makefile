all: diff

dofile=mydo
a:
	@echo $(dofile)
	@./adtTest.array -f $(dofile) >  mine.txt 2>&1
	@./adtTest-linux16.array -f $(dofile) >  ref.txt 2>&1
	colordiff mine.txt ref.txt
	# @rm mine.txt
	# @rm ref.txt

d:
	@echo $(dofile)
	@./adtTest-linux16.dlist -f $(dofile) >  ref.txt 2>&1
	@./adtTest.dlist -f $(dofile) >  mine.txt 2>&1
	colordiff mine.txt ref.txt

b:
	@echo $(dofile)
	@./adtTest-linux18.bst -f $(dofile) >  ref.txt 2>&1
	@./adtTest.bst -f $(dofile) >  mine.txt 2>&1
	colordiff mine.txt ref.txt
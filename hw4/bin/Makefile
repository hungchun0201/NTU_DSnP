all: diff

dofile=do1

mydo:
	@python3 gen.py 1000
	# @echo "mydo"
	@./memTest -f mydo >  mine.txt 2>&1
	@./memTest-linux16 -f mydo >  ref.txt 2>&1
	diff mine.txt ref.txt
	@rm mydo

diff:
	@echo $(dofile)
	@./memTest -f $(dofile) >  mine.txt 2>&1
	@./memTest-linux16 -f $(dofile) >  ref.txt 2>&1
	diff mine.txt ref.txt
	# @rm mine.txt
	# @rm ref.txt
detailed:
	@echo $(dofile)
	@valgrind --leak-check=full --show-leak-kinds=all ./memTest -f $(dofile) >  mine.txt 2>&1
	@valgrind --leak-check=full --show-leak-kinds=all ./memTest-linux16 -f $(dofile) >  ref.txt 2>&1
	diff mine.txt ref.txt
leak_check:
	@echo $(dofile)
	@valgrind --leak-check=full ./memTest -f $(dofile) >  mine.txt 2>&1
	@valgrind --leak-check=full ./memTest-linux16 -f $(dofile) >  ref.txt 2>&1
	diff mine.txt ref.txts
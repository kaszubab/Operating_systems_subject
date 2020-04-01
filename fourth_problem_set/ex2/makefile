CC = gcc 
CC_flags = -Wall -std=c11

all : child.c main.c
				$(CC) $(CC_flags) child.c -o child
				$(CC) $(CC_flags) main.c -o program
				make tests
				make clean

tests:
				echo "Ignore tests" >> raport.txt
				./program ignore > raport.txt
				echo "\n" >> raport.txt

				echo "Handler tests" >> raport.txt
				./program handler >> raport.txt
				echo "\n" >> raport.txt

				echo "Mask tests" >> raport.txt
				./program mask >> raport.txt
				echo "\n" >> raport.txt

				echo "Pending tests" >> raport.txt
				./program pending >> raport.txt
				echo "\n" >> raport.txt

				echo "Jak widać zarówno procesy pochodne stworzone za pomocą polecenia fork jak i exec dziedziczą Ignorowanie sygnału." >> raport.txt
				echo "Procesy pochodne nie dziedziczą definiowanych funkcji obsługi sygnału."  >> raport.txt
				echo "Procesy pochodne dziedziczą maski igmorowanych sygnałów natomiast nie są w stanie odebrać sygnału oczekującego na odbiór przez proces macierzysty" >> raport.txt


clean:
				rm -r program
				rm -r child



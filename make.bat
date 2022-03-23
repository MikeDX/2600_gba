zarmasm -CPU ARM7TDMI -Littleend boot.asm
rem zarmasm -CPU ARM7TDMI -Littleend data.asm
rem zarmcc -c -Wall -O2 -ansic -fpu none -Littleend -cpu ARM7TDMI -apcs /narrow/noswst main.c -o main.o -errors errlog.txt
rem zarmcc -c -Wall -O2 -ansic -fpu none -Littleend -cpu ARM7TDMI -apcs /narrow/noswst collisio.c -o collisio.o -errors errlog.txt
rem zarmcc -c -Wall -O2 -ansic -fpu none -Littleend -cpu ARM7TDMI -apcs /narrow/noswst cpu.c cpu.o -errors errlog.txt
rem zarmcc -c -Wall -O2 -ansic -fpu none -Littleend -cpu ARM7TDMI -apcs /narrow/noswst memory.c memory.o -errors errlog.txt
rem zarmcc -c -Wall -O2 -ansic -fpu none -Littleend -cpu ARM7TDMI -apcs /narrow/noswst display.c display.o -errors errlog.txt
zarmcc -c -Wall -O2 -ansic -fpu none -Littleend -cpu ARM7TDMI -apcs /narrow/noswst files.c files.o -errors errlog.txt
zarmcc -c -Wall -O2 -ansic -fpu none -Littleend -cpu ARM7TDMI -apcs /narrow/noswst keyboard.c keyboard.o -errors errlog.txt
rem zarmcc -c -Wall -O2 -ansic -fpu none -Littleend -cpu ARM7TDMI -apcs /narrow/noswst misc.c misc.o -errors errlog.txt
rem zarmcc -c -Wall -O2 -ansic -fpu none -Littleend -cpu ARM7TDMI -apcs /narrow/noswst mouse.c mouse.o -errors errlog.txt
rem zarmcc -c -Wall -O2 -ansic -fpu none -Littleend -cpu ARM7TDMI -apcs /narrow/noswst raster.c raster.o -errors errlog.txt
rem zarmcc -c -Wall -O2 -ansic -fpu none -Littleend -cpu ARM7TDMI -apcs /narrow/noswst table.c table.o -errors errlog.txt
rem zarmcc -c -Wall -O2 -ansic -fpu none -Littleend -cpu ARM7TDMI -apcs /narrow/noswst vmachine.c vmachine.o -errors errlog.txt
@rem zarmcc -c -Wall -O2 -ansic -fpu none -Littleend -cpu ARM7TDMI -apcs /narrow/noswst xdebug.c xdebug.o -errors errlog.txt
@rem For emulators....
rem zarmlink -bin -first boot.o main.o collisio.o cpu.o display.o files.o keyboard.o misc.o mouse.o raster.o table.o vmachine.o memory.o -map -ro-base 0x08000000 -o pacman.gba
@rem For 256K download cart.....
rem zarmlink -bin -first start.o start.o data.o main.o ghost.o libc.o title.o cosin.o irq.o -map -ro-base 0x02000000 -o pacman.bin
zarmlink -bin -first boot.o -map -ro-base 0x08000000 -rw-base 0x2000000 boot.o main.o collisio.o cpu.o display.o files.o keyboard.o misc.o mouse.o raster.o table.o vmachine.o memory.o -o vcs2600.gba
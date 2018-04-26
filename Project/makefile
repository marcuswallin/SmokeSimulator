# set this variable to the director in which you saved the common files
commondir = ../common/

all : main

main : main.c $(commondir)GL_utilities.c smoke.h smoke.c cameracontrol.c smoke_emitter.c smoke_emitter.h display.c $(commondir)VectorUtils3.c $(commondir)loadobj.c $(commondir)LoadTGA.c $(commondir)Linux/MicroGlut.c
	gcc -Wall -o main -I$(commondir) -I../common/Linux -DGL_GLEXT_PROTOTYPES main.c  $(commondir)GL_utilities.c $(commondir)loadobj.c $(commondir)VectorUtils3.c $(commondir)LoadTGA.c $(commondir)Linux/MicroGlut.c -lXt -lX11 -lGL -lm

clean :
	rm main

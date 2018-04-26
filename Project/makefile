# set this variable to the director in which you saved the common files
commondir = ../common/

all : main

main : main.c $(commondir)GL_utilities.c lamp.c smoke.c cameracontrol.c display.c $(commondir)VectorUtils3.c $(commondir)loadobj.c $(commondir)LoadTGA.c $(commondir)Linux/MicroGlut.c
	gcc -Wall -o main -std=gnu99 -I$(commondir) -I../common/Linux -DGL_GLEXT_PROTOTYPES main.c  $(commondir)GL_utilities.c $(commondir)loadobj.c $(commondir)VectorUtils3.c $(commondir)LoadTGA.c $(commondir)Linux/MicroGlut.c -lXt -lX11 -lGL -lm

clean :
	rm main

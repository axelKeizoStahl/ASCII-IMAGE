ascii_webcam: ascii_webcam.c
	gcc ascii_webcam.c -o ascii_webcam -lncurses

clean:
	rm -f ascii_webcam

run: ascii_webcam
	./ascii_webcam

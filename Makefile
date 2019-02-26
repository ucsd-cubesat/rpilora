lorarx: lora.c lorarx.c spi.c helper.c
	gcc lorarx.c lora.c spi.c helper.c -lbcm2835 -o lorarx

loratx: lora.c loratx.c spi.c helper.c
	gcc loratx.c lora.c spi.c helper.c -lbcm2835 -o loratx

clean:
	rm -f loratest loratx lorarx *.o
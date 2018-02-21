all : chat_client chat_server

chat_client : chat_client.c
						gcc -Wall -g -o chat_client chat_client.c

chat_server : chat_server.c
						gcc -Wall -g -o chat_server chat_server.c

clean :
			rm -f *.o chat_client chat_server

all: clean client server

client:
	gcc -Wall client.c general_fn/error_handling.c client_fn/io_related.c client_fn/log_related.c client_fn/options_related.c \
	client_fn/socket_related.c general_fn/compress_related.c general_fn/conversion_related.c -lz -o bin/client

server:
	gcc -Wall server.c server_fn/pipe_io.c server_fn/socket_related.c general_fn/error_handling.c general_fn/compress_related.c \
	general_fn/conversion_related.c server_fn/options_related.c -lz -o bin/server

clean:
	rm -f bin/client bin/server

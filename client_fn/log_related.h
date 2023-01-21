// Create log with log_new_name, with all initial configurations (port PORT,
// host HOST and if compress is activated through COMPRESS).
char *create_log(char *log_new_name, int PORT, char *HOST, int COMPRESS);

// Update log LOG_NAME for non-compressed based client, logging sent command if
// SEND_OR_RECEIVE is 0 and received command otherwise.
void update_log(char *command, char *LOG_NAME, int SEND_OR_RECEIVE);

// Update log LOG_NAME for compressed based client, logging sent command and
// compressed_command if SEND_OR_RECEIVE is 0 and received command and
// compressed_command otherwise.
void update_log_compressed(char *command, char *compressed_command,
                           char *LOG_NAME, int SEND_OR_RECEIVE);

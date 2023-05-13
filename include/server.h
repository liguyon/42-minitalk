#ifndef SERVER_H
# define SERVER_H

# define _XOPEN_SOURCE 500

#include "libft.h"
# include <signal.h>

# define ERRCODE_SIGACTION_FAIL 1
# define ERRMSG_SIGACTION_FAIL "Error: Failed to register a signal handler.\n"
# define ERRCODE_MALLOC_FAIL 2
# define ERRMSG_MALLOC_FAIL "Error: malloc failed to allocate memory.\n";
# define ERRCODE_SEND_SIG 3
# define ERRMSG_SEND_SIG "Failed to send a signal to the specified PID.\n"

# define MSG_LISTEN "Listening on [PID %d]...\n\n"
# define MSG_RECEIVED "Received message of length %d from [PID %d].\n___\n%s\n___\n\n"

typedef struct s_msg
{
	unsigned char	*str;
	size_t	len;
	int		client_pid;
	int		len_received;
	int		message_received;
	int		received_bits;
	int		current_bit;
	int		current_char;
	int		send_signal;
}	t_msg;

#endif
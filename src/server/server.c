
#include "server.h"
#include "libft.h"
#include <limits.h>

static t_msg	g_msg;

int	print_error_message(int error_code)
{
	if (error_code == ERRCODE_SIGACTION_FAIL)
		ft_print_error(ERRMSG_SIGACTION_FAIL);
	else if (error_code == ERRCODE_SEND_SIG)
		ft_print_error(ERRMSG_SEND_SIG);
	return (error_code);
}

void	send_reply(int pid, int signal)
{
	if (g_msg.send_signal == 1 || g_msg.message_received)
	{
		if (kill(pid, signal) == -1)
		{
			if (g_msg.str)
			{
				free(g_msg.str);
				g_msg.str = NULL;
			}
			exit(print_error_message(ERRCODE_SEND_SIG));	
		}
	}
	g_msg.send_signal = 0;
}

void	recv_msg_len(int signum)
{
	if (signum == SIGUSR1)
		g_msg.received_bits |= (1 << g_msg.current_bit);
	g_msg.current_bit++;
	send_reply(g_msg.client_pid, SIGUSR1);
	if (g_msg.current_bit == sizeof(size_t) * CHAR_BIT)
	{
		g_msg.len = g_msg.received_bits;
		g_msg.received_bits = 0;
		g_msg.current_bit = 0;
		g_msg.len_received = 1;
	}
}

void	recv_msg_str(int signum)
{
	char	c;

	if (!g_msg.str)
	{
		g_msg.str = ft_calloc(1 + g_msg.len, sizeof(char));
		if (!g_msg.str)
			exit(print_error_message(ERRCODE_MALLOC_FAIL));
	}
	if (signum == SIGUSR1)
		g_msg.received_bits |= (1 << g_msg.current_bit);
	g_msg.current_bit++;
	send_reply(g_msg.client_pid, SIGUSR1);
	if (g_msg.current_bit == sizeof(char) * CHAR_BIT)
	{
		c = (char)g_msg.received_bits;
		g_msg.received_bits = 0;
		g_msg.current_bit = 0;
		g_msg.str[g_msg.current_char] = c;
		g_msg.current_char++;
		if (g_msg.current_char == (int)g_msg.len)
		{
			g_msg.str[g_msg.current_char] = '\0';
			g_msg.message_received = 1;
		}
	}
}

void	sigusr_handler(int signum, siginfo_t *siginfo, void *context)
{
	(void)context;
	g_msg.client_pid = siginfo->si_pid;
	g_msg.send_signal = 1;
	if (!g_msg.len_received)
		recv_msg_len(signum);
	else
		recv_msg_str(signum);
}

void	sigint_handler(int signum)
{
	if (signum == SIGINT)
	{
		if (g_msg.str != NULL)
		{
			free(g_msg.str);
			g_msg.str = NULL;
		}
		ft_printf("\nServer closed.\n");
		exit(0);
	}
}

int	set_signal_handler(void)
{
	struct sigaction	sa_sigusr;
	struct sigaction	sa_sigint;

	sa_sigusr.sa_sigaction = sigusr_handler;
	sigemptyset(&sa_sigusr.sa_mask);
	sa_sigusr.sa_flags = SA_SIGINFO;
	if (sigaction(SIGUSR1, &sa_sigusr, NULL) == -1 || sigaction(SIGUSR2, &sa_sigusr, NULL) == -1)
		exit(print_error_message(ERRCODE_SIGACTION_FAIL));
	sa_sigint.sa_handler = sigint_handler;
	sigemptyset(&sa_sigint.sa_mask);
	sa_sigint.sa_flags = 0;
	if (sigaction(SIGINT, &sa_sigint, NULL) == -1)
		exit(print_error_message(ERRCODE_SIGACTION_FAIL));
	return (0);
}

void	init_gmsg(void)
{
	g_msg.client_pid = 0;
	g_msg.current_bit = 0;
	g_msg.current_char = 0;
	g_msg.len = 0;
	g_msg.len_received = 0;
	g_msg.message_received = 0;
	g_msg.received_bits = 0;
	g_msg.send_signal = 0;
	g_msg.str = NULL;
	ft_printf(MSG_LISTEN, getpid());
}

int	main(void)
{
	set_signal_handler();
	while (1)
	{
		init_gmsg();
		pause();
		while (!g_msg.message_received)
			pause();
		if (g_msg.message_received == 1)
		{
			usleep(200);
			send_reply(g_msg.client_pid, SIGUSR2);
			ft_printf(MSG_RECEIVED, (int)g_msg.len, g_msg.client_pid, g_msg.str);
			free(g_msg.str);
		}
	}
	return (0);
}
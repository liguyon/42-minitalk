#include "client.h"
#include <limits.h>
#include <stdio.h>
#include <signal.h>

static t_msg	g_msg;

int	print_error_message(int error_code)
{
	if (error_code == ERRCODE_ARGC)
		ft_print_error(ERRMSG_ARGC);
	else if (error_code == ERRCODE_INVALID_PID)
		ft_print_error(ERRMSG_INVALID_PID);
	else if (error_code == ERRCODE_EMPTY_STR)
		ft_print_error(ERRMSG_EMPTY_STR);
	else if (error_code == ERRCODE_SIGACTION_FAIL)
		ft_print_error(ERRMSG_SIGACTION_FAIL);
	else if (error_code == ERRCODE_SEND_SIG)
		ft_print_error(ERRMSG_SEND_SIG);
	else if (error_code == ERRCODE_TIMEOUT)
		ft_print_error(ERRMSG_TIMEOUT);
	else if (error_code == ERRCODE_SEND_MSG)
		ft_print_error(ERRMSG_SEND_MSG);
	return (error_code);
}

int	send_bit(int pid, u_int64_t bit)
{
	if (bit)
	{
		if (kill(pid, SIGUSR1) == -1)
			exit(print_error_message(ERRCODE_SEND_SIG));
	}
	else
	{
		if (kill(pid, SIGUSR2) == -1)
			exit(print_error_message(ERRCODE_SEND_SIG));
	}
	return (0);
}

int	send_data(int pid, u_int64_t data, size_t bits_size)
{
	size_t	i;

	i = -1;
	while (++i < bits_size)
	{
		g_msg.send_signal = 0;
		send_bit(pid, data & (1 << i));
		usleep(2000);
		if (g_msg.send_signal != 1)
			exit(print_error_message(ERRCODE_TIMEOUT));
	}
	return (0);
}

void	send_str(int pid, const unsigned char *str)
{
	size_t	i;

	i = -1;
	while (++i < g_msg.len)
		send_data(pid, str[i], CHAR_BIT * sizeof(unsigned char));
}

int	parse_msg(int ac, char **av)
{
	long	pid;

	if (ac != 3)
		exit(print_error_message(ERRCODE_ARGC));
	g_msg.len = ft_strlen(av[2]);
	if (!g_msg.len)
		exit(print_error_message(ERRCODE_EMPTY_STR));
	g_msg.str = (const unsigned char *)av[2];
	pid = ft_atol(av[1]);
	if (pid < 1 || pid > INT_MAX)
		exit(print_error_message(ERRCODE_INVALID_PID));
	g_msg.pid = (int)pid;
	return (0);
}

void	sigusr_handler(int signum)
{
	if (signum == SIGUSR1)
		g_msg.send_signal = 1;
	else if (signum == SIGUSR2)
		g_msg.msg_received = 1;
}

int	set_signal_handler(void)
{
	struct sigaction	sa_sigusr;

	sa_sigusr.sa_handler = sigusr_handler;
	sigemptyset(&sa_sigusr.sa_mask);
	sa_sigusr.sa_flags = 0;
	if (sigaction(SIGUSR1, &sa_sigusr, NULL) == -1 || sigaction(SIGUSR2, &sa_sigusr, NULL) == -1)
		exit(print_error_message(ERRCODE_SIGACTION_FAIL));
	return (0);
}

int	main(int ac, char **av)
{
	parse_msg(ac, av);
	set_signal_handler();
	g_msg.msg_received = 0;
	ft_printf(MSG_SENDING, (int)g_msg.len, g_msg.pid);
	send_data(g_msg.pid, g_msg.len, CHAR_BIT * sizeof(size_t));
	send_str(g_msg.pid, g_msg.str);
	usleep(1000);
	if (!g_msg.msg_received)
		return (print_error_message(ERRCODE_SEND_MSG));
	else
		ft_printf(MSG_SENT);
	return (0);
}
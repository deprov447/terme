#include <iostream>
#include <sys/ioctl.h>
#include <cstring>
#include "get_tty_pair.h"

int main()
{
	Psuedo_term pty;
	auto [master_fd, slave_fd] = pty.get();

	pid_t p = fork();
	if (p == -1)
	{
		throw "Fork failed";
		return 1;
	}

	if (p == 0)
	{
		/* child process
		   replace process with a shell, attach its IO to slave pty
		*/
		std::cout << "in child";
		pty.close_master();
		attach_shell(p, slave_fd);
		pty.close_slave();
	}
	else
	{
		/*
			read from master pty and output to stdout
			write "whoami"
		*/

		char buffer[256];
		const char *message = "whoami\n";
		write(master_fd, message, strlen(message));
		sleep(1);
		ssize_t bytesRead = read(master_fd, buffer, sizeof(buffer));

		std::cout << std::string(buffer, bytesRead) << std::endl;

		// Close the device file
		pty.close_master();
		pty.close_slave();
	}

	return 0;
}

void attach_shell(pid_t p, int slave_fd)
{
	ioctl(slave_fd, TIOCSCTTY, NULL);

	dup2(slave_fd, 0);
	dup2(slave_fd, 1);
	dup2(slave_fd, 2);

	execl(SHELL, "-" SHELL, (char *)NULL);
}

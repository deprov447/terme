#include <string>
#include <fcntl.h>
#include <unistd.h>

class Psuedo_term
{
    int master_fd;
    int slave_fd;

public:
    Psuedo_term(std::string ptmx = "/dev/ptmx"){
        master_fd = open("/dev/ptmx", O_RDONLY);
        const char *slavePath = ptsname(master_fd);
        if (grantpt(master_fd))
            throw "grantpt() failed";
        if (unlockpt(master_fd))
            throw "unlockpt() failed";
        slave_fd = open(slavePath, O_RDONLY);
    }

    ~Psuedo_term(){
        close_master();
        close_slave();
    }

    std::pair<int, int> get(){
        return {master_fd, slave_fd};
    }

    void close_master(){
        close(master_fd);
    }

    void close_slave(){
        close(slave_fd);
    }
};
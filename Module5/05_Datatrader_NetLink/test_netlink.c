#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/netlink.h>

#define NETLINK_USER 31
#define MAX_PAYLOAD 1024

struct sockaddr_nl src_addr, dest_addr;
struct nlmsghdr *nlh = NULL;
struct iovec iov;
struct msghdr msg;
int sock_fd;



int send_netlink_message(const char *message)
{
    int status;
    
    sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_USER);
    if (sock_fd < 0) {
        perror("socket");
        return -1;
    }
    
    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid();
    
    if (bind(sock_fd, (struct sockaddr *)&src_addr, sizeof(src_addr)) < 0) {
        perror("bind");
        close(sock_fd);
        return -1;
    }
    
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid    = 0;
    dest_addr.nl_groups = 0;
    
    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
    if (!nlh) {
        perror("malloc");
        close(sock_fd);
        return -1;
    }
    
    memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
    nlh->nlmsg_len      = NLMSG_SPACE(MAX_PAYLOAD);
    nlh->nlmsg_pid      = getpid();
    nlh->nlmsg_flags    = 0;
    
    strcpy(NLMSG_DATA(nlh), message);
    
    iov.iov_base    = (void *)nlh;
    iov.iov_len     = nlh->nlmsg_len;
    
    memset(&msg, 0, sizeof(msg));
    msg.msg_name    = (void *)&dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov     = &iov;
    msg.msg_iovlen  = 1;
    
    printf("Sending message: %s\n", message);
    
    status= sendmsg(sock_fd, &msg, 0);
    if (status< 0) {
        perror("sendmsg");
        free(nlh);
        close(sock_fd);
        return -1;
    }
    
    printf("Waiting for response...\n");
    
    status= recvmsg(sock_fd, &msg, 0);
    if (status< 0) {
        perror("recvmsg");
    } else {
        printf("Received response: %s\n", (char *)NLMSG_DATA(nlh));
    }
    
    free(nlh);
    close(sock_fd);
    
    return 0;
}





int main(int argc, char *argv[])
{
    if (argc > 1) {
        return send_netlink_message(argv[1]);
    } else {
        
        return send_netlink_message("Hello");
    }

    return 0;
}
#ifndef SSH_CHANNEL_HPP
# define SSH_CHANNEL_HPP

# include <libssh/callbacks.h>

// temporary
# define USER "myuser"
# define PASS "x"

struct channel_data_struct {
    // pid of the child process the channel will spawn
    pid_t pid;
    // pty allocation
    socket_t pty_master;
    socket_t pty_slave;
    // communication with the child process
    socket_t child_stdin;
    socket_t child_stdout;
    // only used for subsystem and exec requests
    socket_t child_stderr;
    // event which is used to poll the above descriptors
    ssh_event event;
    // terminal size structure
    struct winsize *winsize;
};

struct session_data_struct {
    // pointer to the channel the session will allocate
    ssh_channel channel;
    int auth_attempts;
    int authenticated;
};

class SshChannel
{
private:
    static const size_t _bufferSize;

    static const struct winsize _defaultWindowSize;
    static const struct channel_data_struct _defaultChannelData;
    static const struct session_data_struct _defaultSessionData;
    static const struct ssh_channel_callbacks_struct _defaultChannelCallbacks;
    static const struct ssh_server_callbacks_struct _defaultServerCallbacks;

public:
    SshChannel();
    ~SshChannel();

    static int data_function(ssh_session session, ssh_channel channel, void *data, uint32_t len, int is_stderr, void *userdata);
    static int pty_request(ssh_session session, ssh_channel channel, const char *term, int cols, int rows, int py, int px, void *userdata);
    static int pty_resize(ssh_session session, ssh_channel channel, int cols, int rows, int py, int px, void *userdata);
    static int exec_pty(const char *mode, const char *command, struct channel_data_struct *cdata);
    static int exec_nopty(const char *command, struct channel_data_struct *cdata);
    static int exec_request(ssh_session session, ssh_channel channel, const char *command, void *userdata);
    static int shell_request(ssh_session session, ssh_channel channel, void *userdata);
    static int auth_password(ssh_session session, const char *user, const char *pass, void *userdata);
    static ssh_channel channel_open(ssh_session session, void *userdata);
    static int process_stdout(socket_t fd, int revents, void *userdata);
    static int process_stderr(socket_t fd, int revents, void *userdata);
    static int init(ssh_event &event, ssh_session &session);
};

#endif // !SSH_CHANNEL_HPP

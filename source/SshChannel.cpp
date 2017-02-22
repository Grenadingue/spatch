#include <sys/ioctl.h>
#include <sys/wait.h>
#include <libssh/server.h>
#include <libssh/callbacks.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <poll.h>
#include <iostream>
#include "SshChannel.hpp"
#include "PseudoTerminal.hpp"

const size_t SshChannel::_bufferSize = 1048576;

const struct winsize SshChannel::_defaultWindowSize =
{
    0, // unsigned short ws_row
    0, // unsigned short ws_col
    0, // unsigned short ws_xpixel
    0 // unsigned short ws_ypixel
};

const struct channel_data_struct SshChannel::_defaultChannelData =
{
    0, // pid_t pid
    -1, // socket_t pty_master
    -1, // socket_t pty_slave
    -1, // socket_t child_stdin
    -1, // socket_t child_stdout
    -1, // socket_t child_stderr
    NULL, // ssh_event event
    NULL // struct winsize *winsize
};

const struct session_data_struct SshChannel::_defaultSessionData =
{
    NULL, // ssh_channel channel
    0, // int auth_attempts
    0 // int authenticated
};

const struct ssh_channel_callbacks_struct SshChannel::_defaultChannelCallbacks =
{
    sizeof(struct ssh_channel_callbacks_struct), // size_t size
    NULL, // void *userdata
    &SshChannel::data_function, // ssh_channel_data_callback
    NULL, // ssh_channel_eof_callback
    NULL, // ssh_channel_close_callback
    NULL, // ssh_channel_signal_callback
    NULL, // ssh_channel_exit_status_callback
    NULL, // ssh_channel_exit_signal_callback
    &SshChannel::pty_request, // ssh_channel_pty_request_callback
    &SshChannel::shell_request, // ssh_channel_shell_request_callback
    NULL, // ssh_channel_auth_agent_req_callback
    NULL, // ssh_channel_x11_req_callback
    &SshChannel::pty_resize, // ssh_channel_pty_window_change_callback
    &SshChannel::exec_request, // ssh_channel_exec_request_callback
    NULL, // ssh_channel_env_request_callback
    NULL // &subsystem_request // ssh_channel_subsystem_request_callback
};

const struct ssh_server_callbacks_struct SshChannel::_defaultServerCallbacks =
{
    sizeof(struct ssh_server_callbacks_struct), // size_t size
    NULL, // void *userdata
    &SshChannel::auth_password, // ssh_auth_password_callback
    NULL, // ssh_auth_none_callback
    NULL, // ssh_auth_gssapi_mic_callback
    NULL, // ssh_auth_pubkey_callback
    NULL, // ssh_service_request_callback
    &SshChannel::channel_open, // ssh_channel_open_request_session_callback
    NULL, // ssh_gssapi_select_oid_callback
    NULL, // ssh_gssapi_accept_sec_ctx_callback
    NULL // ssh_gssapi_verify_mic_callback
};

SshChannel::SshChannel()
{
}

SshChannel::~SshChannel()
{
}

int SshChannel::data_function(ssh_session session, ssh_channel channel, void *data, uint32_t len, int is_stderr, void *userdata)
{
    struct channel_data_struct *cdata = static_cast<struct channel_data_struct *>(userdata);

    (void) session;
    (void) channel;
    (void) is_stderr;

    if (len == 0 || cdata->pid < 1 || kill(cdata->pid, 0) < 0)
        return 0;
    return write(cdata->child_stdin, (char *) data, len);
}

int SshChannel::pty_request(ssh_session session, ssh_channel channel, const char *term, int cols, int rows, int py, int px, void *userdata)
{
    struct channel_data_struct *cdata = static_cast<struct channel_data_struct *>(userdata);

    (void) session;
    (void) channel;
    (void) term;

    cdata->winsize->ws_row = rows;
    cdata->winsize->ws_col = cols;
    cdata->winsize->ws_xpixel = px;
    cdata->winsize->ws_ypixel = py;

    if (PseudoTerminal::openPty(&cdata->pty_master, &cdata->pty_slave, cdata->winsize) != 0)
    {
        std::cerr << "Failed to open pty" << std::endl;
        return SSH_ERROR;
    }
    return SSH_OK;
}

int SshChannel::pty_resize(ssh_session session, ssh_channel channel, int cols, int rows, int py, int px, void *userdata)
{
    struct channel_data_struct *cdata = static_cast<struct channel_data_struct *>(userdata);

    (void) session;
    (void) channel;

    cdata->winsize->ws_row = rows;
    cdata->winsize->ws_col = cols;
    cdata->winsize->ws_xpixel = px;
    cdata->winsize->ws_ypixel = py;

    if (cdata->pty_master != -1)
        return ioctl(cdata->pty_master, TIOCSWINSZ, cdata->winsize);
    return SSH_ERROR;
}

int SshChannel::exec_pty(const char *mode, const char *command, struct channel_data_struct *cdata)
{
    std::string buffer;

    switch ((cdata->pid = fork()))
    {
    case -1:
        close(cdata->pty_master);
        close(cdata->pty_slave);
        std::cerr << "Failed to fork" << std::endl;
        return SSH_ERROR;
    case 0:
        close(cdata->pty_master);
        if (PseudoTerminal::loginTty(cdata->pty_slave) != 0)
            exit(1);

        while (buffer != "hello")
        {
            std::cout << "say hello" << std::endl;
            std::getline(std::cin, buffer);
        }

        execl("/bin/sh", "sh", mode, command, NULL);

        exit(0);
    default:
        close(cdata->pty_slave);
        // pty fd is bi-directional
        cdata->child_stdout = cdata->child_stdin = cdata->pty_master;
    }
    return SSH_OK;
}

int SshChannel::exec_nopty(const char *command, struct channel_data_struct *cdata)
{
    int in[2], out[2], err[2];

    // do the plumbing to be able to talk with the child process
    if (pipe(in) != 0)
    {
        goto stdin_failed;
    }
    if (pipe(out) != 0)
    {
        goto stdout_failed;
    }
    if (pipe(err) != 0)
    {
        goto stderr_failed;
    }

    switch(cdata->pid = fork())
    {
    case -1:
        goto fork_failed;
    case 0:
        // finish the plumbing in the child process
        close(in[1]);
        close(out[0]);
        close(err[0]);
        dup2(in[0], STDIN_FILENO);
        dup2(out[1], STDOUT_FILENO);
        dup2(err[1], STDERR_FILENO);
        close(in[0]);
        close(out[1]);
        close(err[1]);

        // exec the requested command
        execl("/bin/sh", "sh", "-c", command, NULL);
        exit(0);
    }

    close(in[0]);
    close(out[1]);
    close(err[1]);

    cdata->child_stdin = in[1];
    cdata->child_stdout = out[0];
    cdata->child_stderr = err[0];

    return SSH_OK;

fork_failed:
    close(err[0]);
    close(err[1]);
stderr_failed:
    close(out[0]);
    close(out[1]);
stdout_failed:
    close(in[0]);
    close(in[1]);
stdin_failed:
    return SSH_ERROR;
}

int SshChannel::exec_request(ssh_session session, ssh_channel channel, const char *command, void *userdata)
{
    struct channel_data_struct *cdata = static_cast<struct channel_data_struct *>(userdata);

    (void) session;
    (void) channel;

    if(cdata->pid > 0)
        return SSH_ERROR;
    if (cdata->pty_master != -1 && cdata->pty_slave != -1)
        return exec_pty("-c", command, cdata);
    return exec_nopty(command, cdata);
}

int SshChannel::shell_request(ssh_session session, ssh_channel channel, void *userdata)
{
    struct channel_data_struct *cdata = static_cast<struct channel_data_struct *>(userdata);

    (void) session;
    (void) channel;

    if(cdata->pid > 0)
        return SSH_ERROR;
    if (cdata->pty_master != -1 && cdata->pty_slave != -1)
        return exec_pty("-l", NULL, cdata);
    // client requested a shell without a pty, let's pretend we allow that
    return SSH_OK;
}

int SshChannel::auth_password(ssh_session session, const char *user, const char *pass, void *userdata)
{
    struct session_data_struct *sdata = static_cast<struct session_data_struct *>(userdata);

    (void) session;

    if (strcmp(user, USER) == 0 && strcmp(pass, PASS) == 0)
    {
        sdata->authenticated = 1;
        return SSH_AUTH_SUCCESS;
    }
    sdata->auth_attempts++;
    return SSH_AUTH_DENIED;
}

ssh_channel SshChannel::channel_open(ssh_session session, void *userdata)
{
    struct session_data_struct *sdata = static_cast<struct session_data_struct *>(userdata);

    sdata->channel = ssh_channel_new(session);
    return sdata->channel;
}

int SshChannel::process_stdout(socket_t fd, int revents, void *userdata)
{
    ssh_channel channel = static_cast<ssh_channel>(userdata);
    char buf[_bufferSize];
    int n = -1;

    if (channel != NULL && (revents & POLLIN) != 0 && (n = read(fd, buf, _bufferSize)) > 0)
        ssh_channel_write(channel, buf, n);
    return n;
}

int SshChannel::process_stderr(socket_t fd, int revents, void *userdata)
{
    ssh_channel channel = static_cast<ssh_channel>(userdata);
    char buf[_bufferSize];
    int n = -1;

    if (channel != NULL && (revents & POLLIN) != 0 && (n = read(fd, buf, _bufferSize)) > 0)
        ssh_channel_write_stderr(channel, buf, n);
    return n;
}

int SshChannel::init(ssh_event &event, ssh_session &session)
{
    struct winsize wsize = _defaultWindowSize;
    struct channel_data_struct cdata = _defaultChannelData;
    struct session_data_struct sdata = _defaultSessionData;
    struct ssh_channel_callbacks_struct channel_cb = _defaultChannelCallbacks;
    struct ssh_server_callbacks_struct server_cb = _defaultServerCallbacks;
    int n = 0, rc;

    cdata.winsize = &wsize;
    channel_cb.userdata = &cdata;
    server_cb.userdata = &sdata;

    ssh_callbacks_init(&server_cb);
    ssh_callbacks_init(&channel_cb);

    ssh_set_server_callbacks(session, &server_cb);

    if (ssh_handle_key_exchange(session) != SSH_OK)
    {
        std::cerr << ssh_get_error(session) << std::endl;
        return -1;
    }

    ssh_set_auth_methods(session, SSH_AUTH_METHOD_PASSWORD);
    ssh_event_add_session(event, session);

    while (sdata.authenticated == 0 || sdata.channel == NULL)
    {
        // if the user has used up all attempts, or if he hasn't been able to authenticate in 10 seconds (n * 100ms), disconnect
        if (sdata.auth_attempts >= 3 || n >= 100)
            return -1;

        if (ssh_event_dopoll(event, 100) == SSH_ERROR)
        {
            std::cerr << ssh_get_error(session) << std::endl;
            return -1;
        }
        n++;
    }

    ssh_set_channel_callbacks(sdata.channel, &channel_cb);

    do {
        // poll the main event which takes care of the session, the channel and even our child process's stdout/stderr (once it's started)
        if (ssh_event_dopoll(event, -1) == SSH_ERROR)
            ssh_channel_close(sdata.channel);

        // if child process's stdout/stderr has been registered with the event, or the child process hasn't started yet, continue
        if (cdata.event != NULL || cdata.pid == 0)
            continue;
        // executed only once, once the child process starts
        cdata.event = event;
        // if stdout valid, add stdout to be monitored by the poll event
        if (cdata.child_stdout != -1 && ssh_event_add_fd(event, cdata.child_stdout, POLLIN, process_stdout, sdata.channel) != SSH_OK)
        {
            std::cerr << "Failed to register stdout to poll context" << std::endl;
            ssh_channel_close(sdata.channel);
        }
        // if stderr valid, add stderr to be monitored by the poll event
        if (cdata.child_stderr != -1 && ssh_event_add_fd(event, cdata.child_stderr, POLLIN, process_stderr, sdata.channel) != SSH_OK)
        {
            std::cerr << "Failed to register stderr to poll context" << std::endl;
            ssh_channel_close(sdata.channel);
        }
    } while (ssh_channel_is_open(sdata.channel) && (cdata.pid == 0 || waitpid(cdata.pid, &rc, WNOHANG) == 0));

    close(cdata.pty_master);
    close(cdata.child_stdin);
    close(cdata.child_stdout);
    close(cdata.child_stderr);

    // remove the descriptors from the polling context, since they are now closed, they will always trigger during the poll calls
    ssh_event_remove_fd(event, cdata.child_stdout);
    ssh_event_remove_fd(event, cdata.child_stderr);

    // if the child process exited
    if (kill(cdata.pid, 0) < 0 && WIFEXITED(rc))
    {
        rc = WEXITSTATUS(rc);
        ssh_channel_request_send_exit_status(sdata.channel, rc);
        // if client terminated the channel or the process did not exit nicely, but only if something has been forked
    }
    else if (cdata.pid > 0)
        kill(cdata.pid, SIGKILL);

    ssh_channel_send_eof(sdata.channel);
    ssh_channel_close(sdata.channel);

    // wait up to 5 seconds for the client to terminate the session
    for (n = 0; n < 50 && (ssh_get_status(session) & (SSH_CLOSED | SSH_CLOSED_ERROR)) == 0; n++)
        ssh_event_dopoll(event, 100);

    return 0;
}

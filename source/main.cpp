#include <libssh/libssh.h>
#include <libssh/server.h>

#include <argp.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#define SSHD_USER "libssh"
#define SSHD_PASSWORD "libssh"

#define KEYS_FOLDER "/etc/ssh/"

static int port = 22;

static const char *pcap_file = "debug.server.pcap";
static ssh_pcap_file pcap;

static void set_pcap(ssh_session session){
	if(!pcap_file)
		return;
	pcap=ssh_pcap_file_new();
	if(ssh_pcap_file_open(pcap,pcap_file) == SSH_ERROR){
		printf("Error opening pcap file\n");
		ssh_pcap_file_free(pcap);
		pcap=NULL;
		return;
	}
	ssh_set_pcap_file(session,pcap);
}

static void cleanup_pcap(void) {
	ssh_pcap_file_free(pcap);
	pcap=NULL;
}

static int auth_password(const char *user, const char *password){
    if(strcmp(user, SSHD_USER))
        return 0;
    if(strcmp(password, SSHD_PASSWORD))
        return 0;
    return 1; // authenticated
}
const char *argp_program_version = "libssh server example "
  SSH_STRINGIFY(LIBSSH_VERSION);
const char *argp_program_bug_address = "<libssh@libssh.org>";

/* Program documentation. */
static char doc[] = "libssh -- a Secure Shell protocol implementation";

/* A description of the arguments we accept. */
static char args_doc[] = "BINDADDR";

/* The options we understand. */
static struct argp_option options[] = {
  {
    .name  = "port",
    .key   = 'p',
    .arg   = "PORT",
    .flags = 0,
    .doc   = "Set the port to bind.",
    .group = 0
  },
  {
    .name  = "hostkey",
    .key   = 'k',
    .arg   = "FILE",
    .flags = 0,
    .doc   = "Set the host key.",
    .group = 0
  },
  {
    .name  = "dsakey",
    .key   = 'd',
    .arg   = "FILE",
    .flags = 0,
    .doc   = "Set the dsa key.",
    .group = 0
  },
  {
    .name  = "rsakey",
    .key   = 'r',
    .arg   = "FILE",
    .flags = 0,
    .doc   = "Set the rsa key.",
    .group = 0
  },
  {
    .name  = "verbose",
    .key   = 'v',
    .arg   = NULL,
    .flags = 0,
    .doc   = "Get verbose output.",
    .group = 0
  },
  {NULL, 0, 0, 0, NULL, 0}
};

/* Parse a single option. */
static error_t parse_opt (int key, char *arg, struct argp_state *state) {
  /* Get the input argument from argp_parse, which we
   * know is a pointer to our arguments structure.
   */
  ssh_bind sshbind = (ssh_bind_struct*)state->input;

  switch (key) {
    case 'p':
      ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_BINDPORT_STR, arg);
      port = atoi(arg);
      break;
    case 'd':
      ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_DSAKEY, arg);
      break;
    case 'k':
      ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_HOSTKEY, arg);
      break;
    case 'r':
      ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_RSAKEY, arg);
      break;
    case 'v':
      ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_LOG_VERBOSITY_STR, "3");
      break;
    case ARGP_KEY_ARG:
      if (state->arg_num >= 1) {
        /* Too many arguments. */
        argp_usage (state);
      }
      ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_BINDADDR, arg);
      break;
    case ARGP_KEY_END:
      if (state->arg_num < 1) {
        /* Not enough arguments. */
        argp_usage (state);
      }
      break;
    default:
      return ARGP_ERR_UNKNOWN;
  }

  return 0;
}

/* Our argp parser. */
static struct argp argp = {options, parse_opt, args_doc, doc, NULL, NULL, NULL};

static const char *name;
static const char *instruction;
static const char *prompts[2];
static char echo[] = { 1, 0 };

static int kbdint_check_response(ssh_session session) {
    int count;

    count = ssh_userauth_kbdint_getnanswers(session);
    if(count != 2) {
        instruction = "Something weird happened :(";
        return 0;
    }
    if(strcasecmp("Arthur Dent",
                        ssh_userauth_kbdint_getanswer(session, 0)) != 0) {
        instruction = "OK, this is not YOUR name, "
                                        "but it's a reference to the HGTG...";
        prompts[0] = "The main character's full name: ";
        return 0;
    }
    if(strcmp("42", ssh_userauth_kbdint_getanswer(session, 1)) != 0) {
        instruction = "Make an effort !!! What is the Answer to the Ultimate "
                            "Question of Life, the Universe, and Everything ?";
        prompts[1] = "Answer to the Ultimate Question of Life, the Universe, "
                            "and Everything: ";
        return 0;
    }

    return 1;
}

static int authenticate(ssh_session session) {
    ssh_message message;

    name = "\n\nKeyboard-Interactive Fancy Authentication\n";
    instruction = "Please enter your real name and your password";
    prompts[0] = "Real name: ";
    prompts[1] = "Password: ";

    do {
        message=ssh_message_get(session);
        if(!message)
            break;
        switch(ssh_message_type(message)){
            case SSH_REQUEST_AUTH:
                switch(ssh_message_subtype(message)){
                    case SSH_AUTH_METHOD_PASSWORD:
                        printf("User %s wants to auth with pass %s\n",
                               ssh_message_auth_user(message),
                               ssh_message_auth_password(message));
                        if(auth_password(ssh_message_auth_user(message),
                           ssh_message_auth_password(message))){
                               ssh_message_auth_reply_success(message,0);
                               ssh_message_free(message);
                               return 1;
                           }
                        ssh_message_auth_set_methods(message,
                                                SSH_AUTH_METHOD_PASSWORD |
                                                SSH_AUTH_METHOD_INTERACTIVE);
                        // not authenticated, send default message
                        ssh_message_reply_default(message);
                        break;

                    case SSH_AUTH_METHOD_INTERACTIVE:
                        if(!ssh_message_auth_kbdint_is_response(message)) {
                            printf("User %s wants to auth with kbdint\n",
                                   ssh_message_auth_user(message));
                            ssh_message_auth_interactive_request(message, name,
                                                    instruction, 2, prompts, echo);
                        } else {
                            if(kbdint_check_response(session)) {
                                ssh_message_auth_reply_success(message,0);
                                ssh_message_free(message);
                                return 1;
                            }
                            ssh_message_auth_set_methods(message,
                                                    SSH_AUTH_METHOD_PASSWORD |
                                                    SSH_AUTH_METHOD_INTERACTIVE);
                            ssh_message_reply_default(message);
                        }
                        break;
                    case SSH_AUTH_METHOD_NONE:
                    default:
                        printf("User %s wants to auth with unknown auth %d\n",
                               ssh_message_auth_user(message),
                               ssh_message_subtype(message));
                        ssh_message_auth_set_methods(message,
                                                SSH_AUTH_METHOD_PASSWORD |
                                                SSH_AUTH_METHOD_INTERACTIVE);
                        ssh_message_reply_default(message);
                        break;
                }
                break;
            default:
                ssh_message_auth_set_methods(message,
                                                SSH_AUTH_METHOD_PASSWORD |
                                                SSH_AUTH_METHOD_INTERACTIVE);
                ssh_message_reply_default(message);
        }
        ssh_message_free(message);
    } while (1);
    return 0;
}

int server_sample(int argc, char **argv){
    ssh_session session;
    ssh_bind sshbind;
    ssh_message message;
    ssh_channel chan=0;
    char buf[2048];
    int auth=0;
    int shell=0;
    int i;
    int r;

    sshbind=ssh_bind_new();
    session=ssh_new();

    ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_DSAKEY,
                                            KEYS_FOLDER "ssh_host_dsa_key");
    ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_RSAKEY,
                                            KEYS_FOLDER "ssh_host_rsa_key");

    /*
     * Parse our arguments; every option seen by parse_opt will
     * be reflected in arguments.
     */
    argp_parse (&argp, argc, argv, 0, 0, sshbind);
    (void) argc;
    (void) argv;
    set_pcap(session);

    if(ssh_bind_listen(sshbind)<0){
        printf("Error listening to socket: %s\n", ssh_get_error(sshbind));
        return 1;
    }
    printf("Started sample libssh sshd on port %d\n", port);
    printf("You can login as the user %s with the password %s\n", SSHD_USER,
                                                            SSHD_PASSWORD);
    r = ssh_bind_accept(sshbind, session);
    if(r==SSH_ERROR){
      printf("Error accepting a connection: %s\n", ssh_get_error(sshbind));
      return 1;
    }
    if (ssh_handle_key_exchange(session)) {
        printf("ssh_handle_key_exchange: %s\n", ssh_get_error(session));
        return 1;
    }

    /* proceed to authentication */
    auth = authenticate(session);
    if(!auth){
        printf("Authentication error: %s\n", ssh_get_error(session));
        ssh_disconnect(session);
        return 1;
    }

    /* wait for a channel session */
    do {
        message = ssh_message_get(session);
        if(message){
            if(ssh_message_type(message) == SSH_REQUEST_CHANNEL_OPEN &&
                    ssh_message_subtype(message) == SSH_CHANNEL_SESSION) {
                chan = ssh_message_channel_request_open_reply_accept(message);
                ssh_message_free(message);
                break;
            } else {
                ssh_message_reply_default(message);
                ssh_message_free(message);
            }
        } else {
            break;
        }
    } while(!chan);

    if(!chan) {
        printf("Error: cleint did not ask for a channel session (%s)\n",
                                                    ssh_get_error(session));
        ssh_finalize();
        return 1;
    }


    /* wait for a shell */
    do {
        message = ssh_message_get(session);
        if(message != NULL) {
            if(ssh_message_type(message) == SSH_REQUEST_CHANNEL &&
                    ssh_message_subtype(message) == SSH_CHANNEL_REQUEST_SHELL) {
                shell = 1;
                ssh_message_channel_request_reply_success(message);
                ssh_message_free(message);
                break;
            }
            ssh_message_reply_default(message);
            ssh_message_free(message);
        } else {
            break;
        }
    } while(!shell);

    if(!shell) {
        printf("Error: No shell requested (%s)\n", ssh_get_error(session));
        return 1;
    }


    printf("it works !\n");
    do{
        i=ssh_channel_read(chan,buf, 2048, 0);
        if(i>0) {
            if(*buf == '' || *buf == '')
                    break;
            if(i == 1 && *buf == '\r')
                ssh_channel_write(chan, "\r\n", 2);
            else
                ssh_channel_write(chan, buf, i);
            if (write(1,buf,i) < 0) {
                printf("error writing to buffer\n");
                return 1;
            }
        }
    } while (i>0);
    ssh_channel_close(chan);
    ssh_disconnect(session);
    ssh_bind_free(sshbind);
    cleanup_pcap();
    ssh_finalize();
    return 0;
}


int verify_knownhost(ssh_session session)
{
  int state, hlen;
  unsigned char *hash = NULL;
  char *hexa;
  char buf[10];

  state = ssh_is_server_known(session);
  hlen = ssh_get_pubkey_hash(session, &hash);
  if (hlen < 0)
    return -1;
  switch (state)
  {
    case SSH_SERVER_KNOWN_OK:
      break; /* ok */
    case SSH_SERVER_KNOWN_CHANGED:
      fprintf(stderr, "Host key for server changed: it is now:\n");
      ssh_print_hexa("Public key hash", hash, hlen);
      fprintf(stderr, "For security reasons, connection will be stopped\n");
      free(hash);
      return -1;
    case SSH_SERVER_FOUND_OTHER:
      fprintf(stderr, "The host key for this server was not found but an other type of key exists.\n");
      fprintf(stderr, "An attacker might change the default server key to"
	      "confuse your client into thinking the key does not exist\n");
      free(hash);
      return -1;
    case SSH_SERVER_FILE_NOT_FOUND:
      fprintf(stderr, "Could not find known host file.\n");
      fprintf(stderr, "If you accept the host key here, the file will be automatically created.\n");
      /* fallback to SSH_SERVER_NOT_KNOWN behavior */
    case SSH_SERVER_NOT_KNOWN:
      hexa = ssh_get_hexa(hash, hlen);
      fprintf(stderr,"The server is unknown. Do you trust the host key?\n");
      fprintf(stderr, "Public key hash: %s\n", hexa);
      free(hexa);
      if (fgets(buf, sizeof(buf), stdin) == NULL)
      {
        free(hash);
        return -1;
      }
      if (strncasecmp(buf, "yes", 3) != 0)
      {
        free(hash);
        return -1;
      }
      if (ssh_write_knownhost(session) < 0)
      {
        fprintf(stderr, "Error %s\n", strerror(errno));
        free(hash);
        return -1;
      }
      break;
    case SSH_SERVER_ERROR:
      fprintf(stderr, "Error %s", ssh_get_error(session));
      free(hash);
      return -1;
  }
  free(hash);
  return 0;
}

int list_home_remote_directory(ssh_session session)
{
  ssh_channel channel;
  int rc;
  char buffer[256];
  int nbytes;

  channel = ssh_channel_new(session);
  if (channel == NULL)
    return SSH_ERROR;
  rc = ssh_channel_open_session(channel);
  if (rc != SSH_OK)
  {
    ssh_channel_free(channel);
    return rc;
  }
  rc = ssh_channel_request_exec(channel, "ls ~/");
  if (rc != SSH_OK)
  {
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    return rc;
  }
  nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
  while (nbytes > 0)
  {
    if (write(1, buffer, nbytes) != (unsigned int) nbytes)
    {
      ssh_channel_close(channel);
      ssh_channel_free(channel);
      return SSH_ERROR;
    }
    nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
  }

  if (nbytes < 0)
  {
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    return SSH_ERROR;
  }
  ssh_channel_send_eof(channel);
  ssh_channel_close(channel);
  ssh_channel_free(channel);
  return SSH_OK;
}

int client_sample()
{
  ssh_session my_ssh_session = NULL;
  int verbosity = SSH_LOG_PROTOCOL;
  int port = 22;
  int rc = SSH_ERROR;
  char *password = NULL;

  my_ssh_session = ssh_new();
  if (my_ssh_session == NULL)
    {
      printf("ssh_new() failure\n");
      return (-1);
    }
  printf("ssh_new() success!\n");

  if (ssh_options_set(my_ssh_session, SSH_OPTIONS_HOST, "localhost") != 0 ||
      ssh_options_set(my_ssh_session, SSH_OPTIONS_LOG_VERBOSITY, &verbosity) != 0 ||
      ssh_options_set(my_ssh_session, SSH_OPTIONS_PORT, &port) != 0)
    {
      fprintf(stderr, "Error connecting to localhost: %s\n", ssh_get_error(my_ssh_session));
      return (-1);
    }
  printf("ssh_options_set() success!\n");

  rc = ssh_connect(my_ssh_session);
  if (rc != SSH_OK)
    {
      fprintf(stderr, "Error connecting to localhost: %s\n", ssh_get_error(my_ssh_session));
      return (-1);
    }
  printf("ssh_connect() success!\n");

  // Verify the server's identity
  if (verify_knownhost(my_ssh_session) < 0)
  {
    ssh_disconnect(my_ssh_session);
    ssh_free(my_ssh_session);
    return (-1);
  }

  // Authenticate ourselves
  password = getpass("Password: ");
  rc = ssh_userauth_password(my_ssh_session, NULL, password);
  if (rc != SSH_AUTH_SUCCESS)
  {
    fprintf(stderr, "Error authenticating with password: %s\n", ssh_get_error(my_ssh_session));
    ssh_disconnect(my_ssh_session);
    ssh_free(my_ssh_session);
    return (-1);
  }
  printf("ssh_userauth_password() success!\n");

  list_home_remote_directory(my_ssh_session);

  ssh_disconnect(my_ssh_session);
  ssh_free(my_ssh_session);
  return (0);
}

int main(int argc, char **argv){
  server_sample(argc, argv);
  client_sample();
  return 0;
}

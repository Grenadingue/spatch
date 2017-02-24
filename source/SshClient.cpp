#include <libssh/libssh.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "SshClient.hpp"

#include <vector>
#include <string>
#include <iostream>

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

SshClient::SshClient(const Endpoint &endpoint, const std::string &username, const char *command)
    : _endpoint(endpoint), _username(username), _command(command)
{
}

SshClient::~SshClient()
{
}

void SshClient::connect()
{
    ssh_session my_ssh_session = NULL;
    int verbosity = SSH_LOG_NONE;
    int rc = SSH_ERROR;

    my_ssh_session = ssh_new();
    if (my_ssh_session == NULL)
      {
        printf("ssh_new() failure\n");
        return;
      }

    if (ssh_options_set(my_ssh_session, SSH_OPTIONS_HOST, _endpoint.ipAdress.c_str()) != 0 ||
        // ssh_options_set(my_ssh_session, SSH_OPTIONS_LOG_VERBOSITY_STR, "1") != 0 ||
        ssh_options_set(my_ssh_session, SSH_OPTIONS_LOG_VERBOSITY, &verbosity) != 0 ||
        ssh_options_set(my_ssh_session, SSH_OPTIONS_PORT_STR, _endpoint.port.c_str()) != 0 ||
        ssh_options_set(my_ssh_session, SSH_OPTIONS_USER, _username.c_str()) != 0)
      {
        fprintf(stderr, "Error connecting to remote host: %s\n", ssh_get_error(my_ssh_session));
        return;
      }

    rc = ssh_connect(my_ssh_session);
    if (rc != SSH_OK)
      {
        fprintf(stderr, "Error connecting to localhost: %s\n", ssh_get_error(my_ssh_session));
        return;
      }

    // Verify the server's identity
    if (verify_knownhost(my_ssh_session) < 0)
    {
      ssh_disconnect(my_ssh_session);
      ssh_free(my_ssh_session);
      return;
    }

    std::string configFolder = "/etc/spatch/";
    std::vector<std::string> keys = { "id_rsa", "id_dsa" };
    ssh_key pubkey = NULL, privkey = NULL;
    bool authenticated = false;

    for (auto key : keys)
    {
        const std::string privateKeyPath = configFolder + key + "_" + _endpoint.name;
        const std::string publicKeyPath = privateKeyPath + ".pub";

        if (ssh_pki_import_pubkey_file(publicKeyPath.c_str(), &pubkey) == SSH_OK)
        {
            if (ssh_userauth_try_publickey(my_ssh_session, NULL, pubkey) == SSH_AUTH_SUCCESS)
            {
                if (ssh_pki_import_privkey_file(privateKeyPath.c_str(), NULL, NULL, NULL, &privkey) == SSH_OK)
                {
                    if (ssh_userauth_publickey(my_ssh_session, NULL, privkey) == SSH_AUTH_SUCCESS)
                    {
                        authenticated = true;
                        break;
                    }
                }
            }
        }
    }

    if (!authenticated)
    {
      fprintf(stderr, "Error authenticating: %s\n", ssh_get_error(my_ssh_session));
      ssh_disconnect(my_ssh_session);
      ssh_free(my_ssh_session);
      return;
    }

    list_home_remote_directory(my_ssh_session);

    ssh_disconnect(my_ssh_session);
    ssh_free(my_ssh_session);
    ssh_key_free(pubkey);
    ssh_key_free(privkey);
}

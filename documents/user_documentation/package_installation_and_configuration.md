## Explanation :
You can find the config file in ```./config/spatch/config.ini```.

> Please respect the configuration rules otherwise the configuration reader or the configuration controller will exit in error.

You can specify some configuration by sections like :
  - server :
```
    port=42
```
  - local_users :
```
    username=password
```
  - endpoints :
```
    endpoint_name=ip_address|port
```
  - users_control :
```
    local_username=available_endpoint_1|available_endpoint_2|...
```
  - remote_users :
```
    local_username|endpoint_name=available_username_1|available_username_2|...
```

## Example :
```
[server]
port=42

[local_users]
toto=toto
...

[endpoints]
endpoint1=10.0.2.15|22
...

[users_control]
toto=endpoint1|endpoint2
...

[remote_users]
toto|endpoint1=toto|root
...
```

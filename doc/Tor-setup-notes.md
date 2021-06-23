# c-lightning Tor Setup

- https://lightning.readthedocs.io/TOR.html
- https://2019.www.torproject.org/docs/tor-onion-service.html.en
- https://github.com/satindergrewal/lightning/blob/grewal/doc/TOR.md


### Install tor with

```bash
sudo apt update
sudo apt install tor
```

### Edit Tor's config file

```bash
sudo nano /etc/tor/torrc

# Uncomment following lines
ExitPolicy reject *:* # no exits allowed

ControlPort 9051
CookieAuthentication 1

# Also add this
CookieAuthFileGroupReadable 1
```

### Add user which will be running lightingd, to tor service's user group

```bash
# Make environment variables to temporarily store the user groups info

export TORGROUP=$(stat -c '%G' /run/tor/control.authcookie)
export LIGHTNINGUSER=$(whoami)

# Add tor service's user group to lightingd running user's group (i.e debian-tor).
# In our case the same user logged in currently (i.e. satinder) will be running lightningd.
# So, adding adding satinder to debian-tor group.
sudo usermod -a -G $TORGROUP $LIGHTNINGUSER

# Or you can also use this command directly
sudo usermod -a -G debian-tor $(whoami)
```

### Logout/Login, or Restart
`groups` command should return torgroup in result after restart

```bash
satinder@ubuntu:~$ groups
satinder adm cdrom sudo dip plugdev lpadmin sambashare debian-tor
```

### Making sure Tor setup works

```bash
# Following command should just return nothing. If error, setup is not done correct
satinder@ubuntu:~$ cat /run/tor/control.authcookie > /dev/null
satinder@ubuntu:~$
```

If for some reason the above command does not return a blank result, your tor setup is not perfect.
And it may cause issues with setting up and making lightning network daemon and chips daemon service over tor.

Like, for me in my testing, [Proxmox Container](https://pve.proxmox.com/wiki/Linux_Container) was not reflecting the expected result of the above test command. The auth cookie file which should have been present at the expected location was not present.

```bash
satinder@ubuntu:~$ ls -lh /run/tor/control.authcookie
ls: cannot access '/run/tor/control.authcookie': No such file or directory
satinder@ubuntu:~$ sudo ls -lh /run/tor/control.authcookie
ls: cannot access '/run/tor/control.authcookie': No such file or directory
```

And the following solution was tested and provided by a community member, documented in this [github issue](https://github.com/satindergrewal/lightning/issues/17).

```bash
sudo apt -y install apparmor-utils

# Setting USE_AA_EXEC to no, if it's setup to yes (default value)
if (grep 'USE_AA_EXEC="no"' /etc/default/tor > /dev/null 2>&1); then
echo 'USE_AA_EXEC="no"' | sudo tee --append /etc/default/tor
fi

# Update tor app permissions
sudo aa-complain system_tor

# Restar tor servic and check if it's running fine
sudo systemctl restart tor
sudo systemctl status tor
```

After these changes the authcookie file shows fine, and we can proceed with our further setup

```bash
satinder@ubuntu:~$ sudo ls -lh /run/tor/control.authcookie
-rw-r----- 1 debian-tor debian-tor 32 Jun  6 13:05 /run/tor/control.authcookie
```

### After editing the /etc/tor/torrc file looks like this

```bash
ControlPort 9051
CookieAuthentication 1
CookieAuthFileGroupReadable 1

# This is the lightningd service setup with tor hidden service
HiddenServiceDir /var/lib/tor/lightningd-service_v3/
HiddenServiceVersion 3
HiddenServicePort 9735 127.0.0.1:9735
```

### Restart Tor service and check the onion v3 address

```bash
# Restart Tor service
satinder@ubuntu:~$ sudo systemctl restart tor

# Check if Tor service is running fine
satinder@ubuntu:~$ sudo systemctl status tor
● tor.service - Anonymizing overlay network for TCP (multi-instance-master)
   Loaded: loaded (/lib/systemd/system/tor.service; enabled; vendor preset: enabled)
   Active: active (exited) since Sun 2021-05-30 07:54:51 PDT; 1h 20min ago
  Process: 6880 ExecStart=/bin/true (code=exited, status=0/SUCCESS)
 Main PID: 6880 (code=exited, status=0/SUCCESS)

May 30 07:54:51 ubuntu systemd[1]: Stopped Anonymizing overlay network for TCP (multi-instance-master).
May 30 07:54:51 ubuntu systemd[1]: Stopping Anonymizing overlay network for TCP (multi-instance-master)...
May 30 07:54:51 ubuntu systemd[1]: Starting Anonymizing overlay network for TCP (multi-instance-master)...
May 30 07:54:51 ubuntu systemd[1]: Started Anonymizing overlay network for TCP (multi-instance-master).

# Check the newly generated onion v3 address for lightningd service
satinder@ubuntu:~$ sudo cat /var/lib/tor/lightningd-service_v3/hostname
4rn3inojrpfnre7ewaizrcdeqopy6dcgjyso5nmqu76r72fa2xmchvqd.onion 
```

### Add following to config
Get your `.onion` address as shown earlier and use that in the lightning config file.

```bash
mkdir -p ~/.chipsln/chips/
nano ~/.chipsln/chips/config

proxy=127.0.0.1:9050
bind-addr=127.0.0.1:9735
addr=4rn3inojrpfnre7ewaizrcdeqopy6dcgjyso5nmqu76r72fa2xmchvqd.onion:9735
always-use-proxy=true
```

### At the end just start lightningd server like before

```bash
satinder@ubuntu:~/lightning$ ./lightningd/lightningd --log-level debug
2021-06-01T19:39:41.276Z DEBUG   plugin-manager: started(36316) /home/satinder/lightning/lightningd/../plugins/autoclean
2021-06-01T19:39:41.278Z DEBUG   plugin-manager: started(36317) /home/satinder/lightning/lightningd/../plugins/bcli
2021-06-01T19:39:41.279Z DEBUG   plugin-manager: started(36318) /home/satinder/lightning/lightningd/../plugins/fetchinvoice
2021-06-01T19:39:41.280Z DEBUG   plugin-manager: started(36319) /home/satinder/lightning/lightningd/../plugins/keysend
2021-06-01T19:39:41.290Z DEBUG   plugin-manager: started(36320) /home/satinder/lightning/lightningd/../plugins/offers
2021-06-01T19:39:41.293Z DEBUG   plugin-manager: started(36321) /home/satinder/lightning/lightningd/../plugins/pay
2021-06-01T19:39:41.294Z DEBUG   plugin-manager: started(36322) /home/satinder/lightning/lightningd/../plugins/txprepare
2021-06-01T19:39:41.296Z DEBUG   plugin-manager: started(36323) /home/satinder/lightning/lightningd/../plugins/spenderp
2021-06-01T19:39:41.311Z UNUSUAL lightningd: You used `--addr=4rn3inojrpfnre7ewaizrcdeqopy6dcgjyso5nmqu76r72fa2xmchvqd.onion:9735` option with an .onion address, please use `--announce-addr` ! You are lucky in this node live some wizards and fairies, we have done this for you and announce, Be as hidden as wished
2021-06-01T19:39:41.312Z DEBUG   lightningd: testing /home/satinder/lightning/lightningd/lightning_channeld
2021-06-01T19:39:41.315Z DEBUG   lightningd: testing /home/satinder/lightning/lightningd/lightning_closingd
2021-06-01T19:39:41.316Z DEBUG   lightningd: testing /home/satinder/lightning/lightningd/lightning_connectd
2021-06-01T19:39:41.318Z DEBUG   lightningd: testing /home/satinder/lightning/lightningd/lightning_gossipd
2021-06-01T19:39:41.320Z DEBUG   lightningd: testing /home/satinder/lightning/lightningd/lightning_hsmd
2021-06-01T19:39:41.323Z DEBUG   lightningd: testing /home/satinder/lightning/lightningd/lightning_onchaind
2021-06-01T19:39:41.325Z DEBUG   lightningd: testing /home/satinder/lightning/lightningd/lightning_openingd
2021-06-01T19:39:41.327Z DEBUG   hsmd: pid 36331, msgfd 31
2021-06-01T19:39:41.343Z DEBUG   connectd: pid 36332, msgfd 35
2021-06-01T19:39:41.343Z DEBUG   hsmd: Client: Received message 11 from client
2021-06-01T19:39:41.343Z DEBUG   hsmd: Client: Received message 9 from client
2021-06-01T19:39:41.343Z DEBUG   hsmd: new_client: 0
2021-06-01T19:39:41.353Z DEBUG   connectd: Proxy address: 127.0.0.1:9050
2021-06-01T19:39:41.353Z DEBUG   connectd: Created IPv4 listener on port 9735
2021-06-01T19:39:41.353Z DEBUG   connectd: REPLY WIRE_CONNECTD_INIT_REPLY with 0 fds
2021-06-01T19:39:41.353Z DEBUG   gossipd: pid 36333, msgfd 34
topo 0
2021-06-01T19:39:41.358Z DEBUG   hsmd: Client: Received message 9 from client
2021-06-01T19:39:41.358Z DEBUG   hsmd: new_client: 0
2021-06-01T19:39:41.364Z INFO    plugin-bcli: bitcoin-cli initialized and connected to bitcoind.
topo 1
2021-06-01T19:39:41.364Z DEBUG   lightningd: All Bitcoin plugin commands registered
topo 2
2021-06-01T19:39:41.365Z DEBUG   gossipd: gossip_store_compact_offline: 0 deleted, 0 copied
2021-06-01T19:39:41.365Z DEBUG   gossipd: total store load time: 0 msec
2021-06-01T19:39:41.365Z DEBUG   gossipd: gossip_store: Read 0/0/0/0 cannounce/cupdate/nannounce/cdelete from store (0 deleted) in 1 bytes
2021-06-01T19:39:41.365Z DEBUG   gossipd: seeker: state = STARTING_UP New seeker
2021-06-01T19:39:41.393Z DEBUG   lightningd: Smoothed feerate estimate for opening initialized to polled estimate 12500
2021-06-01T19:39:41.393Z DEBUG   lightningd: Feerate estimate for opening set to 12500 (was 0)
2021-06-01T19:39:41.393Z DEBUG   lightningd: Smoothed feerate estimate for mutual_close initialized to polled estimate 12500
2021-06-01T19:39:41.393Z DEBUG   lightningd: Feerate estimate for mutual_close set to 12500 (was 0)
2021-06-01T19:39:41.393Z DEBUG   lightningd: Smoothed feerate estimate for unilateral_close initialized to polled estimate 12500
2021-06-01T19:39:41.393Z DEBUG   lightningd: Feerate estimate for unilateral_close set to 12500 (was 0)
2021-06-01T19:39:41.393Z DEBUG   lightningd: Smoothed feerate estimate for delayed_to_us initialized to polled estimate 12500
2021-06-01T19:39:41.393Z DEBUG   lightningd: Feerate estimate for delayed_to_us set to 12500 (was 0)
2021-06-01T19:39:41.393Z DEBUG   lightningd: Smoothed feerate estimate for htlc_resolution initialized to polled estimate 12500
2021-06-01T19:39:41.393Z DEBUG   lightningd: Feerate estimate for htlc_resolution set to 12500 (was 0)
2021-06-01T19:39:41.393Z DEBUG   lightningd: Smoothed feerate estimate for penalty initialized to polled estimate 12500
2021-06-01T19:39:41.393Z DEBUG   lightningd: Feerate estimate for penalty set to 12500 (was 0)
2021-06-01T19:39:41.393Z DEBUG   lightningd: Smoothed feerate estimate for min_acceptable initialized to polled estimate 6250
2021-06-01T19:39:41.393Z DEBUG   lightningd: Feerate estimate for min_acceptable set to 6250 (was 0)
2021-06-01T19:39:41.393Z DEBUG   lightningd: Smoothed feerate estimate for max_acceptable initialized to polled estimate 125000
2021-06-01T19:39:41.393Z DEBUG   lightningd: Feerate estimate for max_acceptable set to 125000 (was 0)
2021-06-01T19:39:41.436Z DEBUG   lightningd: Adding block 8090742: 000000506a7e4df9d0d9ef2c89b5d38bf0a65613f0333a989837019cd1f64a48
2021-06-01T19:39:41.438Z DEBUG   wallet: Loaded 0 channels from DB
2021-06-01T19:39:41.439Z DEBUG   plugin-autoclean: autocleaning not active
2021-06-01T19:39:41.439Z DEBUG   connectd: REPLY WIRE_CONNECTD_ACTIVATE_REPLY with 0 fds
2021-06-01T19:39:41.439Z INFO    lightningd: --------------------------------------------------
2021-06-01T19:39:41.439Z INFO    lightningd: Server started with public key 03e45318266315810eaf9f53e4ecb4dd683ec3d274574e823ff5b81a7d9ab093f6, alias VIOLENTMASTER (color #03e453) and lightningd chipsln.0.0.0
2021-06-01T19:39:41.440Z DEBUG   plugin-fetchinvoice: Killing plugin: disabled itself at init: offers not enabled in config
2021-06-01T19:39:41.440Z DEBUG   plugin-offers: Killing plugin: disabled itself at init: offers not enabled in config
2021-06-01T19:39:41.452Z DEBUG   lightningd: Adding block 8090743: 0000003e4cca5e26fe0d0ce842aa0d3640996c32429afbabe205932667acc015
2021-06-01T19:39:41.466Z DEBUG   lightningd: Adding block 8090744: 00000029ea741b67a84d2524ee6c5bf476776941aea8a378a87af6573cb430da
2021-06-01T19:39:41.480Z DEBUG   lightningd: Adding block 8090745: 0000000b95db2ffd75816f6bc57ced974fe2379295e6dd062f126fb0291e1534
2021-06-01T19:39:41.493Z DEBUG   lightningd: Adding block 8090746: 00000020a33e7f8cf3ab29acddb5505de5c5304671178950240e5292c25efb4a
2021-06-01T19:39:41.506Z DEBUG   lightningd: Adding block 8090747: 0000000da7f4c278e5f2fea59f7f8874564e66a9a1925a4d55fe3d9b22cc8370
2021-06-01T19:39:41.517Z DEBUG   lightningd: Adding block 8090748: 00000058aaac43a81193cfc121d7b2e52a2eaf55548c2907ded567ef3aade53b
2021-06-01T19:39:41.530Z DEBUG   lightningd: Adding block 8090749: 00000034c6e38829de3a4c14338e49cd1193a1f5673ee461210ec4ff4980ecf5
2021-06-01T19:39:41.541Z DEBUG   lightningd: Adding block 8090750: 000000048c43bbe0efba94c9834b1208ad2c8fb08fb59cfaf17aebb2fdd06802
2021-05-30T15:54:47.542Z DEBUG   gossipd: seeker: no peers, waiting
```


# Chips Tor Setup

- https://bitcoin.stackexchange.com/questions/70069/how-can-i-setup-bitcoin-to-be-anonymous-with-tor
- https://en.bitcoin.it/wiki/Setting_up_a_Tor_hidden_service
- https://github.com/chips-blockchain/chips/blob/master/doc/tor.md

If you configured `c-lightning` successfully with Tor, just add the following in `/etc/tor/torrc` Hidden Services section, just under where you configured `c-lightning` tor hidden service:

```shell
# Tor hidden service for Chips blockchain
# Since Chips is old code fork from Bitcoin-Core, it only supports
# version 2 of onion addresses, which are soon to
# be obsolete https://blog.torproject.org/v2-deprecation-timeline
# The ideal solution would be to also upgrade Chips-core software
# to Bitcoin-Core's latest codebase to add latest upstream code support
HiddenServiceDir /var/lib/tor/chips-service/
HiddenServiceVersion 2
HiddenServicePort 57777 127.0.0.1:57777
```

### Get Chips onion hidden service address
Once above changes are done to `torrc`, restart `tor` service and get the generated v2 onion address for Chips hidden service:

```shell
# Restart tor service
sudo systemctl restart tor

# Get onion address for Chips hidden service
sudo cat /var/lib/tor/chips-service/hostname
d4mc5ld3jkyo5or2.onion
```

### Setup chips.conf to connect through tor

```shell
# Edit chips.conf
nano ~/.chips/chips.conf

# Then add the following at the end of this file

proxy=127.0.0.1:9050
listen=1
bind=127.0.0.1
externalip=d4mc5ld3jkyo5or2.onion
```

### Additional options for `chips.conf`
If you want to connect your Chips node only through tor completly and don't want any connections to be made over public network add the following
But this will require you to add .onion seed nodes too. Otherwise you'll most probably have issues getting peers to connect to.

```shell
onlynet=onion

# Chips tor nodes
addnode=4l75z2ai3jyapzlm.onion
addnode=xtwgiapr5tzmigjk.onion
addnode=qtwjcqfyaq6db3yb.onion
addnode=3ds2xm2yhccayswh.onion
addnode=syjmeab77as4hyhj.onion
addnode=dljbtak3s6ki675v.onion
addnode=fnfrbsv3d3mia4u4.onion
addnode=z3f3f4amdbmrrawz.onion
addnode=f3qvjq5fojyikeqi.onion
addnode=7tt3u6lkg63kizc2.onion
addnode=5asiwieufupdnqem.onion
addnode=ysthaqh7bpspaqgf.onion
addnode=c7ilhzjjcybt4pg5.onion
addnode=mrgntmhdg5bhed65.onion
addnode=abkflu3l7vr67ews.onion

# Chips tor seed nodes
seednode=4l75z2ai3jyapzlm.onion
seednode=xtwgiapr5tzmigjk.onion
seednode=qtwjcqfyaq6db3yb.onion
seednode=3ds2xm2yhccayswh.onion
seednode=syjmeab77as4hyhj.onion
seednode=dljbtak3s6ki675v.onion
seednode=fnfrbsv3d3mia4u4.onion
seednode=z3f3f4amdbmrrawz.onion
seednode=f3qvjq5fojyikeqi.onion
seednode=7tt3u6lkg63kizc2.onion
seednode=5asiwieufupdnqem.onion
seednode=ysthaqh7bpspaqgf.onion
seednode=c7ilhzjjcybt4pg5.onion
seednode=mrgntmhdg5bhed65.onion
seednode=abkflu3l7vr67ews.onion
```

Once `chips.conf` is setup, restart your `chipsd` daemon.

After editing your `chips.conf` will look like this:

```shell
server=1
daemon=1
txindex=1
rpcuser=chipsuser
rpcpassword=YOUR_RPC_PASSWORD
rpcbind=127.0.0.1
rpcallowip=127.0.0.1
proxy=127.0.0.1:9050
listen=1
bind=127.0.0.1
onlynet=onion
discover=1
externalip=d4mc5ld3jkyo5or2.onion
# Chips tor nodes
addnode=4l75z2ai3jyapzlm.onion
addnode=xtwgiapr5tzmigjk.onion
addnode=qtwjcqfyaq6db3yb.onion
addnode=3ds2xm2yhccayswh.onion
addnode=syjmeab77as4hyhj.onion
addnode=dljbtak3s6ki675v.onion
addnode=fnfrbsv3d3mia4u4.onion
addnode=z3f3f4amdbmrrawz.onion
addnode=f3qvjq5fojyikeqi.onion
addnode=7tt3u6lkg63kizc2.onion
addnode=5asiwieufupdnqem.onion
addnode=ysthaqh7bpspaqgf.onion
addnode=c7ilhzjjcybt4pg5.onion
addnode=mrgntmhdg5bhed65.onion
addnode=abkflu3l7vr67ews.onion

# Chips tor seed nodes
seednode=4l75z2ai3jyapzlm.onion
seednode=xtwgiapr5tzmigjk.onion
seednode=qtwjcqfyaq6db3yb.onion
seednode=3ds2xm2yhccayswh.onion
seednode=syjmeab77as4hyhj.onion
seednode=dljbtak3s6ki675v.onion
seednode=fnfrbsv3d3mia4u4.onion
seednode=z3f3f4amdbmrrawz.onion
seednode=f3qvjq5fojyikeqi.onion
seednode=7tt3u6lkg63kizc2.onion
seednode=5asiwieufupdnqem.onion
seednode=ysthaqh7bpspaqgf.onion
seednode=c7ilhzjjcybt4pg5.onion
seednode=mrgntmhdg5bhed65.onion
seednode=abkflu3l7vr67ews.onion
```

### Verify Chips tor connection
To verify if you are able are getting blocks from the network and your node is connected through tor, you can use the command `getnetworkinfo`:

```shell
pi@raspberrypi:~ $ chips-cli getnetworkinfo
{
  "version": 169900,
  "subversion": "/Satoshi:0.16.99/",
  "protocolversion": 70016,
  "localservices": "000000000000040d",
  "localrelay": true,
  "timeoffset": 0,
  "networkactive": true,
  "connections": 9,
  "networks": [
    {
      "name": "ipv4",
      "limited": false,
      "reachable": true,
      "proxy": "127.0.0.1:9050",
      "proxy_randomize_credentials": true
    },
    {
      "name": "ipv6",
      "limited": false,
      "reachable": true,
      "proxy": "127.0.0.1:9050",
      "proxy_randomize_credentials": true
    },
    {
      "name": "onion",
      "limited": false,
      "reachable": true,
      "proxy": "127.0.0.1:9050",
      "proxy_randomize_credentials": true
    }
  ],
  "relayfee": 0.00001000,
  "incrementalfee": 0.00001000,
  "localaddresses": [
    {
      "address": "d4mc5ld3jkyo5or2.onion",
      "port": 57777,
      "score": 4
    }
  ],
  "warnings": "This is a pre-release test build - use at your own risk - do not use for mining or merchant applications"
}
```

You can also check latest updates in chips' `debug.log` file:

```shell
pi@raspberrypi:~ $ tail -f ~/.chips/debug.log
2021-06-03T12:44:29Z UpdateTip: new best=0000000bb2345c98b7ce36f0a020cc9aad23b894bb88e6cc151f4ef06706abd3 height=8104344 version=0x20000000 log2_work=75.981897 tx=8359473 date='2021-06-03T12:44:31Z' progress=1.000000 cache=0.0MiB(204txo)
2021-06-03T12:44:37Z UpdateTip: new best=00000028a6f69f30c018c52acdfdf7737d360b013621e7e7605245414e73942a height=8104345 version=0x20000000 log2_work=75.981897 tx=8359474 date='2021-06-03T12:44:35Z' progress=1.000000 cache=0.0MiB(205txo)
2021-06-03T12:44:40Z UpdateTip: new best=0000003922e6d642aeaf96f4cd9438b4c2e48c09d6438847bf0ad459db792d3c height=8104346 version=0x20000000 log2_work=75.981897 tx=8359475 date='2021-06-03T12:44:40Z' progress=1.000000 cache=0.0MiB(206txo)
2021-06-03T12:44:48Z UpdateTip: new best=00000007281bd67c1ef14366f2e81ecaaa8ffd16f2422b515f6a03d066de0d92 height=8104347 version=0x20000000 log2_work=75.981897 tx=8359476 date='2021-06-03T12:44:47Z' progress=1.000000 cache=0.0MiB(207txo)
2021-06-03T12:44:57Z Socks5() connect to 95.179.192.102:57777 failed: general failure
2021-06-03T12:45:19Z UpdateTip: new best=0000000ec6915a165f94d0b72049ec35ebb00c61709f6fba50001bb91568af13 height=8104348 version=0x20000000 log2_work=75.981897 tx=8359477 date='2021-06-03T12:45:19Z' progress=1.000000 cache=0.0MiB(208txo)
2021-06-03T12:45:21Z Socks5() connect to 145.239.149.173:57777 failed: general failure
2021-06-03T12:45:21Z UpdateTip: new best=00000032d47a18a79e325a662287ecca810ecfbf346d6908c493b41e185734af height=8104349 version=0x20000000 log2_work=75.981897 tx=8359478 date='2021-06-03T12:45:20Z' progress=1.000000 cache=0.0MiB(209txo)
2021-06-03T12:45:24Z UpdateTip: new best=000000172cb30e52f653a48803e4e040b745b1aa96b58055ce367296f5337ceb height=8104350 version=0x20000000 log2_work=75.981897 tx=8359479 date='2021-06-03T12:45:23Z' progress=1.000000 cache=0.0MiB(210txo)
2021-06-03T12:45:25Z Socks5() connect to 178.63.53.110:57777 failed: general failure
^C
```

It will keep updating with new blocks coming in from the network. To cancel just press `CTRL+C` to exit from this process.


# Tor Monitoring service(s)

You can try Nyx after you setup your tor hidden services to monitor Tor specific network traffic:
https://nyx.torproject.org/#torrc

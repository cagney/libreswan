ipsec route westnet-eastnet
taskset 0x2 ../../guestbin/ping-once.sh --fire-and-forget -I 192.0.1.254 192.0.2.254
echo done

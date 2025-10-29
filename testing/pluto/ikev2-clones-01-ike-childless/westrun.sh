ipsec up westnet-eastnet
taskset 0x1 ../../guestbin/ping-once.sh --fire-and-forget -I 192.0.1.254 192.0.2.254
echo done

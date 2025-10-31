ipsec up westnet-eastnet
 ../../guestbin/ping-once.sh --up -I 192.0.1.254 192.0.2.254
ipsec _kernel state
ipsec _kernel policy
ipsec trafficstatus

taskset 0x1 ../../guestbin/ping-once.sh --fire-and-forget -I 192.0.1.254 192.0.2.254
sleep 5
echo done

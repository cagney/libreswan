east# /testing/guestbin/swan-prep --46
east# ipsec start
east# ../../guestbin/wait-until-pluto-started
east# ipsec auto --add eastnet-westnet-ikev2
east# ipsec whack --impair suppress-retransmits
east# echo "initdone"
freebsdw# ../../guestbin/netbsd-prep.sh # note: swan-prep does not yet support BSD
freebsdw# ipsec start
freebsdw# ipsec auto --add eastnet-westnet-ikev2
freebsdw# echo "initdone"
freebsdw# ../../guestbin/ping-once.sh --down -I 192.0.1.254 192.0.2.254
freebsdw# ipsec auto --up eastnet-westnet-ikev2
freebsdw# ../../guestbin/ipsec-kernel-policy.sh
freebsdw# ../../guestbin/ping-once.sh --up -I 192.0.1.254 192.0.2.254
freebsdw# ../../guestbin/ipsec-kernel-state.sh
freebsdw# ../../guestbin/ping-once.sh --medium --up -I 192.0.1.254 192.0.2.254
freebsdw# ../../guestbin/ipsec-kernel-state.sh
freebsdw# dmesg | grep ipsec

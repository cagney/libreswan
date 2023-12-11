east# /testing/guestbin/swan-prep --46
east# ipsec start
east# ../../guestbin/wait-until-pluto-started
east# ipsec auto --add eastnet-westnet-ikev2
east# ipsec whack --impair suppress-retransmits
east# echo "initdone"
netbsdw# ../../guestbin/netbsd-prep.sh # note: swan-prep does not yet support BSD
netbsdw# ipsec start
netbsdw# ipsec auto --add eastnet-westnet-ikev2
netbsdw# echo "initdone"
netbsdw# ipsec auto --up eastnet-westnet-ikev2
netbsdw# ../../guestbin/ipsec-kernel-policy.sh
netbsdw# ../../guestbin/ping-once.sh --up 192.1.2.23
netbsdw# ../../guestbin/ipsec-kernel-state.sh
netbsdw# ../../guestbin/ping-once.sh --medium --up 192.1.2.23
netbsdw# ../../guestbin/ipsec-kernel-state.sh
netbsdw# dmesg | grep ipsec

east# /testing/guestbin/swan-prep --46
east# ipsec start
east# ../../guestbin/wait-until-pluto-started
east# ipsec auto --add eastnet-westnet-ikev2
east# ipsec whack --impair suppress-retransmits
netbsdw# ../../guestbin/netbsd-prep.sh # note: swan-prep does not yet support BSD
netbsdw# ipsec start
netbsdw# ipsec auto --add eastnet-westnet-ikev2
netbsdw# echo "initdone"
netbsdw# ../../guestbin/ping-once.sh --down -I 2001:db8:0:1::254 2001:db8:0:2::254
netbsdw# ipsec auto --up eastnet-westnet-ikev2
netbsdw# ../../guestbin/ipsec-kernel-policy.sh
netbsdw# ../../guestbin/ping-once.sh --up -I 2001:db8:0:1::254 2001:db8:0:2::254
netbsdw# ../../guestbin/ipsec-kernel-state.sh
netbsdw# ../../guestbin/ping-once.sh --medium --up -I 2001:db8:0:1::254 2001:db8:0:2::254
netbsdw# ../../guestbin/ipsec-kernel-state.sh
netbsdw# dmesg | grep ipsec

east# /testing/guestbin/swan-prep --46
east# ipsec start
east# ../../guestbin/wait-until-pluto-started
east# ipsec whack --impair suppress-retransmits
east# echo "initdone"
netbsdw# ../../guestbin/netbsd-prep.sh # note: swan-prep does not yet support BSD
netbsdw# ipsec start
netbsdw# ipsec auto --add eastnet-westnet-ikev2
netbsdw# echo "initdone"
netbsdw# ipsec auto --route eastnet-westnet-ikev2
netbsdw# ../../guestbin/ipsec-kernel-state.sh
netbsdw# ../../guestbin/ipsec-kernel-policy.sh
netbsdw# ../../guestbin/ping-once.sh --up -I 192.0.1.254 192.0.2.254
netbsdw# ../../guestbin/ping-once.sh --up -I 192.0.1.254 192.0.2.254


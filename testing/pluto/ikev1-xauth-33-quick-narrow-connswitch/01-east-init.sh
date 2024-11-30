../../guestbin/swan-prep --x509
ipsec start
../../guestbin/wait-until-pluto-started
ipsec auto --add wide-to-east
ipsec auto --add narrow-to-east
ipsec whack --impair suppress_retransmits
echo initdone

/testing/guestbin/swan-prep --x509
ipsec start
../../guestbin/wait-until-pluto-started
ipsec add any-east
ipsec whack --impair suppress_retransmits
echo initdone
